"""Reopen each review artifact and probe the physical shapes independently of their paint."""
import json
import math
from pathlib import Path
import struct
import sys

import bpy
import numpy as np
from mathutils import Vector
from mathutils.bvhtree import BVHTree

sys.path.insert(0,str(Path(__file__).resolve().parent))
from modeling import OUTPUT
import delta_tree


def meshes():
    return [o for o in bpy.context.scene.objects if o.type=="MESH"]


def bounds(objects):
    corners=np.array([tuple(obj.matrix_world@Vector(p)) for obj in objects for p in obj.bound_box])
    return np.ptp(corners,axis=0).tolist()


def hit(group, design_point, direction):
    x,y,z=design_point
    origin=Vector(((50-x)*12.8,-y*12.8,z*12.8))
    ray=Vector((-direction[0],-direction[1],direction[2]))
    hits=[where for tree in group if (where:=tree.ray_cast(origin,ray,6000)[0]) is not None]
    if not hits:
        return None
    return min(hits,key=lambda h:(h-origin).length)


def inspect(ship):
    directory=OUTPUT/ship
    bpy.ops.wm.open_mainfile(filepath=str(directory/(ship+".blend")))
    objects=meshes()
    assert all(not obj.hide_render for obj in objects)
    native_bounds=bounds(objects)
    assert math.isclose(native_bounds[0],1280,abs_tol=.04)
    images=[i for i in bpy.data.images if i.source=="FILE"]
    assert images and all(i.packed_file for i in images)
    deps=bpy.context.evaluated_depsgraph_get()
    bvh={o.name:BVHTree.FromObject(o,deps) for o in objects}
    select=lambda text:[v for n,v in bvh.items() if text in n]
    measured={}
    if ship=="delta-tree":
        deck=select("Cut channel")+select("Inset courtyard")
        dips=[]
        for poly in delta_tree.COURTS:
            px,py=np.mean(poly,axis=0)
            x,y=delta_tree.point(px,py)
            sample=hit(deck,(x,y,30),(0,0,-1))
            assert sample is not None
            dips.append(round(delta_tree.top(px,py)*12.8-sample.z,2))
        assert len(dips)==11 and min(dips)>6
        measured["courtyard_depth_below_local_deck_m"]=dips
        cores=[]
        for n,z in ((5,-5),(4,.1),(3,5.1)):
            for j in range(n):
                y=(j-(n-1)/2)*6.65
                sample=hit(select("drive housings"),(105,y,z),(-1,0,0))
                assert sample is not None and sample.x < -510
                cores.append([round(v,1) for v in sample])
        measured["exhaust_cores_from_stern"]=cores
    else:
        blade=select("Outboard armored blade")
        ab=hit(blade,(73,-25.4,24),(0,0,-1))
        below=hit(blade,(73,-25.4,-22),(0,0,1))
        thickness=ab.z-below.z
        assert thickness>38
        measured["outboard_blade_thickness_at_sample_m"]=round(thickness,2)
        spans=[]
        for x in (54.9,65.65,81.8):
            up=hit(select("Three enclosed crossings"),(x,-14.2,24),(0,0,-1))
            dn=hit(select("Three enclosed crossings"),(x,-14.2,-18),(0,0,1))
            assert up is not None and dn is not None and up.z-dn.z>10
            spans.append(round(up.z-dn.z,2))
        measured["three_crossing_heights_at_midspan_m"]=spans
        ceiling=hit(select("Scalloped upper decks"),(61.2,-7.5,-1.45),(0,0,1))
        assert ceiling is not None and ceiling.z+1.45*12.8>65
        measured["clearance_to_overhang_near_dock_m"]=round(ceiling.z+1.45*12.8,2)
    blob=(directory/(ship+".glb")).read_bytes()
    assert blob[:4]==b"glTF" and struct.unpack_from("<I",blob,8)[0]==len(blob)
    size=struct.unpack_from("<I",blob,12)[0]
    gltf=json.loads(blob[20:20+size])
    assert all("uri" not in buf for buf in gltf["buffers"])
    assert all("bufferView" in img for img in gltf["images"])
    assert all("camera" not in n for n in gltf["nodes"])
    assert any("normalTexture" in m for m in gltf["materials"])
    assert any("metallicRoughnessTexture" in m.get("pbrMetallicRoughness",{}) for m in gltf["materials"])
    native_image_count=len(images)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=str(directory/(ship+".glb")))
    imported=meshes()
    imported_bounds=bounds(imported)
    assert len(imported)==len(objects) and np.allclose(native_bounds,imported_bounds,atol=.08)
    return dict(native_dimensions_m=native_bounds,interchange_dimensions_m=imported_bounds,
                reopened_assemblies=len(imported),packed_images_in_scene=native_image_count,
                embedded_images_in_glb=len(gltf["images"]),measurements=measured)


if __name__=="__main__":
    ship=sys.argv[sys.argv.index("--")+1]
    result=inspect(ship)
    (OUTPUT/ship/"geometry-check.json").write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(result),flush=True)
