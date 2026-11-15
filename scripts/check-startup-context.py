"""Check local startup routes, skill adapters, links and checkpoint budgets; no model calls."""
import argparse
import json
from pathlib import Path
import re
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--handoff')
    args = ap.parse_args()
    failures = []
    sizes = {}
    budgets = {'AGENTS.md': 10000, 'Docs/TaskRoutes.md': 6000, 'HANDOFF.md': 9000}
    if args.handoff:
        budgets[args.handoff] = 10000
    for name, cap in budgets.items():
        path = ROOT / name
        if not path.is_file():
            failures.append(f'Missing {name}')
            continue
        raw = path.read_bytes()
        sizes[name] = len(raw)
        if len(raw) > cap:
            failures.append(f'Over size budget: {name}')
        if name == args.handoff and len(raw.decode('utf-8-sig').split()) > 1200:
            failures.append('Checkpoint exceeds 1200 words')
    wrapper = ROOT / 'CLAUDE.md'
    if not wrapper.exists() or not wrapper.read_text(encoding='utf-8').startswith('@AGENTS.md\n'):
        failures.append('Missing native Claude import')
    for skill in ('newsession', 'session-wrap'):
        for parent in ('.agents', '.claude'):
            path = ROOT / parent / 'skills' / skill / 'SKILL.md'
            if not path.is_file() or not path.read_text(encoding='utf-8').startswith('---\nname: ' + skill + '\n'):
                failures.append(f'Invalid skill entry point: {parent}/{skill}')
    sources = [ROOT/'AGENTS.md', ROOT/'CLAUDE.md']
    for folder in ('Docs', 'knowledge', '.agents', '.claude'):
        sources.extend((ROOT/folder).rglob('*.md'))
    for path in sources:
        if 'History' in path.parts:
            continue
        for link in re.findall(r'\]\(([^)]+)\)', path.read_text(encoding='utf-8-sig')):
            link = unquote(link.strip('<>'))
            if link.startswith(('http:', 'https:', '#', 'mailto:')):
                continue
            link = link.split('#', 1)[0]
            if not (path.parent / link).exists():
                failures.append(f'Broken link in {path.relative_to(ROOT)}: {link}')
    print(json.dumps({'sizes': sizes, 'failures': failures, 'proves': 'local files and routes only'}, indent=2))
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
