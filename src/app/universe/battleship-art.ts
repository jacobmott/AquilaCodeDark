import * as THREE from 'three';
import { Vessel } from './universe-model';

// Original rough exterior. Normalize the 136 × 48 × 6.4 envelope to vessel dimensions on each axis.
// Repeated armor, ribs and running lights share instanced geometry.
export function createBattleship(vessel: Vessel): THREE.Group {
  const root = new THREE.Group(),
    hull = new THREE.Group();
  root.name = 'research-battleship';
  root.add(hull);
  hull.scale.set(vessel.width / 48, vessel.height / 6.4, vessel.length / 136);
  const armor = new THREE.MeshStandardMaterial({
    color: vessel.color,
    metalness: 0.65,
    roughness: 0.48,
  });
  const edge = new THREE.MeshStandardMaterial({
    color: 0x48565e,
    metalness: 0.7,
    roughness: 0.58,
  });
  const dark = new THREE.MeshStandardMaterial({
    color: 0x17242b,
    metalness: 0.55,
    roughness: 0.7,
  });
  const trim = new THREE.MeshStandardMaterial({
    color: vessel.accent,
    metalness: 0.45,
    roughness: 0.5,
  });
  const glass = new THREE.MeshStandardMaterial({
    color: 0x183e48,
    emissive: 0x43808a,
    emissiveIntensity: 0.6,
    metalness: 0.8,
    roughness: 0.2,
  });
  const warm = new THREE.MeshBasicMaterial({ color: 0xf6c98b });
  const cool = new THREE.MeshBasicMaterial({ color: 0xa0e2ec });
  const boxes = new Map<THREE.Material, THREE.Matrix4[]>();
  const transform = new THREE.Object3D();
  const box = (
    x: number,
    y: number,
    z: number,
    w: number,
    h: number,
    l: number,
    material: THREE.Material,
    turn = 0,
  ) => {
    transform.position.set(x, y, z);
    transform.scale.set(w, h, l);
    transform.rotation.set(0, turn, 0);
    transform.updateMatrix();
    if (!boxes.has(material)) boxes.set(material, []);
    boxes.get(material)!.push(transform.matrix.clone());
  };
  const plate = (
    points: number[][],
    bottom: number,
    height: number,
    material: THREE.Material,
  ) => {
    const shape = new THREE.Shape();
    points.forEach(([x, z], i) =>
      i ? shape.lineTo(x, z) : shape.moveTo(x, z),
    );
    shape.closePath();
    const geometry = new THREE.ExtrudeGeometry(shape, {
      depth: height,
      bevelEnabled: false,
      steps: 1,
    });
    geometry.rotateX(Math.PI / 2);
    geometry.translate(0, bottom + height, 0);
    const mesh = new THREE.Mesh(geometry, material);
    hull.add(mesh);
  };
  // Broad split prow, recessed central spine and an armored keel. No saucer or replica hull.
  plate(
    [
      [-3, -68],
      [3, -68],
      [10, -57],
      [12, -40],
      [8, -25],
      [8, 54],
      [6, 62],
      [-6, 62],
      [-8, 54],
      [-8, -25],
      [-12, -40],
      [-10, -57],
    ],
    -1.9,
    3.3,
    armor,
  );
  plate(
    [
      [-2, -66],
      [2, -66],
      [5, -48],
      [4, 54],
      [-4, 54],
      [-5, -48],
    ],
    -3.2,
    1.4,
    dark,
  );
  plate(
    [
      [-1.8, -65],
      [1.8, -65],
      [6, -52],
      [5, -34],
      [-5, -34],
      [-6, -52],
    ],
    1.42,
    0.45,
    edge,
  );
  box(0, 1.6, 9, 4.8, 0.8, 78, edge);
  for (const side of [-1, 1]) {
    // Shoulders protect two open hangar galleries. No wall crosses the lateral mouth.
    plate(
      [
        [side * 8, -40],
        [side * 17, -44],
        [side * 23, -34],
        [side * 24, -24],
        [side * 24, 23],
        [side * 17, 35],
        [side * 10, 26],
      ],
      0.5,
      0.9,
      armor,
    );
    box(side * 16, -2.3, -10, 16, 0.4, 48, edge);
    box(side * 8.4, -0.9, -10, 0.8, 2.4, 48, dark);
    for (let i = 0; i < 12; i++) {
      const z = -32 + i * 4;
      box(side * 8.9, -0.9, z, 0.2, 2.35, 0.22, edge);
      box(side * 8.86, -0.2, z + 1.5, 0.05, 0.13, 1.7, warm);
      box(side * 8.86, -1.25, z + 1.5, 0.05, 0.6, 1.7, edge);
      for (const x of [11, 16, 21])
        box(
          side * x,
          -2.085,
          z + 1.2,
          4.8,
          0.02,
          3.7,
          i % 2 === 0 ? armor : edge,
        );
    }
    for (const z of [-35, 15]) box(side * 16, -0.9, z, 16, 2.4, 2, armor);
    // Ceiling/floor strips make the depth readable without transparent forcefields.
    for (const z of [-31, -20, -9, 2, 12]) {
      box(side * 16, -2.06, z, 14, 0.035, 0.1, warm);
      box(side * 16, 0.295, z, 14, 0.035, 0.08, cool);
    }
    box(side * 23.9, -2.04, -10, 0.12, 0.08, 47, warm);
    box(side * 23.9, 0.44, -10, 0.12, 0.08, 47, cool);
    // Engine shoulders sit aft of the open bay and carry cooling fins.
    plate(
      [
        [side * 8, 25],
        [side * 16, 29],
        [side * 17, 50],
        [side * 12, 61],
        [side * 8, 57],
      ],
      -1.6,
      3.2,
      edge,
    );
    for (let i = 0; i < 12; i++) {
      box(side * 13, 0.18, 30 + i * 1.6, 7, 0.3, 0.34, dark);
      box(side * 11.7, 1.78, 30 + i * 1.6, 4, 0.15, 0.38, armor);
    }
    for (let i = 0; i < 20; i++) {
      const z = -29 + i * 3.8;
      box(side * 5.7, 1.47, z, 3.2, 0.16, 3.5, i % 4 === 0 ? edge : armor);
      box(side * 7.7, 0.1, z, 0.1, 0.5, 0.2, warm);
    }
    for (let i = 0; i < 12; i++) {
      box(
        side * 17,
        1.47,
        -29 + i * 4.5,
        9,
        0.14,
        3.9,
        i % 3 === 0 ? edge : armor,
      );
      box(side * 21.8, 1.58, -28 + i * 4.5, 0.18, 0.07, 1.8, trim);
    }
    // Inert hardpoints reserve future turret locations, without pretending weapons work.
    for (const z of [-48, -29, 8, 39]) {
      box(side * 6, 1.85, z, 2.4, 0.6, 3.4, dark);
      box(side * 6, 2.2, z, 1.9, 0.15, 2.6, armor);
    }
    for (const z of [-54, 20, 54])
      box(side * 8, 1.65, z, 0.16, 0.12, 0.5, side > 0 ? cool : warm);
  }
  // Asymmetric command island with a forward window band and reserved viewing anchors.
  box(3.7, 2, -19, 5.6, 0.8, 14, edge);
  box(3.7, 2.65, -20, 4.8, 0.5, 10, armor);
  box(3.7, 3.02, -22, 4.3, 0.36, 5, edge);
  box(3.7, 2.95, -24.53, 3.9, 0.17, 0.07, glass);
  box(0, 1.9, -58, 0.6, 0.14, 8, trim);
  box(0, 1.47, 49, 2, 0.1, 9, trim);
  // Six recessed drives. Exhaust extends beyond the collision envelope only as light.
  box(0, -0.3, 59, 16, 4.8, 10, edge);
  for (const x of [-5.4, 0, 5.4])
    for (const y of [-1.55, 1.05]) {
      const nozzle = new THREE.Mesh(
        new THREE.CylinderGeometry(1.0, 1.35, 5, 12),
        dark,
      );
      nozzle.rotation.x = Math.PI / 2;
      nozzle.position.set(x, y, 65.5);
      hull.add(nozzle);
      const glow = new THREE.Mesh(new THREE.CircleGeometry(0.85, 16), cool);
      glow.position.set(x, y, 68.01);
      glow.userData['nonSolid'] = true;
      hull.add(glow);
      const exhaust = new THREE.Mesh(
        new THREE.ConeGeometry(0.85, 5, 12),
        new THREE.MeshBasicMaterial({
          color: 0x81dfff,
          transparent: true,
          opacity: 0.13,
          depthWrite: false,
          blending: THREE.AdditiveBlending,
        }),
      );
      exhaust.rotation.x = Math.PI / 2;
      exhaust.position.set(x, y, 70.5);
      exhaust.userData['nonSolid'] = true;
      hull.add(exhaust);
    }
  for (const [material, matrices] of boxes) {
    const mesh = new THREE.InstancedMesh(
      new THREE.BoxGeometry(1, 1, 1),
      material,
      matrices.length,
    );
    matrices.forEach((matrix, i) => mesh.setMatrixAt(i, matrix));
    mesh.instanceMatrix.needsUpdate = true;
    mesh.computeBoundingSphere();
    mesh.userData['nonSolid'] = material === cool || material === warm;
    hull.add(mesh);
  }
  for (const [name, position] of [
    ['bridge-window', [3.7, 2.95, -24.6]],
    ['quarters-window', [-7.9, 0.8, 9]],
    ['port-hangar', [-24, -0.9, -10]],
    ['starboard-hangar', [24, -0.9, -10]],
  ] as const) {
    const anchor = new THREE.Object3D();
    anchor.name = name;
    anchor.position.set(position[0], position[1], position[2]);
    hull.add(anchor);
  }
  root.updateMatrixWorld(true);
  const bounds = new THREE.Box3();
  root.traverse((node) => {
    if ((node as THREE.Mesh).isMesh && !node.userData['nonSolid'])
      bounds.expandByObject(node);
  });
  root.userData['solidBounds'] = {
    min: bounds.min.toArray(),
    max: bounds.max.toArray(),
  };
  return root;
}
