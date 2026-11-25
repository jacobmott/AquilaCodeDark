"""Blender background entry point for the two review scenes. No Unreal asset is read or written."""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import math
import os
from pathlib import Path
import sys

import bpy
import numpy as np
from mathutils import Euler, Quaternion, Vector

sys.path.insert(0, str(Path(__file__).resolve().parent))
from modeling import FINISHES, Materials, OUTPUT, REFERENCES, ROOT
import delta_tree
import glacier_tongue


DIRECTIONS = {"delta-tree": (.96, 1.0, .76), "glacier-tongue": (.94, 1.0, .205)}
ROLL = {"delta-tree": -1.0, "glacier-tongue": -5.0}
GALLERY = {"delta-tree": "58-delta-tree.png", "glacier-tongue": "41-glacier-tongue.png"}
MEDIA = ROOT / "reports/game-direction/media/hull-review-r174"


def sun(name, direction, power, softness, color):
    source=bpy.data.lights.new(name,"SUN")
    source.energy=power
    source.angle=math.radians(softness)
    source.color=color
    obj=bpy.data.objects.new(name,source)
    bpy.context.collection.objects.link(obj)
    obj.rotation_euler=(-Vector(direction)).to_track_quat("-Z","Y").to_euler()


def studio(scene):
    scene.render.engine = "CYCLES"
    prefs = bpy.context.preferences.addons["cycles"].preferences
    try:
        prefs.compute_device_type = "OPTIX"
        prefs.refresh_devices()
        gpu = any(device.type=="OPTIX" for device in prefs.devices)
        for device in prefs.devices:
            device.use = device.type=="OPTIX"
        scene.cycles.device = "GPU" if gpu else "CPU"
    except (RuntimeError, TypeError):
        scene.cycles.device = "CPU"
    scene.cycles.samples=32
    scene.cycles.use_denoising=True
    scene.cycles.max_bounces=5
    scene.render.film_transparent=False
    scene.render.image_settings.file_format="PNG"
    scene.render.resolution_percentage=100
    scene.view_settings.view_transform="AgX"
    scene.world=bpy.data.worlds.new("Studio · soft reflected sky")
    scene.world.use_nodes=True
    nodes=scene.world.node_tree.nodes
    link=scene.world.node_tree.links.new
    lighting=nodes.get("Background")
    lighting.inputs["Color"].default_value=(.50,.55,.66,1)
    lighting.inputs["Strength"].default_value=.60
    camera_bg=nodes.new("ShaderNodeBackground")
    camera_bg.inputs["Color"].default_value=(.011,.017,.025,1)
    camera_bg.inputs["Strength"].default_value=.70
    ray=nodes.new("ShaderNodeLightPath"); mix=nodes.new("ShaderNodeMixShader")
    link(ray.outputs["Is Camera Ray"],mix.inputs[0])
    link(lighting.outputs[0],mix.inputs[1]); link(camera_bg.outputs[0],mix.inputs[2])
    link(mix.outputs[0],nodes.get("World Output").inputs[0])
    sun("Broad upper key",(1,-.30,1.55),3.15,11,(1.0,.93,.84))
    sun("Silver rim",(-.65,-.9,.95),2.0,8,(.74,.84,1))
    sun("Port fill",(-.30,1,.32),.78,16,(.84,.90,1))
    camera=bpy.data.cameras.new("Gallery angle")
    cam=bpy.data.objects.new("Gallery angle",camera)
    bpy.context.collection.objects.link(cam)
    camera.type="ORTHO"; camera.clip_end=20000; camera.clip_start=1
    scene.camera=cam
    return cam


def point_camera(cam, coordinates, direction, aspect, margin, roll=0, lift=0):
    direction=Vector(direction).normalized()
    rot=(-direction).to_track_quat("-Z","Y") @ Quaternion((0,0,1),math.radians(roll))
    right,up=(rot@Vector(v) for v in ((1,0,0),(0,1,0)))
    axes=np.asarray([tuple(right),tuple(up),tuple(direction)])
    projected=coordinates@axes.T
    low,high=projected.min(axis=0),projected.max(axis=0)
    mid=Vector((.5*(high+low))@axes)
    cam.data.ortho_scale=max(high[0]-low[0],(high[1]-low[1])*aspect)/margin
    mid-=up*(cam.data.ortho_scale*lift)
    cam.location=mid+direction*2300
    cam.rotation_euler=rot.to_euler()
    return mid


def png(scene, path, width, height):
    scene.render.resolution_x=width; scene.render.resolution_y=height
    scene.render.filepath=str(path)
    bpy.ops.render.render(write_still=True)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def build(ship, preview):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene=bpy.context.scene
    scene.unit_settings.system="METRIC"
    target=OUTPUT/ship; target.mkdir(parents=True,exist_ok=True)
    MEDIA.mkdir(parents=True,exist_ok=True)
    parts=delta_tree.make() if ship=="delta-tree" else glacier_tongue.make()
    vertices=np.array([v for p in parts for v in p.vertices])
    low,high=vertices[:,0].min(),vertices[:,0].max()
    scale=1280/(high-low); origin=(low+high)/2
    mats=Materials(ship)
    objects=[p.bake(mats,origin,scale) for p in parts]
    coords=(vertices-[origin,0,0])*[-scale,-scale,scale]
    scene["review_scope"]="Exterior artwork review before gameplay integration"
    scene["shape_source"]="Selected gallery original and supporting exterior views. Geometry authored in three dimensions."
    scene["paint_source"]="Surface colors drawn from the overhead guide, side guide on Glacier, and Asset Pipeline tiles."
    cam=studio(scene)
    direction=DIRECTIONS[ship]
    mid=point_camera(cam,coords,direction,1,.94,ROLL[ship],.034 if ship=="delta-tree" else .045)
    if preview:
        scene.cycles.samples=12
        png(scene,target/"preview.png",736,736)
        return
    for screen in bpy.data.screens:
        for area in screen.areas:
            if area.type=="VIEW_3D":
                space=area.spaces.active; space.clip_end=12000; space.clip_start=.4
                space.shading.type="SOLID"; space.shading.color_type="TEXTURE"; space.overlay.show_overlays=False
                space.region_3d.view_rotation=cam.rotation_euler.to_quaternion()
                space.region_3d.view_distance=cam.data.ortho_scale*1.25
                space.region_3d.view_location=mid
    size=1100
    scene.render.resolution_x=size;scene.render.resolution_y=size
    bpy.context.preferences.filepaths.save_version=0
    bpy.ops.file.pack_all()
    native=target/(ship+".blend")
    bpy.ops.wm.save_as_mainfile(filepath=str(native))
    bpy.ops.object.select_all(action="DESELECT")
    for obj in objects:
        obj.select_set(True)
    glb=target/(ship+".glb")
    bpy.ops.export_scene.gltf(filepath=str(glb),export_format="GLB",use_selection=True,export_cameras=False,
                             export_lights=False,export_extras=False,export_yup=True)
    png(scene,MEDIA/(ship+"-gallery.png"),size,size)
    clay=bpy.data.materials.new("Neutral unpainted clay")
    clay.use_nodes=True
    shader=clay.node_tree.nodes.get("Principled BSDF")
    shader.inputs["Base Color"].default_value=(.44,.47,.50,1)
    shader.inputs["Roughness"].default_value=.74
    scene.view_layers[0].material_override=clay
    png(scene,MEDIA/(ship+"-clay.png"),size,size)
    scene.view_layers[0].material_override=None
    for name, direction in (("reverse",(-.78,-1,.62)),("side",(.035,1,.08)),("top",(0,.001,1))):
        if name=="top":
            # Aim aft to the right. Camera roll also matches the selected plan reference.
            rot=Euler((0,0,math.pi)).to_quaternion()
            bounds=coords.min(axis=0),coords.max(axis=0)
            mid=Vector((bounds[0]+bounds[1])/2)
            cam.rotation_euler=rot.to_euler();cam.location=mid+Vector((0,0,2300))
            cam.data.ortho_scale=max(bounds[1][0]-bounds[0][0],(bounds[1][1]-bounds[0][1])*1.6)/.93
        else:
            point_camera(cam,coords,direction,1.6,.92)
        png(scene,MEDIA/(ship+"-"+name+".png"),1152,720)
    stats=dict(ship=ship,created_at=datetime.now(timezone.utc).isoformat(),length_m=float(np.ptp(coords[:,0])),
               beam_m=float(np.ptp(coords[:,1])),height_m=float(np.ptp(coords[:,2])),vertices=len(coords),
               triangles=sum(sum(len(p.vertices)-2 for p in o.data.polygons) for o in objects),
               assemblies=sorted(obj.name for obj in objects),painted_materials=len(mats),camera=list(DIRECTIONS[ship]),roll_degrees=ROLL[ship],
               render_device=scene.cycles.device,
               files={p.name:digest(p) for p in (native,glb,*sorted(MEDIA.glob(ship+"-*.png")))})
    (target/"model-review.json").write_text(json.dumps(stats,indent=2)+"\n",encoding="utf-8")
    relative=lambda path: Path(os.path.relpath(path,target)).as_posix()
    inputs=[dict(path=relative(ROOT/"Assets/ShipConcepts/sunburst-r144"/GALLERY[ship]),role="Primary design reference")]
    for view in ["25-dorsal-recheck","01-port","04-underside","06-stern"]:
        inputs.append(dict(path=relative(REFERENCES/ship/(view+".png")),role="Supporting exterior reference"))
    if ship=="glacier-tongue":
        inputs.append(dict(path=relative(REFERENCES/ship/"30-starboard-vault.png"),role="Closed starboard side"))
    inputs.append(dict(path=(FINISHES/ship/(ship+"-textures.zip")).as_posix(),role="Existing paint, normal and material maps"))
    outputs=[relative(native),relative(glb)]+[relative(p) for p in sorted(MEDIA.glob(ship+"-*.png"))]
    record=dict(schema="asset-ledger.record/1",project="aquila-code-dark-game",jobs=[
        dict(id="aquila-code-dark-game-r174-"+ship+"-"+stats["files"][glb.name][:14],
             provider="blender",model=bpy.app.version_string+" / Cycles",prompt=None,inputs=inputs,outputs=outputs,
             settings=dict(local=True,length_m=1280,credits=0,views=["gallery","clay","reverse","side","top"]),
             status="succeeded",created_at=stats["created_at"],note="Authored exterior geometry for original-art review. No external generation.")])
    (target/"review.ledger-record.json").write_text(json.dumps(record,indent=2)+"\n",encoding="utf-8")
    print("Completed",ship,stats["triangles"],flush=True)


if __name__=="__main__":
    parser=argparse.ArgumentParser()
    parser.add_argument("ship",choices=["delta-tree","glacier-tongue"])
    parser.add_argument("--preview",action="store_true")
    args=parser.parse_args(sys.argv[sys.argv.index("--")+1:])
    build(args.ship,args.preview)
