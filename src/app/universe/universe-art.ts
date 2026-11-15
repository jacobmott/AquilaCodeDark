import * as THREE from 'three';
import { seedRandom, Vessel } from './universe-model';
import { createBattleship } from './battleship-art';

const noise = `
float hash(vec3 p) { p = fract(p * .3183099 + vec3(.1,.2,.3)); p *= 17.; return fract(p.x*p.y*p.z*(p.x+p.y+p.z)); }
float noise3(vec3 p) {
  vec3 i=floor(p), f=fract(p); f=f*f*(3.-2.*f);
  return mix(mix(mix(hash(i),hash(i+vec3(1,0,0)),f.x),mix(hash(i+vec3(0,1,0)),hash(i+vec3(1,1,0)),f.x),f.y),
    mix(mix(hash(i+vec3(0,0,1)),hash(i+vec3(1,0,1)),f.x),mix(hash(i+vec3(0,1,1)),hash(i+vec3(1,1,1)),f.x),f.y),f.z);
}
float fbm(vec3 p) { float v=0.; float a=.5; for(int i=0;i<5;i++){ v+=a*noise3(p);p=p*2.04+vec3(13.1,7.2,4.3);a*=.5;}return v; }
`;

// Original procedural surface textures. No fetched images or external models.
const cloudNoise = `
float cloudCover(vec3 p,float seed){
  float swirl=sin(p.y*7.+seed)*.28+fbm(p*4.+seed)*.6;
  p.xz=mat2(cos(swirl),-sin(swirl),sin(swirl),cos(swirl))*p.xz;
  float broad=fbm(p*7.+vec3(seed,seed*.1,2.));
  float wisps=fbm(p*48.+seed);
  return smoothstep(.50,.69,broad+wisps*.12)*(.65+wisps*.35);
}`;

function hullMaterial(
  parameters: THREE.MeshStandardMaterialParameters,
): THREE.MeshStandardMaterial {
  const material = new THREE.MeshStandardMaterial(parameters);
  // Object-space projection keeps bevels and differently sized parts at one texture scale.
  material.onBeforeCompile = (shader) => {
    shader.vertexShader =
      `varying vec3 hullPoint;varying vec3 hullNormal;\n${shader.vertexShader}`.replace(
        '#include <begin_vertex>',
        '#include <begin_vertex>\nhullPoint=position;hullNormal=normal;',
      );
    shader.fragmentShader =
      `varying vec3 hullPoint;varying vec3 hullNormal;\n${noise}\n${shader.fragmentShader}`
        .replace(
          '#include <map_fragment>',
          `#include <map_fragment>
        vec3 hn=abs(normalize(hullNormal));
        vec2 panelUv=hn.y>hn.x&&hn.y>hn.z?hullPoint.xz:(hn.x>hn.z?hullPoint.zy:hullPoint.xy);
        vec2 panelGrid=panelUv*vec2(.24,.13);
        panelGrid.x+=floor(panelGrid.y)*.5;
        vec2 cell=fract(panelGrid), aa=max(fwidth(panelGrid),vec2(.002));
        vec2 edge=min(cell,1.-cell);
        float seam=1.-smoothstep(.006,.018+max(aa.x,aa.y),min(edge.x,edge.y));
        float panelTint=hash(vec3(floor(panelGrid),3.));
        float grain=noise3(hullPoint*24.);
        float scuff=noise3(vec3(panelUv*vec2(2.,65.),4.));
        float wear=smoothstep(.68,.85,scuff)*smoothstep(.45,.8,noise3(hullPoint*.7));
        diffuseColor.rgb*=mix(.91,1.03,panelTint)*(1.-seam*.28)*(.96+grain*.07);
        diffuseColor.rgb=mix(diffuseColor.rgb,vec3(.42,.47,.5),wear*.2);
      `,
        )
        .replace(
          '#include <roughnessmap_fragment>',
          `#include <roughnessmap_fragment>
        roughnessFactor=clamp(roughnessFactor+(grain-.5)*.2+seam*.15-wear*.2,.24,.88);
      `,
        );
  };
  material.customProgramCacheKey = () => 'aquila-hull-surface-v1';
  return material;
}

export function createStarlightEnvironment(): THREE.DataTexture {
  const width = 256,
    height = 128,
    data = new Float32Array(width * height * 4);
  const sun = new THREE.Vector3(-0.7, 0.6, 1).normalize();
  const rim = new THREE.Vector3(0.7, 0.2, -1).normalize();
  const direction = new THREE.Vector3();
  for (let y = 0; y < height; y++)
    for (let x = 0; x < width; x++) {
      const phi = (x / width) * Math.PI * 2,
        theta = (y / height) * Math.PI;
      direction.set(
        Math.sin(theta) * Math.cos(phi),
        Math.cos(theta),
        Math.sin(theta) * Math.sin(phi),
      );
      const key = Math.pow(Math.max(0, direction.dot(sun)), 28) * 3;
      const edge = Math.pow(Math.max(0, direction.dot(rim)), 12) * 1.2;
      const ambient = 0.04 + Math.max(0, direction.y) * 0.08;
      const i = (y * width + x) * 4;
      data.set(
        [
          ambient + key + edge * 0.32,
          ambient * 1.15 + key * 0.88 + edge * 0.62,
          ambient * 1.5 + key * 0.7 + edge,
          1,
        ],
        i,
      );
    }
  const texture = new THREE.DataTexture(
    data,
    width,
    height,
    THREE.RGBAFormat,
    THREE.FloatType,
  );
  texture.mapping = THREE.EquirectangularReflectionMapping;
  texture.colorSpace = THREE.LinearSRGBColorSpace;
  texture.needsUpdate = true;
  return texture;
}

export function createSky(): THREE.Mesh {
  return new THREE.Mesh(
    new THREE.SphereGeometry(1800000, 20, 12),
    new THREE.ShaderMaterial({
      side: THREE.BackSide,
      depthWrite: false,
      uniforms: {
        surface: { value: 0 },
        detail: { value: 0.6 },
        upDirection: { value: new THREE.Vector3(0, 1, 0) },
      },
      vertexShader:
        'varying vec3 direction; void main(){direction=position;gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.);}',
      fragmentShader: `varying vec3 direction; uniform float surface;uniform float detail;uniform vec3 upDirection; ${noise}
      void main(){ vec3 p=normalize(direction);
      if(surface>.5){float h=max(0.,dot(p,upDirection));
        vec3 color=mix(vec3(.38,.52,.56),vec3(.07,.19,.29),pow(h,.55));
        float sun=pow(max(0.,dot(p,normalize(vec3(-.7,.6,1.)))),64.);
        color+=vec3(.45,.37,.22)*sun;gl_FragColor=vec4(color,1.);return;}
      float n=detail>.1?fbm(p*4.5):noise3(p*4.5); float mist=detail>.8?fbm(p*12.+8.):noise3(p*12.+8.);
      float band=pow(max(0.,1.-abs(p.y*.9+p.x*.35-.1)),5.);
      vec3 col=vec3(.007,.013,.023);
      col+=vec3(.035,.075,.085)*pow(n,2.)*band*2.;
      col+=vec3(.12,.072,.052)*pow(mist,3.)*band;
      col*=1.-smoothstep(.54,.72,n)*.7;
      gl_FragColor=vec4(col,1.); }`,
    }),
  );
}

export function createStars(): THREE.Points {
  const rng = seedRandom(5189);
  const positions = new Float32Array(6500 * 3);
  const colors = new Float32Array(6500 * 3);
  for (let i = 0; i < 6500; i++) {
    const az = rng() * Math.PI * 2,
      y = rng() * 2 - 1,
      r = Math.sqrt(1 - y * y);
    positions.set(
      [Math.cos(az) * r * 1600000, y * 1600000, Math.sin(az) * r * 1600000],
      i * 3,
    );
    const glow = Math.pow(rng(), 2) * 0.75 + 0.12;
    colors.set(
      [glow, glow * (0.85 + rng() * 0.15), glow * (0.75 + rng() * 0.25)],
      i * 3,
    );
  }
  const geometry = new THREE.BufferGeometry();
  geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
  geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));
  return new THREE.Points(
    geometry,
    new THREE.PointsMaterial({
      size: 1.5,
      sizeAttenuation: false,
      vertexColors: true,
      transparent: true,
      opacity: 0.9,
      depthWrite: false,
    }),
  );
}

function hullOutline(
  points: number[][],
  depth: number,
  material: THREE.Material,
): THREE.Mesh {
  const shape = new THREE.Shape();
  points.forEach(([x, z], index) =>
    index === 0 ? shape.moveTo(x, z) : shape.lineTo(x, z),
  );
  shape.closePath();
  const geometry = new THREE.ExtrudeGeometry(shape, {
    depth,
    bevelEnabled: true,
    bevelThickness: Math.min(0.45, depth * 0.18),
    bevelSize: Math.min(0.55, depth * 0.3 + 0.15),
    bevelSegments: 3,
    steps: 1,
  });
  geometry.rotateX(Math.PI / 2);
  geometry.translate(0, depth / 2, 0);
  return new THREE.Mesh(geometry, material);
}

export function createShip(vessel: Vessel): THREE.Group {
  if (vessel.capital) return createBattleship(vessel);
  const ship = new THREE.Group();
  const armor = hullMaterial({
    color: vessel.color,
    roughness: 0.42,
    metalness: 0.55,
  });
  const dark = hullMaterial({
    color: 0x252c33,
    roughness: 0.6,
    metalness: 0.75,
  });
  const inset = hullMaterial({
    color: 0x56636b,
    roughness: 0.42,
    metalness: 0.7,
  });
  const trim = hullMaterial({
    color: vessel.accent,
    roughness: 0.48,
    metalness: 0.45,
  });
  const cockpit = new THREE.MeshPhysicalMaterial({
    color: 0x0d2e3a,
    emissive: 0x174250,
    emissiveIntensity: 0.2,
    metalness: 0.75,
    roughness: 0.16,
    clearcoat: 1,
    clearcoatRoughness: 0.1,
  });
  const engine = new THREE.MeshBasicMaterial({ color: 0x9ce8ff });
  const long = vessel.id === 'aegis' ? 1.25 : vessel.id === 'osprey' ? 0.8 : 1;
  const wide = vessel.id === 'aegis' ? 1.3 : vessel.id === 'osprey' ? 0.82 : 1;
  ship.add(
    hullOutline(
      [
        [0, -32 * long],
        [5, -24 * long],
        [9, -8],
        [10, 20],
        [6, 28 * long],
        [-6, 28 * long],
        [-10, 20],
        [-9, -8],
        [-5, -24 * long],
      ],
      6,
      armor,
    ),
  );
  const keel = hullOutline(
    [
      [0, -29 * long],
      [4, -12],
      [5, 22],
      [-5, 22],
      [-4, -12],
    ],
    4,
    dark,
  );
  keel.position.y = -4;
  ship.add(keel);
  const canopy = hullOutline(
    [
      [0, -20 * long],
      [4, -13],
      [4, -3],
      [-4, -3],
      [-4, -13],
    ],
    3.4,
    cockpit,
  );
  canopy.position.y = 4;
  ship.add(canopy);
  const spine = hullOutline(
    [
      [-2.5, -9.5],
      [2.5, -9.5],
      [2.5, 9.5],
      [-2.5, 9.5],
    ],
    3.8,
    armor,
  );
  spine.position.set(0, 5, 10);
  ship.add(spine);
  for (const side of [-1, 1]) {
    const wing = hullOutline(
      [
        [side * 7, -12],
        [side * 29 * wide, 11],
        [side * 32 * wide, 24],
        [side * 13, 20],
      ],
      2.5,
      armor,
    );
    ship.add(wing);
    const panel = hullOutline(
      [
        [side * 10, -4],
        [side * 24 * wide, 12],
        [side * 25 * wide, 17],
        [side * 13, 13],
      ],
      0.6,
      inset,
    );
    panel.position.y = 2.2;
    ship.add(panel);
    const stripe = hullOutline(
      [
        [side * 20 * wide, 10],
        [side * 22 * wide, 11],
        [side * 27 * wide, 20],
        [side * 25 * wide, 20],
      ],
      0.35,
      trim,
    );
    stripe.position.y = 2.9;
    ship.add(stripe);
    const pod = new THREE.Mesh(
      new THREE.CylinderGeometry(3.2, 4, 26, 24),
      dark,
    );
    pod.rotation.x = Math.PI / 2;
    pod.position.set(side * 19 * wide, -1, 17);
    ship.add(pod);
    const podArmor = hullOutline(
      [
        [-3, -9],
        [3, -9],
        [3, 9],
        [-3, 9],
      ],
      4,
      armor,
    );
    podArmor.position.set(side * 19 * wide, 2, 14);
    ship.add(podArmor);
    const nozzle = new THREE.Mesh(
      new THREE.CylinderGeometry(2.7, 2.7, 2, 16),
      engine,
    );
    nozzle.rotation.x = Math.PI / 2;
    nozzle.position.set(side * 19 * wide, -1, 31);
    ship.add(nozzle);
    const rim = new THREE.Mesh(
      new THREE.TorusGeometry(3.05, 0.55, 8, 28),
      inset,
    );
    rim.position.set(side * 19 * wide, -1, 31.2);
    ship.add(rim);
    const exhaust = new THREE.Mesh(
      new THREE.ConeGeometry(2.7, 18, 16),
      new THREE.MeshBasicMaterial({
        color: 0x70c6ff,
        transparent: true,
        opacity: 0.26,
        depthWrite: false,
        blending: THREE.AdditiveBlending,
      }),
    );
    exhaust.rotation.x = Math.PI / 2;
    exhaust.position.set(side * 19 * wide, -1, 40);
    exhaust.name = 'exhaust';
    ship.add(exhaust);
    const fin = hullOutline(
      [
        [side * 7, 8],
        [side * 10, 10],
        [side * 10, 24],
        [side * 7, 22],
      ],
      6,
      dark,
    );
    fin.position.y = 6;
    ship.add(fin);
    for (let i = 0; i < 4; i++) {
      const vent = new THREE.Mesh(new THREE.BoxGeometry(3, 0.35, 1), dark);
      vent.position.set(side * 6, 3.6, 4 + i * 3);
      ship.add(vent);
    }
    const nav = new THREE.Mesh(
      new THREE.SphereGeometry(0.7, 8, 6),
      new THREE.MeshBasicMaterial({ color: side > 0 ? 0x7ce6bb : 0xff9476 }),
    );
    nav.position.set(side * 30 * wide, 2, 22);
    ship.add(nav);
  }
  if (vessel.id === 'aegis') {
    for (const side of [-1, 1]) {
      const hangar = new THREE.Mesh(new THREE.BoxGeometry(11, 10, 31), dark);
      hangar.position.set(side * 13, -6, 8);
      ship.add(hangar);
      const door = new THREE.Mesh(new THREE.BoxGeometry(8, 6, 0.5), cockpit);
      door.position.set(side * 13, -6, 23.8);
      ship.add(door);
    }
  }
  // Recessed inspection hatches and fasteners give the hull a human scale.
  const bolts = new THREE.InstancedMesh(
    new THREE.CylinderGeometry(0.17, 0.17, 0.12, 6),
    dark,
    24,
  );
  const boltTransform = new THREE.Matrix4();
  let boltIndex = 0;
  for (const side of [-1, 1]) {
    for (const z of [-1, 8, 17]) {
      const hatch = hullOutline(
        [
          [side * 4, z - 1.8],
          [side * 7.8, z - 1.8],
          [side * 7.8, z + 1.8],
          [side * 4, z + 1.8],
        ],
        0.14,
        inset,
      );
      hatch.position.y = 3.48;
      ship.add(hatch);
      for (const x of [4.45, 7.35])
        for (const dz of [-1.35, 1.35]) {
          boltTransform.makeTranslation(side * x, 3.69, z + dz);
          bolts.setMatrixAt(boltIndex++, boltTransform);
        }
    }
    const rail = new THREE.Mesh(new THREE.BoxGeometry(0.3, 0.3, 12), trim);
    rail.position.set(side * 3.8, 6.05, -8);
    ship.add(rail);
  }
  ship.add(bolts);
  // The catalog dimensions now describe the actual solid hull. Exhaust is visual only.
  ship.updateMatrixWorld(true);
  const bounds = new THREE.Box3();
  ship.children
    .filter((node) => node.name !== 'exhaust')
    .forEach((node) => bounds.expandByObject(node));
  const size = bounds.getSize(new THREE.Vector3()),
    center = bounds.getCenter(new THREE.Vector3());
  const frame = new THREE.Group();
  frame.add(ship);
  ship.scale.set(
    vessel.width / size.x,
    vessel.height / size.y,
    vessel.length / size.z,
  );
  ship.position.copy(center.multiply(ship.scale).negate());
  return frame;
}

export function disposeTree(root: THREE.Object3D): void {
  const geometries = new Set<THREE.BufferGeometry>(),
    materials = new Set<THREE.Material>();
  root.traverse((node) => {
    const mesh = node as THREE.Mesh;
    if (mesh.geometry) geometries.add(mesh.geometry);
    if (mesh.material)
      (Array.isArray(mesh.material) ? mesh.material : [mesh.material]).forEach(
        (m) => materials.add(m),
      );
  });
  geometries.forEach((g) => g.dispose());
  materials.forEach((m) => m.dispose());
}
