import * as THREE from 'three';
import {
  PracticeBattle,
  SHIELD_SIZE,
  DRONE_RADIUS,
  TURRET_PIVOTS,
  BARREL_LENGTH,
  MAX_PROJECTILES,
  MAX_IMPACTS,
} from './combat-model';

// Shared geometry and fixed instance pools. Every simulated round remains visible at every quality.
export class CombatArt {
  readonly root = new THREE.Group();
  private readonly drone = new THREE.Group();
  private readonly guns = new THREE.Group();
  private readonly mounts: {
    pivot: THREE.Group;
    barrel: THREE.Group;
    flash: THREE.Mesh;
  }[] = [];
  private readonly shipShield: THREE.Mesh<
    THREE.SphereGeometry,
    THREE.MeshBasicMaterial
  >;
  private readonly droneShield: THREE.Mesh<
    THREE.SphereGeometry,
    THREE.MeshBasicMaterial
  >;
  private readonly rounds: THREE.InstancedMesh;
  private readonly halos: THREE.InstancedMesh;
  private readonly cores: THREE.InstancedMesh;
  private readonly rings: THREE.InstancedMesh;
  private readonly sparks: THREE.InstancedMesh;
  private readonly transform = new THREE.Object3D();
  private readonly color = new THREE.Color();
  private readonly a = new THREE.Vector3();
  private readonly direction = new THREE.Vector3();
  private readonly inverse = new THREE.Quaternion();
  private readonly up = new THREE.Vector3(0, 1, 0);
  private readonly forward = new THREE.Vector3(0, 0, -1);
  private readonly normal = new THREE.Vector3(0, 0, 1);

  constructor() {
    this.root.name = 'practice-projectile-encounter';
    this.root.visible = false;
    const metal = new THREE.MeshStandardMaterial({
      color: 0x667985,
      metalness: 0.8,
      roughness: 0.32,
    });
    const dark = new THREE.MeshStandardMaterial({
      color: 0x182b36,
      metalness: 0.7,
      roughness: 0.45,
    });
    const ice = new THREE.MeshBasicMaterial({ color: 0xb3ecff });
    const orange = new THREE.MeshBasicMaterial({ color: 0xffa46f });
    const box = new THREE.BoxGeometry(1, 1, 1);
    const part = (
      group: THREE.Group,
      size: number[],
      at: number[],
      material: THREE.Material,
    ) => {
      const mesh = new THREE.Mesh(box, material);
      mesh.scale.set(size[0], size[1], size[2]);
      mesh.position.set(at[0], at[1], at[2]);
      group.add(mesh);
    };
    const body = new THREE.Mesh(new THREE.OctahedronGeometry(600, 1), metal);
    body.scale.set(0.7, 0.55, 1.4);
    this.drone.add(body);
    for (const x of [-1, 1]) {
      part(this.drone, [720, 90, 450], [x * 520, 0, 80], dark);
      part(this.drone, [140, 400, 850], [x * 860, 0, 80], metal);
      part(this.drone, [150, 150, 45], [x * 860, 0, -355], orange);
      part(this.drone, [110, 270, 300], [x * 400, 0, 580], dark);
      part(this.drone, [80, 230, 25], [x * 400, 0, 740], ice);
    }
    part(this.drone, [120, 130, 820], [0, 60, -650], dark);
    part(this.drone, [145, 150, 40], [0, 60, -1060], orange);
    this.root.add(this.drone, this.guns);
    const globe = new THREE.SphereGeometry(1, 32, 18);
    const additive = (opacity = 1) =>
      new THREE.MeshBasicMaterial({
        color: 0xffffff,
        transparent: true,
        opacity,
        depthWrite: false,
        blending: THREE.AdditiveBlending,
        side: THREE.DoubleSide,
      });
    for (const at of TURRET_PIVOTS) {
      part(this.guns, [700, 170, 780], [at[0], at[1] - 220, at[2]], metal);
      const pivot = new THREE.Group();
      pivot.position.fromArray(at);
      this.guns.add(pivot);
      const barrel = new THREE.Group();
      pivot.add(barrel);
      part(barrel, [380, 280, 600], [0, 0, -100], metal);
      part(barrel, [180, 150, 1000], [0, 0, -610], dark);
      part(barrel, [210, 180, 60], [0, 0, -1090], metal);
      part(barrel, [115, 90, 15], [0, 0, -1125], ice);
      for (const side of [-1, 1]) {
        part(barrel, [45, 80, 650], [side * 135, 95, -560], metal);
        for (let j = 0; j < 4; j++)
          part(barrel, [35, 60, 60], [side * 200, 40, 80 - j * 105], dark);
      }
      const flash = new THREE.Mesh(globe, additive());
      flash.position.set(0, 0, -BARREL_LENGTH);
      flash.visible = false;
      pivot.add(flash);
      this.mounts.push({ pivot, barrel, flash });
    }
    const shield = (color: number) => {
      const mesh = new THREE.Mesh(
        globe,
        new THREE.MeshBasicMaterial({
          color,
          transparent: true,
          opacity: 0.02,
          depthWrite: false,
          side: THREE.DoubleSide,
          blending: THREE.AdditiveBlending,
        }),
      );
      mesh.add(
        new THREE.Mesh(
          globe,
          new THREE.MeshBasicMaterial({
            color,
            wireframe: true,
            transparent: true,
            opacity: 0.025,
            depthWrite: false,
          }),
        ),
      );
      this.root.add(mesh);
      return mesh;
    };
    this.shipShield = shield(0x74d8f4);
    this.shipShield.scale.set(...SHIELD_SIZE);
    this.droneShield = shield(0xffa97a);
    this.droneShield.scale.setScalar(DRONE_RADIUS * 1.12);
    const cylinder = new THREE.CylinderGeometry(1, 1, 1, 6);
    const instance = (
      geometry: THREE.BufferGeometry,
      count: number,
      opacity = 1,
    ) => {
      const mesh = new THREE.InstancedMesh(geometry, additive(opacity), count);
      mesh.instanceMatrix.setUsage(THREE.DynamicDrawUsage);
      mesh.frustumCulled = false;
      mesh.count = 0;
      this.root.add(mesh);
      return mesh;
    };
    this.rounds = instance(cylinder, MAX_PROJECTILES);
    this.halos = instance(cylinder, MAX_PROJECTILES, 0.2);
    this.cores = instance(globe, MAX_IMPACTS, 0.8);
    this.rings = instance(
      new THREE.RingGeometry(0.88, 1, 40),
      MAX_IMPACTS,
      0.65,
    );
    this.sparks = instance(cylinder, MAX_IMPACTS * 4);
  }
  private put(mesh: THREE.InstancedMesh, index: number): void {
    this.transform.updateMatrix();
    mesh.setMatrixAt(index, this.transform.matrix);
    mesh.setColorAt(index, this.color);
  }
  private finish(mesh: THREE.InstancedMesh, count: number): void {
    mesh.count = count;
    mesh.instanceMatrix.needsUpdate = true;
    if (mesh.instanceColor) mesh.instanceColor.needsUpdate = true;
  }
  update(
    battle: PracticeBattle | null,
    origin: THREE.Vector3,
    rotation: THREE.Quaternion,
    visible: boolean,
    detail = 1,
  ): void {
    this.root.visible = !!battle && visible;
    if (!battle || !visible) return;
    this.drone.position.fromArray(battle.target).sub(origin);
    this.drone.quaternion.setFromUnitVectors(
      this.forward,
      this.direction.copy(this.drone.position).negate().normalize(),
    );
    this.drone.rotateZ(Math.sin(battle.elapsed * 0.4) * 0.12);
    this.drone.scale.setScalar(battle.status === 'victory' ? 0.85 : 1);
    this.guns.quaternion.copy(rotation);
    this.inverse.copy(rotation).invert();
    this.mounts.forEach((mount, i) => {
      this.direction
        .copy(this.drone.position)
        .applyQuaternion(this.inverse)
        .sub(mount.pivot.position)
        .normalize();
      mount.pivot.quaternion.setFromUnitVectors(this.forward, this.direction);
      const age = battle.muzzleAge[i],
        kick = Math.max(0, 1 - age / 0.2);
      mount.barrel.position.z = kick * 95;
      mount.flash.visible = age < 0.1;
      mount.flash.scale.set(160 * kick, 160 * kick, 330 * kick);
      (mount.flash.material as THREE.MeshBasicMaterial).color.set(
        battle.ammunition === 'standard' ? 0xc8f4ff : 0xffdfad,
      );
    });
    this.shipShield.quaternion.copy(rotation);
    this.droneShield.position.copy(this.drone.position);
    this.shipShield.visible = battle.player.shield > 0;
    this.droneShield.visible = battle.drone.shield > 0;
    this.shipShield.material.opacity = 0.012;
    this.droneShield.material.opacity = 0.024;
    let count = 0;
    for (const p of battle.projectiles) {
      if (!p.active) continue;
      const speed = Math.hypot(...p.velocity),
        length = Math.min(650, p.age * speed);
      if (length <= 0) continue;
      this.direction.fromArray(p.velocity).normalize();
      this.transform.position
        .fromArray(p.position)
        .sub(origin)
        .addScaledVector(this.direction, -length * 0.5);
      this.transform.quaternion.setFromUnitVectors(this.up, this.direction);
      this.transform.scale.set(28, length, 28);
      this.color.set(
        p.victim === 'ship'
          ? 0xff9068
          : p.ammo === 'piercing'
          ? 0xffd69d
          : 0xb8edff,
      );
      this.put(this.rounds, count);
      this.transform.scale.set(85, length, 85);
      this.put(this.halos, count);
      count++;
    }
    this.finish(this.rounds, count);
    this.finish(this.halos, count);
    let hits = 0,
      sparks = 0;
    for (const impact of battle.impacts) {
      if (!impact.active) continue;
      const fade = Math.max(0, 1 - impact.age / 0.85);
      const shield = impact.shieldDamage > 0,
        blocked = impact.blocked && impact.hullDamage === 0;
      this.color
        .set(shield ? 0x81dcff : blocked ? 0xffcc7d : 0xff714b)
        .multiplyScalar(fade);
      this.a.fromArray(impact.position).sub(origin);
      this.direction.fromArray(impact.direction).normalize();
      this.transform.position.copy(this.a);
      this.transform.quaternion.identity();
      this.transform.scale.setScalar(
        (shield ? 170 : blocked ? 100 : 230) * fade,
      );
      this.put(this.cores, hits);
      this.transform.quaternion.setFromUnitVectors(this.normal, this.direction);
      this.transform.scale.setScalar(
        (shield ? 220 : 110) + impact.age * (shield ? 1500 : 600),
      );
      this.put(this.rings, hits++);
      const sparkCount = detail === 0 ? 1 : detail === 2 ? 4 : 3;
      for (let j = 0; j < sparkCount; j++) {
        this.direction
          .set(
            Math.sin(j * 2.4 + hits),
            Math.cos(j * 1.8 + hits),
            Math.sin(j * 3.1 - hits),
          )
          .normalize();
        this.transform.position
          .copy(this.a)
          .addScaledVector(
            this.direction,
            impact.age * (blocked ? 1700 : 1100),
          );
        this.transform.quaternion.setFromUnitVectors(this.up, this.direction);
        this.transform.scale.set(15 * fade, 200 * fade, 15 * fade);
        this.put(this.sparks, sparks++);
      }
      if (shield) {
        const shell =
          impact.victim === 'ship' ? this.shipShield : this.droneShield;
        shell.visible = true;
        shell.material.opacity = Math.max(
          shell.material.opacity,
          fade * (impact.shieldBroken ? 0.2 : 0.1),
        );
      }
    }
    this.finish(this.cores, hits);
    this.finish(this.rings, hits);
    this.finish(this.sparks, sparks);
  }
}
