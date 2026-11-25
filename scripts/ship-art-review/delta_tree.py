"""Delta Tree's forked gallery deck, observatory, and three terraced drive banks."""
import math
import numpy as np
from modeling import AFT, TAU, UP, Part, antenna, box, dish, drum, lin, profile, revolved, rod, strip, windows


PX = 9.59
STATIONS = [31, 41, 56, 82, 130, 188, 281, 370, 464, 558, 644, 697, 716, 728]
FAR = [344, 330, 318, 302, 281, 259, 234, 215, 196, 177, 167, 167, 173, 184]
NEAR = [345, 359, 371, 388, 410, 433, 454, 473, 490, 501, 504, 506, 502, 491]
# Each contour follows the shadowed watercourse in the chosen overhead reference.
COURTS = [
    [(175,329),(208,321),(269,323),(303,335),(296,353),(257,376),(200,393),(166,382),(174,357)],
    [(243,400),(261,380),(317,351),(363,341),(390,348),(391,360),(344,393),(288,416),(252,414)],
    [(305,421),(325,400),(379,372),(423,353),(475,344),(496,353),(487,368),(429,402),(378,436),(335,437)],
    [(398,438),(422,413),(468,387),(503,370),(524,380),(526,401),(489,444),(445,450)],
    [(494,441),(496,418),(501,388),(513,373),(536,383),(578,406),(594,423),(578,442),(548,450),(518,451)],
    [(192,282),(211,274),(253,276),(296,288),(322,301),(327,312),(312,321),(272,318),(222,309),(196,300)],
    [(252,259),(273,250),(301,250),(345,270),(380,294),(397,311),(392,322),(367,324),(336,312),(304,286)],
    [(325,242),(349,231),(374,228),(417,244),(459,271),(481,296),(477,312),(453,323),(423,321),(395,303),(357,263)],
    [(408,224),(436,211),(470,206),(507,223),(528,249),(536,275),(523,293),(501,297),(479,288),(448,267)],
    [(492,207),(519,194),(546,194),(574,203),(601,226),(613,248),(605,263),(574,276),(551,277),(535,264),(519,234)],
    [(578,193),(600,187),(620,188),(644,202),(654,222),(648,232),(632,233),(606,223),(588,210)],
]
DOCKS = [(134,291,1.22),(188,267,1.93),(282,231,1.95),(373,209,2.04),(471,192,2.10),
         (560,173,2.05),(640,157,2.09),(197,404,2.01),(284,423,2.02),(374,442,2.02),(472,451,2.13)]


def point(px, py):
    return (px-31)/PX, (344-py)/PX


def uv(px, py):
    return px/1024, 1-py/688


def edges(px):
    far = profile(list(zip(STATIONS, FAR)), px)
    near = profile(list(zip(STATIONS, NEAR)), px)
    return far, near


def top(px, py):
    far, near = edges(px)
    t = (2*py-near-far)/max(1,near-far)
    return 1.25 + 2.6*(px-31)/697 + .72*(1-t*t)


def smooth_path(points):
    p = np.array(points, dtype=float)
    result = []
    for i,b in enumerate(p):
        a,c,d = p[(i-1)%len(p)],p[(i+1)%len(p)],p[(i+2)%len(p)]
        for t in np.arange(0,1,.125):
            result.append(.5*((2*b)+(-a+c)*t+(2*a-5*b+4*c-d)*t*t+(-a+3*b-3*c+d)*t*t*t))
    return np.array(result)


def recess_field():
    """Metric depth follows the inside distance to each rim, including an inner raised deck."""
    depth = np.zeros((530, 750), np.float32)
    for anchors in COURTS:
        curve = smooth_path(anchors)
        x0,y0 = np.floor(curve.min(axis=0)-2).astype(int)
        x1,y1 = np.ceil(curve.max(axis=0)+3).astype(int)
        yy,xx = np.mgrid[y0:y1,x0:x1]
        def signed_distance(poly):
            inside = np.zeros(xx.shape, bool)
            nearest = np.full(xx.shape, 1.e6)
            for (a,b),(c,d) in zip(poly,np.roll(poly,-1,axis=0)):
                dx,dy = c-a,d-b
                t = np.clip(((xx-a)*dx+(yy-b)*dy)/max(1.e-8,dx*dx+dy*dy),0,1)
                nearest = np.minimum(nearest,(xx-a-t*dx)**2+(yy-b-t*dy)**2)
                if abs(dy)>1.e-7:
                    inside ^= ((b>yy)!=(d>yy)) & (xx < dx*(yy-b)/dy+a)
            return np.sqrt(nearest) * np.where(inside,1,-1)
        outer = signed_distance(curve)
        center = curve.mean(axis=0)
        inner = signed_distance(center+(curve-center)*[.76,.68])
        cut = 2.05*np.clip((outer+.75)/4.2,0,1) - .69*np.clip((inner+.1)/2.8,0,1)
        depth[y0:y1,x0:x1] = np.maximum(depth[y0:y1,x0:x1],cut)
    return depth


def engines(assembly, lights):
    for tier,(count,z,first) in enumerate(((5,-5,65),(4,.1,67.2),(3,5.1,69.5))):
        for n in range(count):
            y = (n-(count-1)/2)*6.65
            end = 100-.105*abs(y)-.60*tier
            start = first+abs(y)*.19
            length = end-start
            r = 3.25 if tier==0 else 3.05
            data = [(0,.72,"metal"),(.07,.80,"black"),(.23,.76,"metal"),(.27,.91,"trim"),
                    (.32,1.00,"ivory"),(.46,1.01,"ivory"),(.50,.92,"metal"),(.54,.98,"black"),
                    (.58,1.0,"metal"),(.82,.98,"metal"),(.86,1.02,"black"),(.88,1.0,"silver"),
                    (.90,.91,"black"),(.955,.96,"trim"),(.99,.92,"trim"),(1,.85,"black"),
                    (.977,.73,"metal"),(.91,.55,"black"),(.90,.39,"trim"),(.92,.31,"metal")]
            revolved(assembly,(start,y,z),AFT,[(u*length,v*r,m) for u,v,m in data],"black",32)
            for u in (.13,.20,.54,.64,.71,.81,.95):
                thick = .14 if u<.3 else .18
                rad = r*(.80 if u<.3 else (.99 if u<.9 else .95))
                revolved(assembly,(start+length*u,y,z),AFT,[(0,rad,"black"),(.08,rad+.08,"silver"),(thick,rad+.08,"metal")],"black",32)
            for j in range(8):
                a = TAU*j/8
                yy,zz = math.cos(a), math.sin(a)
                rod(assembly,(start+.07*length,y+yy*.84*r,z+zz*.84*r),
                    (start+.27*length,y+yy*.84*r,z+zz*.84*r), .21,"metal",8)
                rod(assembly,(start+.575*length,y+yy*r,z+zz*r),(start+.81*length,y+yy*r,z+zz*r),.13,"black",6)
            # Three bolted armor saddles wrap each core. The exhaust bell stays circular.
            for sector in (0,1,2,3):
                angle=(sector+.5)*TAU/4
                radial=np.array([0,math.cos(angle),math.sin(angle)])
                tangent=np.array([0,-math.sin(angle),math.cos(angle)])
                for u,v,finish in ((.10,.31,"metal"),(.31,.49,"ivory"),(.53,.84,"metal")):
                    center=np.array([start,y,z]); rows=[]
                    for f,spread,swell in ((u,.57,.90),(u+.025,.82,1.025),(v-.025,.82,1.025),(v,.60,.92)):
                        root=center+np.array([f*length,0,0])
                        rows.append([root+r*(radial*rad+tangent*off) for rad,off in
                                     ((.70,-spread*.80),(swell-.14,-spread),(swell,-spread*.72),
                                      (swell,spread*.72),(swell-.14,spread),(.70,spread*.80))])
                    strip(assembly,rows,lambda i,j: "black" if j==5 else finish,finish)
                    for side in (-1,1):
                        aa=center+np.array([(u+.040)*length,0,0])+r*(radial*1.035+side*.43*tangent)
                        bb=center+np.array([(v-.042)*length,0,0])+r*(radial*1.035+side*.43*tangent)
                        rod(assembly,aa,bb,.049,"trim",5)
                    root=center+np.array([(.46*u+.54*v)*length,0,0])
                    def panel(a,b):
                        return root+np.array([a,0,0])+r*(radial*1.037+tangent*b)
                    assembly.quad([panel(-.61,-.25),panel(.61,-.25),panel(.61,.24),panel(-.61,.24)],"black")
                    assembly.quad([panel(-.51,-.19)+radial*.014,panel(.46,-.19)+radial*.014,
                                   panel(.46,.18)+radial*.014,panel(-.51,.18)+radial*.014],"metal")
            for j in range(20):
                a,b=TAU*j/20,TAU*(j+.68)/20
                assembly.quad([(end-.47,y+math.cos(a)*r*.78,z+math.sin(a)*r*.78),
                               (end-.44,y+math.cos(b)*r*.78,z+math.sin(b)*r*.78),
                               (end-.95,y+math.cos(b+.12)*r*.48,z+math.sin(b+.12)*r*.48),
                (end-.99,y+math.cos(a+.12)*r*.48,z+math.sin(a+.12)*r*.48)],"trim")
            revolved(lights,(end-1.12,y,z),AFT,[(0,r*.30,"black"),(.12,r*.23,"dim")],"dim",28)


def make():
    deck, courts, belt, hatch, docks, bridge, gear, drives, lights = [
        Part(name) for name in ("Delta · Cut channel deck","Delta · Inset courtyard terraces","Delta · Pressure hull and keel","Delta · Recessed service doors",
                                "Delta · Branch docking rotundas","Delta · Command terraces and forward canopy",
                                "Delta · Observatory and aerials","Delta · Twelve separate drive housings","Delta · Cabin glazing")]
    fields = recess_field()
    pxs = np.linspace(31,728,350)
    rows, coords, cuts = [], [], []
    for px in pxs:
        f,n = edges(px)
        yp = np.linspace(f,n,148)
        xp = np.full_like(yp,px)
        i,j = int(px),yp.astype(int)
        fx,fy=px-i,yp-j
        cut = (1-fy)*((1-fx)*fields[j,i]+fx*fields[j,i+1]) + fy*((1-fx)*fields[j+1,i]+fx*fields[j+1,i+1])
        x,y = point(xp,yp)
        z = top(px,yp)-cut
        rows.append(list(zip(x,y,z)))
        coords.append([uv(px,p) for p in yp])
        cuts.append(cut)
    # Near-to-far winding at the upper surface gives outward normals.
    strip(deck, rows, "dorsal", loop=False, uv=lambda i,j:coords[i][j])
    # Avoid stretching the illustrated deck paint down the exposed retaining walls.
    for i in range(len(pxs)-1):
        for j in range(len(cuts[i])-1):
            drops=(cuts[i][j],cuts[i][j+1],cuts[i+1][j],cuts[i+1][j+1])
            if max(drops)-min(drops)>.52 and max(drops)>.81:
                idx=i*(len(cuts[i])-1)+j
                deck.materials[idx]="metal"
                deck.uvs[idx]=None
    for poly in COURTS:
        curve=smooth_path(poly)
        center=curve.mean(axis=0)
        # Each branch encloses a lower, walkable-looking island above the bottom of the channel.
        rings=[]; top_uv=[]
        for fx,fy,dip in ((.77,.66,1.73),(.76,.65,1.30),(.73,.62,1.06)):
            contour=center+(curve[::2]-center)*[fx,fy]
            ring=[]; uvs=[]
            for u,v in contour:
                xx,yy=point(u,v); ring.append((xx,yy,top(u,v)-dip)); uvs.append(uv(u,v))
            rings.append(ring); top_uv=uvs
        ids=strip(courts,rings,lambda i,j:"metal" if i==0 else "trim")
        cx,cy=point(*center); crown=top(*center)-1.09
        cid=courts.points([(cx,cy,crown)])[0]
        for j,vtx in enumerate(ids[-1]):
            k=(j+1)%len(ids[-1])
            courts.face((vtx,cid,ids[-1][k]),"dorsal",(top_uv[j],uv(*center),top_uv[k]))
        for j in range(4):
            a=center+(curve[j*(len(curve)//4)]-center)*.52
            xx,yy=point(*a)
            drum(courts,(xx,yy,top(*a)-1.12),.20,.15,[(0,1,"metal"),(.11,.84,"trim"),(.14,.65,"silver")],12)
        curve=center+(curve-center)*.955
        for j in range(0,len(curve)-1,3):
            a,b=curve[j],curve[j+1]
            distance=math.dist(a,b)
            if distance<.65:
                continue
            a,b=tuple(a),tuple(b)
            ax,ay=point(*a); bx,by=point(*b)
            az,bz=top(*a)-.64,top(*b)-.64
            lights.quad([(ax,ay,az),(bx,by,bz),(bx,by,bz+.085),(ax,ay,az+.085)],"dim")
    hull_rows = []
    for px in pxs[::2].tolist()+[728]:
        f,n = edges(px)
        x,far=point(px,f); _,near=point(px,n)
        mid=(far+near)/2
        thick = profile([(0,.65),(3,3.0),(13,5.25),(40,7.1),(62,8.4),(73,8.5)],x)
        profile_row=[]
        for sign,ymax,py in ((1,far,f),(-1,near,n)):
            h = top(px,py)
            for frac, drop in ((1,0),(1.018,.12),(1.028,.23),(1.005,.33),(.995,.51),(.97,.61),(.87,.78),(.56,.95),(.02,1)):
                y=lin(mid,ymax,frac)
                if .28 <= drop <= .78:
                    inset=profile([(55,0),(59,.1),(61,2.7),(66,2.9),(69,.1),(73,0)],x)
                    y-=sign*inset
                profile_row.append((x,y,h-thick*drop))
        hull_rows.append(profile_row[:9]+profile_row[9:][::-1])
    band_mats=["ivory","metal","trim","black","black","metal","ivory","black","black",
               "black","ivory","metal","black","black","trim","metal","ivory","ivory"]
    strip(belt,hull_rows,lambda i,j:band_mats[j], "metal", loop=False)
    for x in np.arange(2.1,71.4,.69):
        if 58.6<x<69.6 or int(x*13)%23==0:
            continue
        for sign in (1,-1):
            for level in (.354,.414,.474):
                corners=[]
                for xx,dh in ((x,0),(x+.36,0),(x+.36,.105),(x,.105)):
                    px=31+PX*xx; f,n=edges(px); py=f if sign==1 else n
                    _,y=point(px,py); _,y2=point(px,(f+n)/2)
                    thick=profile([(0,.65),(3,3.0),(13,5.25),(40,7.1),(62,8.4),(73,8.5)],xx)
                    corners.append((xx,lin(y2,y,1.014),top(px,py)-thick*level+dh))
                lights.quad(corners,"lamp" if int(3*x)%7 else "dim")
    def canopy(part,x,y,z,rx,ry):
        drum(part,(x,y,z),rx,ry,[(0,1,"black"),(.22,1,"trim"),(.38,.99,"glass"),(.76,.95,"glass"),
                                 (1.14,.83,"glass"),(1.5,.63,"ivory"),(1.79,.31,"metal")])
        for phi in np.linspace(0,TAU,23)[:-1]:
            rib=[(x+rx*rr*math.cos(phi),y+ry*rr*math.sin(phi),z+hh) for hh,rr in
                 ((.35,1.003),(.76,.963),(1.14,.848),(1.51,.649),(1.79,.318))]
            for a,b in zip(rib,rib[1:]):
                rod(part,a,b,.074,"trim",6)
    x,y=point(110,345)
    canopy(bridge,x,y,top(110,345),5.19,2.88)
    for i,(px,py,r) in enumerate(DOCKS):
        x,y=point(px,py); z=top(px,py)-.12
        drum(docks,(x,y,z),r,r,[(0,.66,"black"),(.32,.72,"ivory"),(.51,.99,"trim"),(.70,1,"black"),
                                   (.9,.99,"ivory"),(1.02,.86,"ivory"),(1.13,.54,"metal")],44)
        windows(lights,x,y,z+.72,r*.997,r*.997,27,.10)
        drum(docks,(x,y,z+1.04),r*.43,r*.38,[(0,.99,"metal"),(.36,.87,"silver"),(.55,.42,"metal")],24)
        box(docks,(x-.34,y+.34,z+1.20),(.72,.48,.46),"ivory",i*.47)
        for j in (0,1):
            angle=i*.78+j*math.pi; dx,dy=math.cos(angle),math.sin(angle)
            rod(docks,(x+dx*r*.79,y+dy*r*.79,z+.68),(x+dx*(r+1.34),y+dy*(r+1.34),z+.71),.062,"metal",6,.034)
        rod(gear,(x,y,z+1.47),(x,y,z+3.28),.065,"metal",7,.019)
    x,y=point(612,329); h=top(612,329)
    terraces=[(0,1,"black"),(.40,1.03,"metal"),(.55,1.06,"trim"),(.91,1.01,"ivory"),(1.15,.94,"black"),
              (1.80,.93,"glass"),(2.00,.99,"trim"),(2.22,.96,"ivory"),(2.72,.78,"ivory"),(2.93,.73,"black"),
              (3.42,.72,"glass"),(3.58,.78,"ivory"),(3.92,.64,"ivory"),(4.12,.42,"metal")]
    drum(bridge,(x,y,h),6.35,6.18,terraces,96)
    windows(lights,x,y,h+1.32,6.35*.95,6.18*.95,95,.21)
    windows(lights,x,y,h+3.02,6.35*.738,6.18*.738,70,.16)
    for j in range(28):
        phi=TAU*j/28; dx,dy=math.cos(phi),math.sin(phi)
        rod(bridge,(x+6.35*.94*dx,y+6.18*.94*dy,h+2.24),
                    (x+6.35*.78*dx,y+6.18*.78*dy,h+2.74),.038,"metal",5)
        if j%4==0:
            box(bridge,(x+4.91*dx,y+4.77*dy,h+2.68),(.95,.57,.16),"ivory",phi)
    revolved(gear,(x-.45,y,h+4.05),UP,[(0,1.18,"black"),(.36,1.1,"trim"),(.59,.62,"metal"),(1.68,.48,"silver")],"metal")
    dish(gear,x-.45,y,h+5.70,2.52)
    antenna(gear,68.85,1.55,4.3,10.7,1.13)
    for xx,yy,hh,r in ((50.2,8.7,3.6,.44),(48.8,-8.0,2.3,.30),(64.0,12.6,3.0,.46)):
        antenna(gear,xx,yy,top(31+PX*xx,344-PX*yy),hh,r)
    for sign in (-1,1):
        y=sign*16.72
        outline=[(-4.50,-2.72),(-4.50,.83),(-3.24,2.49),(-2.55,2.82),(2.55,2.82),(3.24,2.49),(4.50,.83),(4.50,-2.72)]
        rings=[[(63.66+a*sx,y+sign*off,zz*sz) for a,zz in outline] for off,sx,sz in
               ((-1.76,1.26,1.38),(.7,1.25,1.38),(1.24,1.16,1.25),(1.33,1,1),(1.01,.94,.96),(-1.50,.84,.91))]
        strip(hatch,rings,lambda k,j:("metal","ivory","ivory","trim","metal")[k],loop=False)
        box(hatch,(63.66,y,-2.75),(9.3,4.8,.47),"metal")
        box(hatch,(63.66,y-sign*1.58,-.37),(7.10,.32,3.72),"black")
        for k in range(9):
            box(hatch,(60.63+k*.75,y-sign*1.36,-.36),(.48,.12,2.90),"dim" if k%3 else "metal")
        for xx in (60.05,67.2):
            for inset in (-.7,.22,.88):
                box(lights,(xx,y+sign*inset,.74),(.16,.21,1.72),"lamp")
    for yy in (-12.5,-6.3,0,6.3,12.5):
        # A tapered yoke carries each bank into the hull ahead of the first drive collar.
        rows=[]
        for xx,half,z0 in ((64.6,1.28,3.1),(66.2,1.40,4.05),(69,1.32,5.18),(74.8,1.05,4.55)):
            rows.append([(xx,yy-half,z0-.82),(xx,yy-half,z0),(xx,yy-half*.55,z0+.65),
                         (xx,yy+half*.55,z0+.65),(xx,yy+half,z0),(xx,yy+half,z0-.82)])
        strip(drives,rows,lambda a,b:"metal" if b!=2 else "silver","metal")
    saddle=[]
    for t in np.linspace(-1,1,57):
        zz=7.44-5.38*abs(t)**2.5; xx=67.8-1.62*(1-t*t); yy=t*16.6
        saddle.append([(xx-1.35,yy,zz-2.32),(xx-1.35,yy,zz-.47),(xx-.87,yy,zz+.03),
                       (xx+.72,yy,zz+.19),(xx+1.26,yy,zz-.37),(xx+1.40,yy,zz-2.15)])
    strip(drives,saddle,lambda i,j:"black" if j in (0,5) else "metal","metal")
    for xx in (16.2,25.7,36.2,47.6,54.6):
        for sign in (-1,1):
            far,near=edges(31+PX*xx); _,y=point(31+PX*xx,near if sign<0 else far)
            box(belt,(xx,y*.47,-2.75-.027*xx),(3.8,1.15,1.29),"black")
            for offset in (-1.2,-.4,.4,1.2):
                box(belt,(xx+offset,y*.47,-3.43-.027*xx),(.27,1.12,.46),"metal")
    engines(drives,lights)
    return [deck,courts,belt,hatch,docks,bridge,gear,drives,lights]
