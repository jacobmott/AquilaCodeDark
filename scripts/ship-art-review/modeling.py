"""Mesh and material tools for the two exterior reviews. Design coordinates run bow to stern."""
import math
from pathlib import Path

import bpy
import numpy as np
from mathutils import Vector


ROOT = Path(__file__).resolve().parents[2]
REFERENCES = ROOT / "Assets/ShipConcepts/modeling-references-r150"
FINISHES = ROOT.parent / "asset-pipeline/Assets/AssetPipelineLab/Aquila_Research_Battleships/R003"
OUTPUT = ROOT / "Assets/ShipConcepts/hull-review-r174"
TAU = math.tau
UP = (0, 0, 1)
AFT = (1, 0, 0)


def profile(keys, x):
    """Cubic Hermite interpolation with level tangents at local extrema."""
    if x <= keys[0][0]:
        return keys[0][1]
    if x >= keys[-1][0]:
        return keys[-1][1]
    def tangent(i):
        if i == 0 or i == len(keys) - 1:
            a, b = keys[:2] if i == 0 else keys[-2:]
            return (b[1] - a[1]) / (b[0] - a[0])
        a, b, c = keys[i-1:i+2]
        left, right = (b[1]-a[1])/(b[0]-a[0]), (c[1]-b[1])/(c[0]-b[0])
        return 2*left*right/(left+right) if left*right > 0 else 0
    i = next(i for i in range(len(keys)-1) if keys[i+1][0] >= x)
    (a, y), (b, z) = keys[i:i+2]
    t = (x-a)/(b-a)
    return (1+2*t)*(1-t)**2*y + t*(1-t)**2*(b-a)*tangent(i) + t*t*(3-2*t)*z + t*t*(t-1)*(b-a)*tangent(i+1)


def interpolate(values, stations, x):
    return profile(list(zip(stations, values)), x)


def lin(a, b, t):
    return a + (b-a)*t


class Part:
    """Editable themed assembly with per-face material and an optional artwork UV for each corner."""
    def __init__(self, name):
        self.name = name
        self.vertices = []
        self.faces = []
        self.materials = []
        self.uvs = []

    def points(self, coords):
        start = len(self.vertices)
        self.vertices.extend(tuple(v) for v in coords)
        return list(range(start, len(self.vertices)))

    def face(self, face, mat, uvs=None):
        self.faces.append(tuple(face))
        self.materials.append(mat)
        self.uvs.append(uvs)

    def quad(self, coords, mat):
        self.face(self.points(coords), mat)

    def bake(self, materials, origin, scale):
        xyz = np.asarray(self.vertices)
        coords = (xyz - [origin, 0, 0]) * [-scale, -scale, scale]
        mesh = bpy.data.meshes.new(self.name)
        mesh.from_pydata(coords.tolist(), [], self.faces)
        slots = list(dict.fromkeys(self.materials))
        indices = {name: i for i, name in enumerate(slots)}
        for name in slots:
            mesh.materials.append(materials[name])
        mesh.polygons.foreach_set("material_index", [indices[m] for m in self.materials])
        mesh.polygons.foreach_set("use_smooth", [True]*len(self.faces))
        mesh.update()
        mesh.set_sharp_from_angle(angle=math.radians(32))
        layer = mesh.uv_layers.new(name="Surface")
        packed = []
        for face, supplied in zip(self.faces, self.uvs):
            if supplied is not None:
                packed.extend(supplied)
            else:
                verts = xyz[list(face)]
                active = np.abs(np.cross(verts[1]-verts[0], verts[2]-verts[0])).argmax()
                a, b = ((1, 2), (0, 2), (0, 1))[active]
                packed.extend(zip(verts[:,a]/9, verts[:,b]/9))
        layer.data.foreach_set("uv", np.asarray(packed, dtype=np.float32).ravel())
        obj = bpy.data.objects.new(self.name, mesh)
        bpy.context.collection.objects.link(obj)
        return obj


def strip(part, sections, material, caps=None, loop=True, uv=None):
    """Connect equal-sized rings or open rows in the supplied winding order."""
    rings = [part.points(row) for row in sections]
    width = len(rings[0])
    for i, (a, b) in enumerate(zip(rings, rings[1:])):
        for j in range(width if loop else width-1):
            k = (j+1) % width
            face = (a[j], a[k], b[k], b[j])
            coords = [uv(i,j), uv(i,k), uv(i+1,k), uv(i+1,j)] if uv else None
            part.face(face, material(i,j) if callable(material) else material, coords)
    if caps:
        part.face(rings[0][::-1], caps)
        part.face(rings[-1], caps)
    return rings


def basis(axis):
    axis = Vector(axis).normalized()
    cross = axis.cross(Vector(UP if abs(axis.z)<0.88 else AFT)).normalized()
    return axis, cross, axis.cross(cross)


def revolved(part, center, axis, levels, cap, segments=48, flatten=(1, 1)):
    """Ordered axial stations carry radius and the material on the following ring."""
    axis, u, v = basis(axis)
    center = Vector(center)
    ring = [u * (flatten[0]*math.cos(i*TAU/segments)) + v * (flatten[1]*math.sin(i*TAU/segments)) for i in range(segments)]
    sections = [[center + axis*h + d*r for d in ring] for h,r,_ in levels]
    strip(part, sections, lambda i,j: levels[i][2], cap)


def rod(part, a, b, radius, mat, sides=8, end_radius=None):
    a, b = Vector(a), Vector(b)
    if (b-a).length < 1e-5:
        return
    r = radius if end_radius is None else end_radius
    revolved(part, a, b-a, [(0,radius,mat), ((b-a).length,r,mat)], mat, sides)


def box(part, center, size, mat, rotation=0):
    x,y,z = center
    dx,dy,dz = [k/2 for k in size]
    cs,sn = math.cos(rotation),math.sin(rotation)
    footprint = [(x+a*cs-b*sn,y+a*sn+b*cs) for a,b in ((-dx,-dy),(dx,-dy),(dx,dy),(-dx,dy))]
    strip(part, [[(a,b,z+t) for a,b in footprint] for t in (-dz,dz)], mat, mat)


def drum(part, center, rx, ry, levels, segments=60):
    x,y,z = center
    sections = [[(x+rx*r*math.cos(t*TAU/segments), y+ry*r*math.sin(t*TAU/segments), z+h)
                 for t in range(segments)] for h,r,_ in levels]
    strip(part, sections, lambda i,j: levels[i][2], levels[-1][2])


def windows(part, x, y, z, rx, ry, count, height, start=0, end=TAU):
    for i in range(count):
        if i % 11 == 7:
            continue
        a = lin(start,end,(i+.16)/count)
        b = lin(start,end,(i+.69)/count)
        pts = [(x+rx*math.cos(t),y+ry*math.sin(t),h) for t,h in ((a,z),(b,z),(b,z+height),(a,z+height))]
        part.quad(pts, "lamp" if i % 4 else "dim")


def dish(part, x, y, z, radius, direction=(-.55, -.22, .8)):
    axis,u,v = basis(direction)
    base = Vector((x,y,z))
    levels = [(-.17*radius,.09*radius,"black"), (-.10*radius,.45*radius,"metal"),
              (.24*radius,radius,"trim"), (.28*radius,radius,"silver"), (.14*radius,.78*radius,"silver"),
              (-.014*radius,.46*radius,"silver"), (-.045*radius,.13*radius,"black")]
    revolved(part, base, axis, levels, "metal", 56)
    for i in range(12):
        out = u*math.cos(i*TAU/12)+v*math.sin(i*TAU/12)
        rod(part, base + out*radius*.14, base+out*radius*.98+axis*(.268*radius), .014*radius, "trim", 5)
    tip = base+axis*.58*radius
    for i in range(3):
        out = u*math.cos(i*TAU/3)+v*math.sin(i*TAU/3)
        rod(part, base + out*radius*.72+axis*.12*radius, tip, .018*radius, "metal", 6)
    rod(part, tip, tip+axis*.11*radius, .04*radius, "trim")


def antenna(part, x, y, z, h, r):
    for level in (0, .3, .58):
        lower, upper = r*(1-level), r*(1-level-.30)
        for i in range(4):
            a,b = i*TAU/4, (i+1)*TAU/4
            start = (x+lower*math.cos(a), y+lower*math.sin(a), z+level*h)
            end = (x+upper*math.cos(a), y+upper*math.sin(a), z+(level+.3)*h)
            rod(part, start, end, r*.10, "metal", 6)
            rod(part, start, (x+upper*math.cos(b),y+upper*math.sin(b),end[2]), r*.042, "silver", 5)
            rod(part, start, (x+lower*math.cos(b),y+lower*math.sin(b),start[2]), r*.075, "metal", 5)
    rod(part, (x,y,z+.6*h),(x,y,z+h), r*.13, "silver", 10, r*.026)
    rod(part, (x,y,z+h),(x,y,z+h+.10), r*.08, "lamp")


class Materials(dict):
    def __init__(self, ship):
        super().__init__()
        self.folder = FINISHES / ship / "textures"
        self.ship = ship

    def __missing__(self, name):
        glacier = self.ship == "glacier-tongue"
        palette = {
            "ivory": ("silver-strake" if glacier else "ivory", (.60,.58,.53), .44, .48),
            "blue": ("blue-strake" if glacier else None, (.12,.22,.32), .38, .53),
            "silver": ("pewter" if glacier else "silver", (.52,.55,.57), .55,.40),
            "metal": ("graphite", (.16,.18,.20), .55,.46),
            "black": ("shadow" if glacier else "joint", (.035,.05,.07), .34,.54),
            "trim": ("pewter" if glacier else "bronze", (.36,.29,.20), .58,.45),
            "floor": ("pewter" if glacier else "limestone", (.28,.28,.27), .34,.59),
            "glass": (None,(.066,.108,.14),.64,.21), "lamp": (None,(1,.58,.23),.12,.34),
            "dim": (None,(.46,.27,.13),.15,.46), "optic": (None,(.35,.55,.66),.3,.27),
            "dorsal": (None,(.54,.51,.46),.28,.53), "port": (None,(.45,.46,.49),.3,.53),
        }
        tile, color, metal, rough = palette[name]
        mat = bpy.data.materials.new(self.ship + " · " + name)
        mat.diffuse_color = (*color, 1)
        mat.use_nodes = True
        nodes = mat.node_tree.nodes
        bsdf = nodes.get("Principled BSDF")
        bsdf.inputs["Base Color"].default_value = (*color, 1)
        bsdf.inputs["Metallic"].default_value = metal
        bsdf.inputs["Roughness"].default_value = rough
        if name in {"lamp","dim","optic"}:
            bsdf.inputs["Emission Color"].default_value = (*color,1)
            bsdf.inputs["Emission Strength"].default_value = {"lamp":1.15,"dim":.38,"optic":.6}[name]
        path = self.folder / (tile + "-color.png") if tile else None
        if name in ("dorsal", "port"):
            ref = "25-dorsal-recheck.png" if name == "dorsal" else "01-port.png"
            path = REFERENCES / self.ship / ref
        if path:
            tex = nodes.new("ShaderNodeTexImage")
            tex.image = bpy.data.images.load(str(path), check_existing=True)
            tex.extension = "EXTEND" if name in ("port","dorsal") else "REPEAT"
            tex.location = (-360, 280)
            mat.node_tree.links.new(tex.outputs["Color"], bsdf.inputs["Base Color"])
        if tile:
            def data_map(filename, location):
                sampler=nodes.new("ShaderNodeTexImage")
                sampler.image=bpy.data.images.load(str(self.folder/filename),check_existing=True)
                sampler.image.colorspace_settings.name="Non-Color"
                sampler.location=location
                return sampler
            orm=data_map(tile+"-orm.png",(-600,-40))
            channels=nodes.new("ShaderNodeSeparateColor")
            channels.location=(-290,-100)
            mat.node_tree.links.new(orm.outputs["Color"],channels.inputs["Color"])
            mat.node_tree.links.new(channels.outputs["Green"],bsdf.inputs["Roughness"])
            mat.node_tree.links.new(channels.outputs["Blue"],bsdf.inputs["Metallic"])
            if name in ("metal","black"):
                normal_map="service-normal.png"
            elif name in ("trim","silver"):
                normal_map="trim-normal.png"
            else:
                normal_map="strake-normal.png" if glacier else "plate-normal.png"
            normal=data_map(normal_map,(-620,-430))
            bump=nodes.new("ShaderNodeNormalMap")
            bump.location=(-280,-365)
            bump.inputs["Strength"].default_value=.38
            mat.node_tree.links.new(normal.outputs["Color"],bump.inputs["Color"])
            mat.node_tree.links.new(bump.outputs["Normal"],bsdf.inputs["Normal"])
        self[name] = mat
        return mat
