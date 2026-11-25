"""Prepare either boarding exterior from its Blender study or the original loft recipe.

Run with Blender in background. The OBJ carries positions and material groups in metres.
Glacier uses 6.4 metres per study unit, with its port deck at study Z = 3.4.
Delta uses its approved 1,280-metre length and includes its outer docking island socket.
"""
import argparse
import sys
from pathlib import Path
from math import cos, sin, pi, atan2, degrees, hypot

import bmesh
import bpy
from mathutils import Vector, Matrix
from mathutils.bvhtree import BVHTree

def material(name, color):
    m = bpy.data.materials.new(name)
    m.diffuse_color = (*color, 1)
    return m


def finish(o, name, mat):
    o.name = name
    o.data.materials.append(mat)
    for face in o.data.polygons:
        face.use_smooth = True
    return o


def loft(name, rows, mat):
    mesh = bpy.data.meshes.new(name)
    verts, faces = [], []
    count = 24
    for u, center, port, starboard, bottom, top in rows:
        for i in range(count):
            a = 2 * pi * i / count
            c, s = cos(a), sin(a)
            y = center + (port if c >= 0 else starboard) * (1 if c >= 0 else -1) * abs(c)**0.70
            z = (bottom+top)/2 + (top-bottom)/2 * (1 if s >= 0 else -1) * abs(s)**0.70
            verts.append((50-100*u, y, z))
    for k in range(len(rows)-1):
        for i in range(count):
            a = k*count + i
            b = k*count + (i+1)%count
            faces.append((a,b,b+count,a+count))
    faces += [tuple(reversed(range(count))),tuple(range(len(verts)-count,len(verts)))]
    mesh.from_pydata(verts, [], faces)
    mesh.update()
    obj=bpy.data.objects.new(name,mesh)
    bpy.context.collection.objects.link(obj)
    return finish(obj,name,mat)


def oval(name, xyz, scale, mat):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=24,ring_count=12,location=xyz)
    obj=bpy.context.object
    obj.scale=scale
    return finish(obj,name,mat)


def tube(name, start, end, radius, mat, end_radius=None):
    a,b=Vector(start),Vector(end)
    axis=b-a
    bpy.ops.mesh.primitive_cone_add(vertices=32, radius1=radius, radius2=radius if end_radius is None else end_radius,
                                   depth=axis.length, location=(a+b)/2)
    obj=bpy.context.object
    obj.rotation_euler=axis.to_track_quat("Z","Y").to_euler()
    return finish(obj,name,mat)


def rail(name, points, width, mat):
    for i,(a,b) in enumerate(zip(points,points[1:])):
        tube(name+str(i),a,b,width,mat)


def glacier(m):
    U=[0,.045,.14,.27,.40,.57,.72,.86,.96,1]
    beam=[.35,4,9,14,16.5,17.6,17,14,8,.6]
    low=[4.5,5,5.5,8,11.5,13,13.7,12.2,12.3,13.5]
    high=[7,10.7,15.5,20,21.1,22.4,24,23,19,15.5]
    rows=[(u,-2.5,w*.90,w*.92,l,h) for u,w,l,h in zip(U,beam,low,high)]
    loft("Overhanging main vault",rows,m[0])
    for i in range(2):
        band=[(u,c+.1,p*(1+.025*i),s*(1+.025*i),l+(h-l)*(.22+.23*i),l+(h-l)*(.48+.23*i))
              for u,c,p,s,l,h in rows]
        loft("Overlapping armor band",band,m[1] if i==0 else m[0])
    loft("Lower forebody",[(.01,-2,.5,.6,4.3,7),(.12,-3,7,6,.5,10),(.25,-3,9.1,8,.4,12),
                           (.36,-4,7,8,1.1,13),(.42,-4,2,4,3.2,13)],m[2])
    loft("Set-back dock and aft spine",[(.43,-6,1,4,8,11),(.51,-6,5,6,5.2,15),(.68,-6,7,7,4.6,17),
                                     (.83,-6,5.5,6.8,7.3,16),(.96,-4,2,4,11,16)],m[2])
    loft("Calved outer blade",[(.33,12.7,.2,.2,.6,1.5),(.41,15.9,1.65,1.8,.05,2.4),(.58,19.0,2.2,2.8,.15,3.2),
                                 (.74,19.7,2.5,2.8,1.6,5.0),(.87,17.8,1.6,2,3.7,6.5),
                                 (.96,14.0,.25,.4,6.2,7.4)],m[0])
    for i,(u,y,z) in enumerate(((.39,15.6,2.6),(.61,19.4,3.3),(.79,19,5.2))):
        x=50-100*u
        rail("Enclosed crossing",[(x,1,7+i),(x-.7,y*.52,5.4+i*.3),(x-1.2,y,z)],.72,m[1])
    for x,y,z,s in [(-16,0,21.1,6.2),(-23,-1,22.8,6.6),(-30,-1,22.6,5.6)]:
        oval("Observation crown",(x,y,z),(s,4.8,2.2),m[1])
    bpy.ops.mesh.primitive_torus_add(major_radius=4.25,minor_radius=.45,major_segments=48,minor_segments=10,
                                    location=(-24,-.3,24.3), rotation=(pi/2,0,0))
    finish(bpy.context.object,"Survey arch",m[0])
    tube("Mast",(-30,-1,23),(-30,-1,32),.72,m[1],.08)


def delta(m):
    rows = [(0,0,.35,.35,5.9,7.5),(.055,0,5,4,3.3,9.5),(.15,-.3,10,7.3,1.6,9.7),
            (.28,-.4,13,10,.9,10),(.45,-.9,16,13,.4,11),(.57,-1,17.4,18,.6,13),
            (.68,-.5,17,16,0,15.6),(.77,0,15.5,15.5,.8,16.2),(.90,0,14,14,2.5,14),(.99,0,10,10,3.3,12)]
    loft("Inhabited branch deck", rows, m[0])
    for side in (1,-1):
        for i,u in enumerate((.16,.26,.36,.46,.56,.63)):
            start, end = 50-100*(u+.14), 50-100*u
            y, z = side*(5+1.9*i)*(1.19 if side<0 else .82), 10+.52*i
            rail("Raised branch", [(start,0,z),(start+5,y*.38,z-.1),(end,y,z-.5)], .72,m[1])
            tube("Round docking island", (end,y,z-1.2),(end,y,z+.05),1.55+.13*i,m[1])
    for row,z in enumerate((4.1,11,18.2)):
        for col,y in enumerate((-12.1,-5.9,.3,6.5,12.7)):
            x = -49 + (col%2)*.8
            tube("Drive room",(-24.5-(row%2)*2,y,z),(x,y,z),3,m[2],3.05)
            tube("Drive end",(x,y,z),(x-.6,y,z),2.55,m[1])
    oval("Observation deck",(-11,0,15.1),(8.8,6.2,2.8),m[0])
    tube("Dish support",(-11,0,16),(-11,0,22.6),1,m[2],.7)
    dish = oval("Dish",(-11,0,23.4),(3.7,2.65,.48),m[1])
    dish.rotation_euler[1] = -.42
    tube("Mast",(-18.5,-2,16),(-18.8,-2,29.2),.82,m[1],.07)


def delta_terminal(hull):
    """Register the playable terminal on the outermost island and open the connected pressure spaces."""
    points = [o.matrix_world @ v.co for o in hull for v in o.data.vertices]
    metres = 1280 / (max(p.x for p in points)-min(p.x for p in points))
    islands = [o for o in hull if o.name.startswith("Round docking island")]
    assert len(islands) == 12 and sum(o.name.startswith("Drive room") for o in hull) == 15
    island = min(islands,key=lambda o:o.matrix_world.translation.y)
    center = island.matrix_world.translation.copy()
    center.z = 12.0  # arrivals deck inside the 11.4 to 12.65 study-unit island
    # Follow the final arm of the sixth branch out to the dock.
    arm = bpy.data.objects["Raised branch1.011"]
    outward = center - arm.matrix_world.translation
    yaw = atan2(-outward.x,outward.y)
    dock_origin = center * metres
    bay_to_ship = Matrix.Translation(dock_origin) @ Matrix.Rotation(yaw,4,"Z") @ Matrix.Translation((0,17,0))
    hull.remove(island)  # Its footprint and foundation become a hollow, walkable terminal.
    bpy.data.objects.remove(island,do_unlink=True)
    for o in hull:
        o.data.transform(Matrix.Scale(metres,4) @ o.matrix_world)
        o.matrix_world = Matrix.Identity(4)
    bpy.context.view_layer.update()

    # Carve only through the crew route. Its deck, curved walls and roofs are furnished in Unreal.
    for shape, pos, size in [
        ("round", (0,-17,4.5), (26.3,26.3,11.4)),
        ("box", (0,-51,3.1), (8.1,33,8.0)),
        ("box", (0,-72,3.2), (41.8,14.8,8.3)),
        ("box", (0,-64,3.1), (11.5,8,8.0)),
        ("box", (24.8,-72.6,2.4), (12.4,7.2,6.8)),
        ("box", (27.5,-72.6,9.0), (8.8,8,22)),
        ("incline", (-62.25,-72.6,32.4), (hypot(179.5,36)+9,8,9.5)),
        ("box", (-161,-72.6,50.4), (24,7.8,7.8)),
        ("round", (-186,-72.6,51.2), (17.8,17.8,8.5)),
    ]:
        if shape == "round":
            bpy.ops.mesh.primitive_cylinder_add(vertices=64,radius=1,depth=1)
            scale = size
        else:
            bpy.ops.mesh.primitive_cube_add(size=1)
            scale = size
        cutter = bpy.context.object
        tilt = Matrix.Rotation(atan2(36,179.5),4,"Y") if shape == "incline" else Matrix.Identity(4)
        cutter.matrix_world = bay_to_ship @ Matrix.Translation(pos) @ tilt @ Matrix.Diagonal((*scale,1))
        bpy.context.view_layer.update()
        bounds = [cutter.matrix_world @ Vector(p) for p in cutter.bound_box]
        for o in hull:
            box = [Vector(p) for p in o.bound_box]
            if any(max(p[i] for p in box)<min(p[i] for p in bounds) or
                   min(p[i] for p in box)>max(p[i] for p in bounds) for i in range(3)):
                continue
            solid = bmesh.new()
            solid.from_mesh(o.data)
            bmesh.ops.recalc_face_normals(solid,faces=list(solid.faces))
            solid.to_mesh(o.data)
            solid.free()
            mod = o.modifiers.new("Boarding clearance","BOOLEAN")
            mod.operation, mod.solver, mod.object = "DIFFERENCE","EXACT",cutter
            bpy.context.view_layer.objects.active = o
            bpy.ops.object.modifier_apply(modifier=mod.name)
        bpy.data.objects.remove(cutter,do_unlink=True)
    dock = "#socket BoardingDock %.6f %.6f %.6f %.8f" % (*dock_origin,degrees(yaw))
    return Matrix.Identity(4), ["# Delta Tree, length 1280 metres, bow +X", dock]


def fitting_sockets(lines, ship):
    """Measure each gun's whole mounting ring against the exported hull triangles."""
    verts = [Vector(tuple(map(float, line.split()[1:]))) for line in lines if line.startswith("v ")]
    faces = [tuple(int(i)-1 for i in line.split()[1:]) for line in lines if line.startswith("f ")]
    low = Vector(tuple(min(v[i] for v in verts) for i in range(3)))
    high = Vector(tuple(max(v[i] for v in verts) for i in range(3)))
    center, scale = (low+high)*.5, 1360/(high.x-low.x)
    tree = BVHTree.FromPolygons([(v-center)*scale for v in verts], faces, all_triangles=True)
    # Keel coordinates, stern to bow. Each bank follows the shape of its own ship.
    xs = (-465,-367,-269,-171,-73,25,123,221,319,417)
    if ship == "delta":
        banks = ((-149,-128,-150,-148,-160,-137,-121,-102,-86,-65),
                 (0,5,74,74,55,34,22,15,10,4), (114,135,160,175,164,151,131,112,98,82))
        utility = ((-565,-53),(530,8),(445,-25))
    else:
        banks = ((-170,-188,-192,-196,-190,-181,-162,-141,-116,-94),
                 (-61,-82,-67,-61,-60,-51,-44,-38,-35,-33), (54,68,71,68,63,53,41,29,19,10))
        utility = ((-564,-105),(525,-38),(455,-60))
    positions = [(f"Gun{row*10+col+1:02}", x,y,13.5)
                 for row,bank in enumerate(banks) for col,(x,y) in enumerate(zip(xs,bank))]
    positions.extend((name,x,y,8.) for name,(x,y) in zip(("Armor","Shield","Reserve"),utility))
    for name,x,y,radius in positions:
        best = None
        for dx in (0,-10,10,-20,20):
            for dy in (0,-10,10,-20,20):
                p,n,_,_ = tree.ray_cast((x+dx,y+dy,400),(0,0,-1))
                if p is None or n.z < .78:
                    continue
                ring = [tree.ray_cast((p.x+radius*cos(k*pi/4),p.y+radius*sin(k*pi/4),400),(0,0,-1))[0] for k in range(8)]
                if any(v is None for v in ring):
                    continue
                # Prefer a nearby broad face. Prevent a barrel base hanging off a ridge or bridging an empty gap.
                error = max(abs((v-p).dot(n)) for v in ring)
                score = error + .045*hypot(dx,dy)
                if best is None or score < best[0]:
                    best = score,p,n,error
        if best is None or best[3] > 3.5:
            raise ValueError(f"{ship} {name} has no supported seat near {(x,y)}: {best}")
        _,p,n,_ = best
        lines.append("#surface %s %.6f %.6f %.6f %.8f %.8f %.8f" % (name,*(p/scale+center),*n))


def export_boarding_hull():
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--study", type=Path)
    parser.add_argument("--ship", choices=("glacier","delta"), default="glacier")
    args = parser.parse_args(sys.argv[sys.argv.index("--") + 1:])
    if args.source.is_file():
        bpy.ops.wm.open_mainfile(filepath=str(args.source.resolve()))
    else:
        bpy.ops.wm.read_factory_settings(use_empty=True)
        build = delta if args.ship == "delta" else glacier
        build([material("Armor", (.52, .54, .58)), material("Raised fittings", (.31, .34, .38)),
               material("Recesses", (.19, .21, .24))])

    bpy.context.view_layer.update()
    hull = sorted((o for o in bpy.context.scene.objects if o.type == "MESH"), key=lambda o: o.name)
    if args.ship == "delta":
        scale, lines = delta_terminal(hull)
    else:
        assert len(hull) == 17, "Unexpected Glacier Tongue study"
        scale = Matrix.Translation((0, 0, -21.76)) @ Matrix.Scale(6.4, 4)
        lines = ["# Glacier Tongue, metres, port +Y, landing deck Z=0"]
    slots = {"Armor": "Armor", "Raised fittings": "Trim", "Recesses": "Spine"}
    offset = 1
    for part in hull:
        part.matrix_world = scale @ part.matrix_world
        mesh = bmesh.new()
        mesh.from_mesh(part.data)
        bmesh.ops.transform(mesh, verts=mesh.verts, matrix=part.matrix_world)
        bmesh.ops.recalc_face_normals(mesh, faces=list(mesh.faces))
        bmesh.ops.triangulate(mesh, faces=list(mesh.faces))
        mesh.verts.index_update()
        if args.ship == "glacier" and part.name == "Observation crown":
            # Forward of the three crowns. The horizontal command deck rests on this vertex.
            crown = max(mesh.verts, key=lambda vert: vert.co.z).co
            lines.append("#socket CommandBridge %.6f %.6f %.6f 0" % tuple(crown))
        lines.append("o " + part.name)
        lines.extend("v %.6f %.6f %.6f" % tuple(v.co) for v in mesh.verts)
        finish_name = "Exhaust" if part.name.startswith("Drive end") else slots[part.data.materials[0].name]
        lines.append("usemtl " + finish_name)
        lines.extend("f " + " ".join(str(offset + v.index) for v in f.verts) for f in mesh.faces)
        offset += len(mesh.verts)
        mesh.free()
    fitting_sockets(lines, args.ship)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="ascii")
    if args.study and args.ship == "glacier":
        for name, center, size in [("Peregrine clearance envelope", (0, 61, 8), (64, 68, 16)),
                                   ("Standing crew", (0, 26, 2.9), (.68, .68, 1.8)),
                                   ("Boarding deck datum", (0, 0, -.15), (12, 52, .3))]:
            bpy.ops.mesh.primitive_cube_add(size=1, location=center)
            guide = bpy.context.object
            guide.name, guide.dimensions, guide.display_type = name, size, "WIRE"
            guide.hide_render = True
        scene = bpy.context.scene
        scene.unit_settings.system = "METRIC"
        scene["scale_note"] = "Trial exterior: 640 m. Shuttle 68 x 64 x 16 m. Player 1.80 m. Port +Y."
        args.study.parent.mkdir(parents=True, exist_ok=True)
        bpy.context.preferences.filepaths.save_version = 0
        bpy.ops.wm.save_as_mainfile(filepath=str(args.study.resolve()))


export_boarding_hull()
