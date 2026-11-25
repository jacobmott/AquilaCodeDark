import * as THREE from 'three';
import { createShip, disposeTree } from './universe-art';
import { FLEET } from './universe-model';
import {
  BOARDING,
  DeckPoint,
  deckRoom,
  EYE_HEIGHT,
  HELM,
  TACTICAL,
  moveOnDeck,
  walkingLookDelta,
} from './interior-model';

export class ShipInterior {
  readonly scene = new THREE.Scene();
  readonly camera = new THREE.PerspectiveCamera(68, 1, 0.05, 300);
  readonly position = new THREE.Vector3(0, EYE_HEIGHT, 2);
  yaw = 0;
  pitch = 0;
  private readonly bridge = new THREE.Group();
  private readonly passage = new THREE.Group();
  private readonly hangar = new THREE.Group();
  private readonly hull = new THREE.MeshStandardMaterial({
    color: 0x334650,
    roughness: 0.73,
    metalness: 0.35,
  });
  private readonly trim = new THREE.MeshStandardMaterial({
    color: 0x84949a,
    roughness: 0.4,
    metalness: 0.6,
  });
  private readonly floor = new THREE.MeshStandardMaterial({
    color: 0x18252c,
    roughness: 0.85,
    metalness: 0.22,
  });
  private readonly dark = new THREE.MeshStandardMaterial({
    color: 0x101a21,
    roughness: 0.65,
    metalness: 0.5,
  });
  private readonly light = new THREE.MeshBasicMaterial({ color: 0xc9e9e8 });
  private readonly amber = new THREE.MeshBasicMaterial({ color: 0xe2b478 });
  private readonly boxGeometry = new THREE.BoxGeometry(1, 1, 1);
  private readonly textures: THREE.Texture[] = [];

  constructor(environment: THREE.Texture) {
    this.scene.environment = environment;
    this.scene.environmentIntensity = 0.6;
    this.scene.add(this.bridge, this.passage, this.hangar);
    this.scene.add(new THREE.HemisphereLight(0xc4e5ed, 0x34404c, 2.4));
    const key = new THREE.DirectionalLight(0xffe9d0, 2.5);
    key.position.set(0, 8, -8);
    this.scene.add(key);
    const fill = new THREE.DirectionalLight(0x82b9d4, 1.2);
    fill.position.set(30, 20, 100);
    this.scene.add(fill);
    this.buildBridge();
    this.buildPassage();
    this.buildHangar();
  }
  private box(
    group: THREE.Group,
    size: number[],
    at: number[],
    material = this.hull,
  ): THREE.Mesh {
    const mesh = new THREE.Mesh(this.boxGeometry, material);
    mesh.scale.set(size[0], size[1], size[2]);
    mesh.position.set(at[0], at[1], at[2]);
    group.add(mesh);
    return mesh;
  }
  private glow(
    group: THREE.Group,
    size: number[],
    at: number[],
    amber = false,
  ): void {
    const mesh = new THREE.Mesh(
      this.boxGeometry,
      amber ? this.amber : this.light,
    );
    mesh.scale.set(size[0], size[1], size[2]);
    mesh.position.set(at[0], at[1], at[2]);
    group.add(mesh);
  }
  private sign(
    group: THREE.Group,
    title: string,
    subtitle: string,
    width: number,
    at: number[],
    yaw = 0,
  ): void {
    const canvas = document.createElement('canvas');
    canvas.width = 1024;
    canvas.height = 256;
    const ctx = canvas.getContext('2d')!;
    ctx.fillStyle = '#10212a';
    ctx.fillRect(0, 0, 1024, 256);
    ctx.fillStyle = '#e8bc83';
    ctx.fillRect(38, 44, 7, 166);
    ctx.font = '500 66px sans-serif';
    ctx.fillStyle = '#edf5f3';
    ctx.fillText(title, 80, 110);
    ctx.font = '26px sans-serif';
    ctx.fillStyle = '#9ec9d4';
    ctx.fillText(subtitle, 82, 178);
    const texture = new THREE.CanvasTexture(canvas);
    texture.colorSpace = THREE.SRGBColorSpace;
    this.textures.push(texture);
    const mesh = new THREE.Mesh(
      new THREE.PlaneGeometry(width, width / 4),
      new THREE.MeshBasicMaterial({ map: texture }),
    );
    mesh.position.set(at[0], at[1], at[2]);
    mesh.rotation.y = yaw;
    group.add(mesh);
  }
  private buildBridge(): void {
    const g = this.bridge;
    this.box(g, [26, 0.3, 26], [0, -0.15, -1], this.floor);
    this.box(g, [26, 0.4, 26], [0, 6.2, -1], this.dark);
    for (const x of [-13, 13]) this.box(g, [0.4, 6, 26], [x, 3, -1]);
    // Forward glazing is open geometry: the first scene pass supplies real surrounding space.
    this.box(g, [26, 0.8, 0.6], [0, 0.4, -14]);
    this.box(g, [26, 1, 0.8], [0, 5.6, -14]);
    for (const x of [-12.7, -6.4, 6.4, 12.7])
      this.box(g, [0.28, 4.4, 0.6], [x, 3, -14], this.trim);
    for (const x of [-8, 8]) this.box(g, [10, 6, 0.5], [x, 3, 12]);
    this.box(g, [6, 1.8, 0.5], [0, 5.1, 12]);
    for (const x of [-3.2, 3.2])
      this.box(g, [0.25, 4.3, 0.8], [x, 2.15, 11.8], this.trim);
    for (const x of [-11.4, 11.4]) {
      this.box(g, [2.8, 1.15, 12], [x, 0.58, -2], this.dark);
      for (let z = -7; z < 4; z += 3) {
        this.glow(g, [1.8, 0.025, 1.1], [x, 1.17, z]);
        this.box(g, [2.6, 1.7, 0.1], [x, 2.3, z - 0.8], this.dark);
      }
    }
    this.box(g, [8, 1.08, 3], [0, 0.54, -5.6], this.dark);
    this.box(g, [8.4, 0.12, 3.2], [0, 1.1, -5.6], this.trim);
    this.sign(
      g,
      'DELTA TREE / HELM',
      'NAVIGATION     /     FLIGHT CONTROL',
      2.4,
      [0, 0.6, -4.08],
    );
    for (const x of [-2.6, 2.6]) {
      this.glow(g, [1.6, 0.03, 1.2], [x, 1.18, -5.2]);
      for (let i = 0; i < 4; i++)
        this.glow(
          g,
          [0.25, 0.04, 0.12],
          [x - 0.5 + i * 0.32, 1.2, -4.35],
          i === 0,
        );
    }
    this.box(g, [2.8, 1.1, 2.4], [6.5, 0.55, -4.95], this.dark);
    this.box(g, [3, 0.12, 2.5], [6.5, 1.13, -4.95], this.trim);
    this.box(g, [2.5, 0.035, 1.7], [6.5, 1.21, -5], this.dark);
    for (const x of [5.28, 7.72])
      this.glow(g, [0.025, 0.02, 1.7], [x, 1.24, -5], true);
    for (let i = 0; i < 5; i++) {
      this.glow(
        g,
        [0.55 + i * 0.12, 0.015, 0.025],
        [5.95, 1.24, -5.6 + i * 0.2],
      );
      this.glow(g, [0.34, 0.015, 0.06], [7.2, 1.24, -5.6 + i * 0.2], i === 4);
    }
    this.sign(
      g,
      'TACTICAL / 03',
      'TARGETING  /  PULSE LANCE',
      2.6,
      [6.5, 0.65, -3.72],
    );
    for (const x of [-7, 7]) {
      this.glow(g, [0.12, 0.02, 24], [x, 0.015, -1], true);
      this.glow(g, [0.24, 0.06, 22], [x, 5.96, -1]);
    }
    for (let z = -12; z < 12; z += 3)
      this.box(g, [25, 0.012, 0.035], [0, 0.009, z], this.trim);
    this.sign(
      g,
      'COMMAND / 01',
      'DELTA TREE     /     EXPLORATION DIVISION',
      6,
      [0, 5.55, -13.5],
    );
    this.sign(
      g,
      'HANGAR / 02',
      'AFT PASSAGE     /     SHUTTLE OPERATIONS',
      5,
      [0, 4.65, 11.65],
      Math.PI,
    );
  }
  private buildPassage(): void {
    const g = this.passage;
    this.box(g, [6, 0.3, 28], [0, -0.15, 25], this.floor);
    this.box(g, [6, 0.3, 28], [0, 4.4, 25], this.dark);
    for (const x of [-3, 3]) {
      this.box(g, [0.3, 4.4, 28], [x, 2.2, 25]);
      this.glow(g, [0.03, 0.09, 28], [x * 0.94, 0.25, 25], true);
      for (let z = 14; z < 38; z += 4)
        this.box(g, [0.18, 4.4, 0.16], [x * 0.94, 2.2, z], this.trim);
    }
    for (let z = 14; z < 38; z += 4)
      this.glow(g, [4.8, 0.06, 0.28], [0, 4.2, z]);
    this.sign(
      g,
      'HANGAR / 02',
      'PEREGRINE     /     FLIGHT DECK',
      4.7,
      [0, 3.7, 37.7],
      Math.PI,
    );
    this.sign(
      g,
      'BRIDGE / 01',
      'COMMAND     /     FORWARD',
      4.7,
      [0, 3.7, 12.7],
    );
  }
  private buildHangar(): void {
    const g = this.hangar;
    this.box(g, [96, 0.4, 108], [0, -0.2, 92], this.floor);
    this.box(g, [96, 0.8, 108], [0, 28, 92], this.dark);
    this.box(g, [0.6, 28, 108], [-48, 14, 92]);
    this.box(g, [96, 28, 0.6], [0, 14, 146]);
    for (const x of [-25.6, 25.6]) this.box(g, [45, 28, 0.6], [x, 14, 38]);
    this.box(g, [6, 23.6, 0.6], [0, 16.2, 38]);
    // Starboard launch aperture. A deck boundary prevents walking into open space.
    for (const z of [49, 141])
      this.box(g, [0.6, 28, z === 49 ? 22 : 10], [48, 14, z]);
    this.box(g, [0.6, 6, 76], [48, 25, 98]);
    this.box(g, [0.6, 0.8, 76], [48, 0.4, 98]);
    for (const z of [60, 136]) this.glow(g, [0.1, 21, 0.25], [47.5, 11.1, z]);
    for (let z = 44; z < 146; z += 12) {
      this.box(g, [96, 0.8, 0.5], [0, 27, z], this.trim);
      this.glow(g, [78, 0.1, 0.35], [0, 26.5, z]);
      this.box(g, [95, 0.02, 0.07], [0, 0.015, z], this.trim);
    }
    for (const x of [-39, 39])
      this.glow(g, [0.14, 0.02, 102], [x, 0.03, 92], true);
    for (let x = -33; x < 34; x += 3)
      this.glow(g, [1.4, 0.02, 0.4], [x, 0.025, 68], true);
    this.box(g, [62, 0.8, 60], [0, 0.4, 102], this.dark);
    const shuttle = createShip(FLEET[0]);
    shuttle.position.set(0, 9.5, 102);
    shuttle.rotation.y = -Math.PI / 2;
    g.add(shuttle);
    for (const x of [-22, 22])
      for (const z of [82, 122]) this.box(g, [2, 4, 2], [x, 2, z], this.trim);
    this.box(g, [2.2, 1.1, 1.2], [0, 0.55, 68], this.dark);
    this.glow(g, [1.7, 0.04, 0.7], [0, 1.12, 68]);
    this.sign(
      g,
      'PEREGRINE / 01',
      'SURVEY SHUTTLE     /     READY FOR LAUNCH',
      1.8,
      [0, 0.6, 67.38],
      Math.PI,
    );
    this.sign(
      g,
      'FLIGHT DECK / 02',
      'STARBOARD GALLERY     /     DELTA TREE',
      20,
      [-47.5, 17, 94],
      Math.PI / 2,
    );
    this.sign(
      g,
      'BRIDGE / 01',
      'FORWARD PASSAGE     /     COMMAND',
      6,
      [0, 5.4, 38.5],
    );
  }
  get point(): DeckPoint {
    return [this.position.x, this.position.z];
  }
  look(dx: number, dy: number, sensitivity: number): void {
    const [yaw, pitch] = walkingLookDelta(dx, dy, sensitivity);
    this.yaw -= yaw;
    this.pitch = THREE.MathUtils.clamp(this.pitch - pitch, -1.3, 1.3);
  }
  walk(keys: ReadonlySet<string>, dt: number, sensitivity: number): void {
    const axis = (p: string, n: string) =>
      Number(keys.has(p)) - Number(keys.has(n));
    this.yaw -= axis('ArrowRight', 'ArrowLeft') * dt * 1.5 * sensitivity;
    this.pitch = THREE.MathUtils.clamp(
      this.pitch + axis('ArrowUp', 'ArrowDown') * dt * sensitivity,
      -1.3,
      1.3,
    );
    const direction = new THREE.Vector3(
      axis('KeyD', 'KeyA'),
      0,
      -axis('KeyW', 'KeyS'),
    );
    if (direction.lengthSq() > 1) direction.normalize();
    direction
      .applyAxisAngle(new THREE.Vector3(0, 1, 0), this.yaw)
      .multiplyScalar(
        dt * (keys.has('ShiftLeft') || keys.has('ShiftRight') ? 10 : 5),
      );
    const next = moveOnDeck(this.point, [direction.x, direction.z]);
    this.position.set(next[0], EYE_HEIGHT, next[1]);
  }
  atBridge(): void {
    this.position.set(0, EYE_HEIGHT, 2);
    this.yaw = this.pitch = 0;
  }
  atHelm(): void {
    this.position.set(HELM[0], EYE_HEIGHT, HELM[1]);
    this.yaw = this.pitch = 0;
  }
  atTactical(): void {
    this.position.set(TACTICAL[0], EYE_HEIGHT, TACTICAL[1]);
    this.yaw = this.pitch = 0;
  }
  atHangar(): void {
    this.position.set(BOARDING[0], EYE_HEIGHT, BOARDING[1]);
    this.yaw = Math.PI;
    this.pitch = 0;
  }
  updateCamera(
    aspect: number,
    helm: boolean,
    orbit: { yaw: number; pitch: number },
  ): void {
    if (this.camera.aspect !== aspect) {
      this.camera.aspect = aspect;
      this.camera.updateProjectionMatrix();
    }
    this.camera.position.copy(this.position);
    this.camera.quaternion.setFromEuler(
      new THREE.Euler(
        helm ? -orbit.pitch : this.pitch,
        helm ? orbit.yaw : this.yaw,
        0,
        'YXZ',
      ),
    );
    // Keep adjacent doorways visible, culling only rooms hidden behind the passage walls.
    const z = this.position.z;
    this.bridge.visible = z < 43;
    this.passage.visible = true;
    this.hangar.visible = z > 8;
  }
  get room(): string {
    return deckRoom(this.position.z);
  }
  dispose(): void {
    this.textures.forEach((t) => t.dispose());
    disposeTree(this.scene);
  }
}
