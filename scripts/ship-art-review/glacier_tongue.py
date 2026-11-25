"""Glacier Tongue's separate armor shelves, asymmetric dock, crossing galleries and outboard blade."""
import math
import bpy
import numpy as np
from mathutils import Vector

from modeling import AFT, TAU, UP, REFERENCES, Part, antenna, box, dish, drum, interpolate, lin, profile, revolved, rod, strip, windows


SX = [0,2.5,7,12,17,22,28,34,40,45.5,51,57,62,67,71,75,80,85,90,95,100]
# Pixels along each independently traced ledge in the port elevation.
UPPER = [395,353,320,301,296,293,283,282,276,266,265,266,268,267,259,256,263,270,277,293,308]
MID = [399,359,350,344,343,326,309,307,293,289,293,287,288,291,282,282,290,296,300,309,311]
LOW = [402,393,387,396,393,378,339,335,325,312,314,302,304,299,291,293,302,307,310,315,313]
BAY = [422,425,423,420,421,416,372,356,353,343,337,335,338,345,354,346,325,322,326,318,316]
PLAN_X = [0,2,6,11,18,26,34,43,52,61,69,77,85,91,95,98,100]
PLAN_FAR = [313,285,253,225,204,187,167,159,147,140,141,150,165,180,204,255,290]
PLAN_NEAR = [315,337,362,383,407,422,429,434,436,447,445,441,423,402,379,334,291]


def cross(x):
    f=interpolate(PLAN_FAR,PLAN_X,x); n=interpolate(PLAN_NEAR,PLAN_X,x)
    return (300-f)/9.9,(300-n)/9.9


def side_lines(x):
    levels=[interpolate(v,SX,x) for v in (UPPER,MID,LOW,BAY)]
    for i in range(1,4):
        levels[i]=max(levels[i],levels[i-1]+1.3)
    return levels


def height(pixel):
    return (412-pixel)/9.64


def plumage():
    ref = bpy.data.images.load(str(REFERENCES/"glacier-tongue/25-dorsal-recheck.png"),check_existing=True)
    w,h=ref.size
    rgb=np.empty(w*h*4,np.float32); ref.pixels.foreach_get(rgb)
    rgb=rgb.reshape(h,w,4)[::-1]
    weight=np.clip((rgb[:,:,0]/np.maximum(rgb[:,:,2],.02)-.42)/.47,0,1)
    def sample(u,v):
        return float(weight[int(np.clip(v,0,h-1)),int(np.clip(u,0,w-1))])
    return sample


def top_crown(x, t):
    rise=profile([(0,.02),(4,1.2),(17,2.9),(38,3.3),(58,3.8),(74,3.7),(88,2.7),(97,1.0),(100,.03)],x)
    return height(side_lines(x)[0])+rise*(1-t*t)


def plate_shell(roofs, plates):
    xs = np.linspace(0,100,260)
    sample=plumage()
    uvrows, roofrows = [], []
    for x in xs:
        far,near=cross(x); mid=(far+near)/2
        roof=[]; uvrow=[]
        top_height=height(side_lines(x)[0])
        lift=top_crown(x,0)-top_height
        for t in np.linspace(-1,1,81):
            side=near if t<0 else far
            y=lin(mid,side,abs(t)*.84)
            u,v=16+x*9.90,300-y*9.90
            weight=sample(u,v)
            z=top_height+lift*(1-t*t)+.57*weight*(1-t*t)*min(1,x/3,(100-x)/3)
            roof.append((x,y,z)); uvrow.append((u/1024,1-v/688))
        roofrows.append(roof[::-1]); uvrows.append(uvrow[::-1])
    strip(roofs,roofrows,"dorsal",loop=False,uv=lambda i,j: uvrows[i][j])
    for sign in (-1,1):
        all_rows=[]; mapped=[]
        for x in xs:
            far,near=cross(x); center=(far+near)/2; outside=near if sign<0 else far
            a,b,c,d=side_lines(x)
            end_scale=max(.015,min(1,x/3,(100-x)/3))
            # Independent wave heights and real returns beneath each lip.
            stations=[(.84,a),(.89,a+.6),(.91,a+1.8),(.865,a+3.5),(.85,a+5.1),
                      (.94,b-2),(.975,b),(.97,b+1.6),(.907,b+3.2),(.9,b+4.6),
                      (.978,c-2.8),(1.018,c),(1.016,c+1.75),(.953,c+3.5),(.938,c+5),
                      (.971,d-3),(.975,d),(.925,d+1.55),(.855,d+2.1)]
            # Keep knife edges ordered at the noses where the painted ledges converge.
            high=height(a)
            row=[]; uvrow=[]; previous=high+.04
            for j,(frac,pixel) in enumerate(stations):
                z=min(previous-.015*end_scale,height(pixel))
                row.append((x,lin(center,outside,frac),z))
                uvrow.append(((32+9.64*x)/1024,1-(412-9.64*z)/688))
                previous=z
            all_rows.append(row if sign>0 else row[::-1])
            mapped.append(uvrow if sign>0 else uvrow[::-1])
        mats=["port","ivory","metal","black","port","port","ivory","metal","black","port",
              "port","ivory","metal","black","port","port","blue","black"]
        if sign<0:
            mats.reverse()
        strip(plates,all_rows,lambda i,j:mats[j],loop=False,uv=lambda i,j:mapped[i][j])
    # The belly closes the side sheets but leaves clear space beneath the port overhang.
    rows=[]
    for x in xs:
        far,near=cross(x); cy=(far+near)/2
        z=height(side_lines(x)[3]+2.15)
        rows.append([(x,lin(cy,near,.855),z),(x,lin(cy,near,.43),z-.63),(x,cy,z-.88),
                     (x,lin(cy,far,.48),z-.63),(x,lin(cy,far,.855),z)])
    strip(roofs,rows,"black",loop=False)


def lower_body(hull, works, lights):
    xs=np.linspace(.7,99.5,155)
    rows=[]
    for x in xs:
        far,near=cross(x); center=(far+near)/2; e=height(side_lines(x)[3]+1.8)
        inset=profile([(0,.79),(9,.93),(21,.88),(32,.56),(43,.31),(55,.14),(70,.22),(84,.48),(95,.72),(100,.4)],x)
        port=lin(center,near,inset); star=lin(center,far,.91)
        bottom=profile([(0,-1.0),(13,-4.1),(32,-4.8),(49,-3.8),(63,-2.6),(77,-1.90),(90,1.5),(100,6.8)],x)
        top=max(e+.40,bottom+.60)
        rows.append([(x,port,top),(x,lin(port,center,.03),lin(top,bottom,.18)),(x,lin(port,center,.045),lin(top,bottom,.68)),
                     (x,lin(port,center,.14),lin(top,bottom,.84)),(x,lin(port,center,.36),bottom),
                     (x,center,bottom-.45),(x,lin(star,center,.31),bottom+.05),(x,star,lin(top,bottom,.73)),
                     (x,star+.26,lin(top,bottom,.54)+.32*math.sin(x*.38)),(x,star-.31,lin(top,bottom,.47)),
                     (x,star-.35,lin(top,bottom,.33)),(x,star+.13,lin(top,bottom,.23)),
                     (x,star,lin(top,bottom,.17)),(x,lin(center,far,.87),top)])
    strip(hull,rows,lambda i,j:("blue","metal","black","metal","black","metal","blue","blue","ivory","black","blue","silver","ivory","black")[j],"metal")
    # Recessed pressure walls and two real deck edges close the forward support beneath the overhang.
    for xmin,xmax,inset in ((3.0,45.2,.66),(70.4,95.6,.57)):
        rows=[]
        for x in np.linspace(xmin,xmax,76):
            far,near=cross(x); cy=(far+near)/2
            roof=height(side_lines(x)[3])+1
            foot=profile([(0,-1.3),(14,-3.78),(31,-4.18),(45,-3.22),(72,-1.55),(87,.38),(98,6.85)],x)
            w=math.sin(math.pi*(x-xmin)/(xmax-xmin))**.35
            y=lin(cy,near,inset+.11*w)
            rows.append([(x,y+1.3,roof),(x,y,roof-.27),(x,y,lin(roof,foot,.27)),
                         (x,y-.36*w,lin(roof,foot,.35)),(x,y-.25*w,lin(roof,foot,.42)),
                         (x,y+.5,lin(roof,foot,.48)),(x,y+.64,lin(roof,foot,.73)),
                         (x,y-.19*w,lin(roof,foot,.78)),(x,y-.12*w,lin(roof,foot,.84)),
                         (x,y+.45,foot),(x,y+2.5,foot-.2)])
        strip(hull,rows,lambda i,j:("black","metal","metal","silver","black","black","metal","metal","black","metal","black")[j],"metal")
    for x in np.arange(11,90,2.6):
        far,near=cross(x); cy=(far+near)/2
        t=profile([(10,.85),(24,.83),(37,.49),(48,.30),(70,.20),(84,.49),(92,.68)],x)
        y=lin(cy,near,t)
        floor=profile([(0,-2.5),(27,-4.5),(49,-3.8),(70,-2.4),(90,1.6)],x)
        roof=height(side_lines(x)[3])-.35
        if 54<x<68:
            continue
        span=max(.7,roof-floor)
        box(works,(x,y-.16,floor+span*.53),(.48,1.1,span*.97),"metal")
        for k in (.25,.57,.82):
            box(works,(x,y-.60,floor+span*k),(2.25,1.12,.25),"metal")
            box(lights,(x+.18,y-1.19,floor+span*k+.12),(.50,.1,.10),"dim")
        if int(x)%3==0:
            box(works,(x,y+.3,floor-.4),(1.35,2.3,1.2),"black")


def dock_frame(works,lights):
    x,y,z=61.2,-2.7,-1.6
    # A continuous octagonal doorway with a genuinely inset back and side walls.
    outline=[(-4.35,0),(-4.35,2.42),(-3.51,3.65),(-2.77,4.02),(2.77,4.02),(3.51,3.65),(4.35,2.42),(4.35,0)]
    rows=[]
    for off, sx, sz in ((-2.8,1.18,1.16),(-3.08,1.105,1.11),(-3.15,1,1),(-2.74,.94,.97),(-.15,.90,.92)):
        rows.append([(x+a*sx,y+off,z+b*sz) for a,b in outline])
    strip(works,rows,lambda i,j:["metal","trim","silver","metal"][i],loop=False)
    box(works,(x,y-1.65,z-.10),(10,3.7,.35),"metal")
    box(works,(x,y-.04,z+1.56),(7.65,.32,3.22),"dim")
    box(works,(x-.20,y-.24,z+1.63),(6.76,.15,2.82),"dim")
    for i in range(17):
        xx=x-3.49+i*.43
        box(works,(xx,y-.36,z+1.56),(.068,.14,2.93),"trim" if i%6 else "metal")
    for k in (.30,1.70,2.91):
        box(lights,(x,y-.38,z+k),(6.83,.10,.095),"lamp")
    box(lights,(x,y-2.32,z+3.38),(5.58,.56,.12),"lamp")
    for k in range(6):
        box(works,(x-2.75+k*1.10,y-4.39,z-.01),(.52,3.1,.09),"floor")
    for side in (-1,1):
        for i in range(5):
            box(lights,(x+side*(3.68+i*.034),y-.30-.49*i,z+2.48),(.20,.30,.20),"lamp")
    for xx in (50.7,53.4,68.5,71.1):
        box(works,(xx,-2.85,2.05),(.78,3.1,7.4),"metal")
        for z0 in (-1.1,.5,2.4,4.4):
            box(works,(xx,-4.33,z0),(2.6,1.0,.29),"black")
            box(lights,(xx+.37,-4.87,z0+.15),(.68,.09,.12),"dim")


def blade(outboard, crossings, lights):
    # The original shows a shorter, deeper port blade than the straight overhead study.
    xs=np.linspace(46.5,96,140)
    mid=lambda x:profile([(46.5,-23.7),(56,-24.9),(68,-26.0),(78,-24.5),(88,-21.8),(96,-18.6)],x)
    width=lambda x:profile([(46.5,.075),(53,1.38),(62,2.55),(73,2.9),(82,2.52),(90,1.61),(96,.065)],x)
    level=lambda x:profile([(46.5,-5.1),(58,-4.04),(69,-1.56),(80,.55),(89,2.23),(96,3.21)],x)
    rows=[]
    for x in xs:
        w,y,z=width(x),mid(x),level(x)
        k=w/2.08
        wave=(.46*math.sin(x*.47)+.24*math.sin(x*.83))*k
        rows.append([(x,y+w*.98,z+.16*k),(x,y+w*.65,z+1.36*k),(x,y+w*.12+wave,z+2.11*k),
                     (x,y-w*.26+.42*wave,z+2.05*k+wave*.28),(x,y-w*.43+.3*wave,z+1.72*k+wave*.55),
                     (x,y-w*.74,z+1.41*k+wave),(x,y-w*1.01,z+.55*k+wave*.75),
                     (x,y-w*1.08,z+.28*k+wave*.70),(x,y-w*.94,z-.01*k+wave*.60),
                     (x,y-w*.94,z-.48*k),(x,y-w*.78,z-.82*k),(x,y-w*.28,z-1.38*k),
                     (x,y+w*.54,z-.98*k),(x,y+w*.92,z-.40*k)])
    strip(outboard,rows,lambda i,j:("blue","ivory","ivory","blue","blue","ivory","silver","black","glass","trim","blue","metal","blue","metal")[j],"metal")
    for x in np.arange(51,92,.72):
        w,y,z=width(x),mid(x),level(x)
        if int(x*3)%7:
            box(lights,(x,y-w*.959,z-.29*w/2.08),(.36,.062,.075+ .065*w/2.08),"lamp" if int(x)%5 else "dim")
    for xx,yy in ((28.7,-11.2),(41.2,-8.1)):
        box(crossings,(xx,yy,-1.43),(2.81,9.7,.56),"metal")
        box(crossings,(xx,yy,-1.09),(2.67,9.6,.16),"silver")
        for off in (-1.4,1.4):
            box(lights,(xx+off,yy-2,-1.44),(.08,1.5,.13),"dim")
    for idx,x in enumerate((54.5,65.25,81.4)):
        start=(x,-4.35,-1.12+idx*.53)
        end=(x+.78,mid(x)+width(x)*.68,level(x)+.76)
        a,b=Vector(start),Vector(end)
        rows=[]
        for t in np.linspace(0,1,25):
            p=a.lerp(b,t); w=1.26+.31*abs(2*t-1)
            rows.append([(p.x-w,p.y,p.z-.15),(p.x-w,p.y,p.z+.89),(p.x-w*.72,p.y,p.z+1.20),
                         (p.x+w*.72,p.y,p.z+1.20),(p.x+w,p.y,p.z+.89),(p.x+w,p.y,p.z-.15)])
        strip(crossings,rows,lambda i,j:("metal","ivory","ivory","silver","glass","black")[j],"metal")
        for j in range(11):
            t=(j+.7)/12; p=a.lerp(b,t); w=1.26+.31*abs(2*t-1)
            box(crossings,(p.x,p.y,p.z+.49),(2*w+.10,.115,1.1),"metal")
            for sign in (-1,1):
                box(lights,(p.x+sign*(w+.017),p.y-.31,p.z+.49),(.060,.43,.16),"dim")


def command_islands(towers, sensors, lights):
    for xmin,xmax,breadth in ((13.0,41.5,2.32),(39.5,95.1,3.18)):
        rows=[]
        for x in np.linspace(xmin,xmax,92):
            t=(x-xmin)/(xmax-xmin); w=breadth*(.04+.97*math.sin(t*math.pi)**.56)
            y=sum(cross(x))/2; z=top_crown(x,0)
            rows.append([(x,y-w*1.15,z-.86),(x,y-w*.99,z+.10),(x,y-w*.89,z+.38),
                         (x,y-w*.71,z+.62),(x,y+w*.71,z+.62),(x,y+w*.89,z+.38),(x,y+w,z+.10),(x,y+w*1.15,z-.86)])
        strip(towers,rows,lambda i,j:("blue","glass","silver","ivory","silver","glass","blue","blue")[j],"ivory")
    for x,rx,ry,h in ((18,7.7,2.4,.59),(42.5,3.2,2.25,.74),(55.6,4.1,3.65,.96),(70.7,4.88,4.15,.76),(86.4,7.6,5.2,1.65),(95.4,2.4,2,.66)):
        far,near=cross(x); y=(far+near)/2; z=top_crown(x,0)+.16
        drum(towers,(x,y,z),rx,ry,[(-2.12,1.24,"blue"),(-.78,1.16,"blue"),(-.36,1.04,"ivory"),(-.22,1,"trim"),(0,.99,"glass"),
                                    (.36*h,.97,"glass"),(.49*h,1.02,"silver"),(.78*h,.92,"ivory"),(1.06*h,.67,"blue")])
        windows(lights,x,y,z+.04,rx*.994,ry*.994,66,.12*h)
        if x in (55.6,42.5,95.4):
            rr=1.95 if x==55.6 else .93
            dome=[(rr*math.sin(a),max(.02,rr*math.cos(a)),"ivory") for a in np.linspace(0,math.pi/2,9)]
            revolved(towers,(x,y,z+h),UP,dome,"silver",40)
        if x==86.4:
            drum(towers,(x,y,z+h),4.95,3.64,[(0,1,"black"),(.40,1,"glass"),(.58,1.03,"silver"),(.93,.83,"blue"),(1.20,.51,"metal")])
            windows(lights,x,y,z+h+.08,4.96,3.64,45,.13)
            rod(sensors,(x+.65,y,z+h+1.15),(x+.65,y,z+h+2.36),.42,"metal")
            dish(sensors,x+.65,y,z+h+2.44,1.90,(.12,-.24,.96))
    x=70.7; f,n=cross(x); y=(f+n)/2; z=top_crown(x,0)+1.05
    axial=Vector((.53,.848,0)); horizontal=Vector((.848,-.53,0)); center=Vector((x,y,z))
    radial=lambda a: horizontal*math.cos(a)+Vector(UP)*math.sin(a)
    rows=[]
    for angle in np.linspace(-.045,math.pi+.045,72):
        v=radial(angle)
        rows.append([center+v*r+axial*d for r,d in ((4.50,-.48),(5.12,-.48),(5.27,-.31),(5.27,.33),
                                                       (5.09,.50),(4.50,.50),(4.39,.26),(4.39,-.26))])
    strip(sensors,rows,lambda i,j:("metal","silver","ivory","ivory","silver","metal","black","metal")[j],"metal")
    for a in np.linspace(.10,math.pi-.10,16):
        v=radial(a)
        rod(sensors,center+v*4.43-axial*.51,center+v*5.1-axial*.51,.043,"trim",6)
    antenna(sensors,79.1,(sum(cross(79.1))/2),top_crown(79.1,0)+.1,6.75,.61)
    antenna(sensors,45.4,(sum(cross(45.4))/2),top_crown(45.4,0)+.31,2.30,.36)
    for xx,yy,rr in ((48.8,-.9,.76),(61.9,.21,.95),(77.4,1.05,.90),(88.8,4.1,.81),(90.3,-3.2,.60)):
        zz=top_crown(xx,0)+.13
        levels=[(rr*math.sin(a),max(.04,rr*math.cos(a)),"ivory") for a in np.linspace(0,math.pi/2,7)]
        revolved(towers,(xx,yy,zz),UP,levels,"silver",27)


def make():
    roof, plates, hull, works, outboard, crossings, towers, sensors, lights = [Part(n) for n in (
        "Glacier · Scalloped upper decks","Glacier · Overlapping armor and window reveals","Glacier · Sealed far hull and keel",
        "Glacier · Recessed port hangar and structure","Glacier · Outboard armored blade","Glacier · Three enclosed crossings",
        "Glacier · Command crown and pressure domes","Glacier · Observation arch and sensors","Glacier · Cabin lights")]
    plate_shell(roof,plates)
    lower_body(hull,works,lights)
    dock_frame(works,lights)
    blade(outboard,crossings,lights)
    command_islands(towers,sensors,lights)
    for y,r in ((-4.05,1.23),(-1.40,1.44),(1.41,1.44),(4.05,1.23)):
        revolved(works,(91.8,y,6.02),AFT,[(0,r*1.39,"metal"),(2.32,r*1.35,"blue"),(3.52,r*1.18,"metal"),
                          (4.35,r,"black"),(4.66,r*.98,"trim"),(4.78,r*.87,"metal"),
                          (4.54,r*.68,"black"),(4.28,r*.38,"dim")],"dim",32)
    return [roof,plates,hull,works,outboard,crossings,towers,sensors,lights]
