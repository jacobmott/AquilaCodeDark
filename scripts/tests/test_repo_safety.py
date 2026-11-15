import argparse
import hashlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import asset_store as store
import check_git_text as guard


class SafetyTests(unittest.TestCase):
    def test_restore_path_traversal_and_windows_aliases(self):
        for name in ('../Secrets/key', '/outside', 'C:/outside', 'assets/../bad', 'NUL.txt', 'a\\b', 'a/stream:ads', 'a./b'):
            with self.subTest(name=name), self.assertRaises(ValueError):
                store.safe_relative(name)

    def test_backup_excludes_sensitive_and_scratch_files(self):
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            for name in ('Assets/ship.png', 'Assets/.env', 'Assets/Secrets/key.txt', 'Assets/Diagnostics/frame.png'):
                p = root / name
                p.parent.mkdir(parents=True, exist_ok=True)
                p.write_bytes(b'sample')
            rows = store.inventory(root, ['Assets'], 'assets')
            self.assertEqual([r['path'] for r in rows], ['Assets/ship.png'])

    def test_existing_remote_conflict_stops_before_write(self):
        class Client:
            def head_object(self, **kw):
                return {'ContentLength': 5, 'Metadata': {'sha256': '0'*64}}
            def put_object(self, **kw):
                raise AssertionError('Conflict must not write')
        with tempfile.TemporaryDirectory() as raw:
            root = Path(raw)
            (root/'Assets').mkdir()
            (root/'Assets/a.txt').write_text('hello')
            args = argparse.Namespace(namespace=None, workers=1, apply=True)
            with patch.object(store, 'ROOT', root), self.assertRaisesRegex(ValueError, 'Conflicts held'):
                store.backup(Client(), {'folders':['Assets'], 'bucket':'test', 'default_namespace':'assets'}, args)

    def test_restore_detects_corrupt_object_bytes(self):
        row = {'path':'Assets/a.txt','key':'assets/Assets/a.txt','sha256':hashlib.sha256(b'good').hexdigest(),'bytes':4}
        manifest = {'schema':'aquila.asset-manifest/1','bucket':'test','files':[row]}
        class Client:
            def get_object(self, **kw):
                return {'Body':io.BytesIO(json.dumps(manifest).encode() if kw['Key'].startswith('_manifests/') else b'evil')}
        with tempfile.TemporaryDirectory() as raw:
            args = argparse.Namespace(manifest_key='_manifests/test.json', target=str(Path(raw)/'restore'), apply=True, workers=1)
            with self.assertRaisesRegex(ValueError, 'hash or size mismatch'):
                store.restore(Client(), {'bucket':'test'}, args)

    def test_parallel_restore_to_new_nested_directory(self):
        content = b'verified asset'
        rows = [{'path':f'AllAssets/group-{n % 8}/nested/art-{n}.txt',
                 'key':f'assets/AllAssets/group-{n % 8}/nested/art-{n}.txt',
                 'sha256':hashlib.sha256(content).hexdigest(), 'bytes':len(content)}
                for n in range(128)]
        manifest = {'schema':'aquila.asset-manifest/1','bucket':'test','files':rows}
        class Client:
            def get_object(self, **kw):
                return {'Body':io.BytesIO(json.dumps(manifest).encode() if kw['Key'].startswith('_manifests/') else content)}
        with tempfile.TemporaryDirectory() as raw:
            target = Path(raw)/'new'/'nested'/'restore'
            args = argparse.Namespace(manifest_key='_manifests/test.json', target=str(target), apply=True, workers=8)
            result = store.restore(Client(), {'bucket':'test'}, args)
            self.assertEqual(result['files'], len(rows))
            for row in rows:
                self.assertEqual((target/row['path']).read_bytes(), content)
            with self.assertRaisesRegex(ValueError, 'new or empty'):
                store.restore(Client(), {'bucket':'test'}, args)

    def test_guard_allows_authored_svg_but_holds_changed_legacy_media(self):
        self.assertIsNone(guard.rejected('src/button.svg', b'<svg><path d="M0 0"/></svg>', {}))
        raw = b'legacy icon'
        exceptions = {'public/favicon.ico':{'sha256':hashlib.sha256(raw).hexdigest()}}
        self.assertIsNone(guard.rejected('public/favicon.ico', raw, exceptions))
        self.assertIsNotNone(guard.rejected('public/favicon.ico', b'changed', exceptions))
        embedded = ('<svg href="data:'+'image/png;base64,'+'A'*200+'"/>').encode()
        self.assertIsNotNone(guard.rejected('src/new.svg', embedded, {}))
        self.assertIsNotNone(guard.rejected('Secrets/deploy.env', b'anything', {}))


if __name__ == '__main__':
    unittest.main()
