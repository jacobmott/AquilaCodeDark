import RAPIER from '@dimforge/rapier2d-compat';
import { AquilaSoundService } from '../services/AquilaSoundService';

export class AquilaEnemyProjectile {
  scene: Phaser.Scene;
  rapierWorld: RAPIER.World;
  graphics: Phaser.GameObjects.Container;
  flameEmitter: Phaser.GameObjects.Particles.ParticleEmitter;
  rigidBody: RAPIER.RigidBody;
  collider: RAPIER.Collider;
  colliderHandle: number;
  speed: number = 625;
  lifetime: number = 9000;
  elapsed: number = 0;
  alive: boolean = true;

  private static rocketTextureCreated = false;

  constructor(
    scene: Phaser.Scene,
    rapierWorld: RAPIER.World,
    x: number,
    y: number,
    directionX: number,
    directionY: number,
  ) {
    this.scene = scene;
    this.rapierWorld = rapierWorld;

    // Create blue rocket texture once
    if (!AquilaEnemyProjectile.rocketTextureCreated) {
      const gfx = scene.make.graphics({ x: 0, y: 0 }, false);
      gfx.fillStyle(0x2266cc, 1);
      gfx.fillTriangle(30, 10, 0, 0, 0, 20); // nose cone
      gfx.fillStyle(0x44aaff, 1);
      gfx.fillRect(0, 2, 20, 16); // body
      gfx.fillStyle(0x66ccff, 1);
      gfx.fillTriangle(0, 0, -6, -4, 0, 8); // top fin
      gfx.fillTriangle(0, 20, -6, 24, 0, 12); // bottom fin
      gfx.generateTexture('rocket_blue', 36, 24);
      gfx.destroy();
      AquilaEnemyProjectile.rocketTextureCreated = true;
    }

    // Rocket visual
    const angle = Math.atan2(directionY, directionX);
    const rocketImage = scene.add.image(0, 0, 'rocket_blue').setOrigin(0.5);
    const container = scene.add.container(x, y, [rocketImage]);
    container.setDepth(200);
    container.setRotation(angle);
    this.graphics = container;

    // Flame trail emitter
    this.flameEmitter = scene.add.particles(x, y, 'explosion_particle', {
      speed: { min: 80, max: 200 },
      scale: { start: 2, end: 0 },
      alpha: { start: 0.7, end: 0 },
      lifespan: { min: 500, max: 1500 },
      frequency: 8,
      tint: [0x4488ff, 0x66ccff, 0xaaddff, 0xffffff],
      angle: { min: (angle * 180 / Math.PI) + 150, max: (angle * 180 / Math.PI) + 210 },
      emitting: true,
    });
    this.flameEmitter.setDepth(199);

    const bodyDesc = RAPIER.RigidBodyDesc.dynamic()
      .setTranslation(x, y)
      .setLinvel(directionX * this.speed, directionY * this.speed)
      .setCcdEnabled(true)
      .setGravityScale(0);

    this.rigidBody = rapierWorld.createRigidBody(bodyDesc);

    const colliderDesc = RAPIER.ColliderDesc.ball(15)
      .setActiveEvents(RAPIER.ActiveEvents.COLLISION_EVENTS)
      .setCollisionGroups(0x00080001)
      .setRestitution(0);
    this.collider = rapierWorld.createCollider(colliderDesc, this.rigidBody);
    this.colliderHandle = this.collider.handle;

    AquilaSoundService.playEnemyFireSound();
  }

  update(delta: number) {
    if (!this.alive) return;

    this.elapsed += delta;
    if (this.elapsed >= this.lifetime) {
      this.destroy();
      return;
    }

    const pos = this.rigidBody.translation();
    this.graphics.x = pos.x;
    this.graphics.y = pos.y;
    this.flameEmitter.setPosition(pos.x, pos.y);
  }

  explode() {
    if (!this.alive) return;
    AquilaSoundService.playProjectileExplodeSound();
    const pos = this.rigidBody.translation();

    const emitter = this.scene.add.particles(pos.x, pos.y, 'explosion_particle', {
      speed: { min: 200, max: 800 },
      scale: { start: 2.5, end: 0 },
      alpha: { start: 1, end: 0 },
      lifespan: { min: 200, max: 500 },
      quantity: 20,
      tint: [0x4488ff, 0x44aaff, 0xffffff],
      emitting: false,
      maxParticles: 20,
    });
    emitter.setDepth(200);
    emitter.explode();

    this.scene.time.delayedCall(600, () => {
      emitter.destroy();
    });

    this.destroy();
  }

  destroy() {
    if (!this.alive) return;
    this.alive = false;
    this.flameEmitter.stop();
    this.scene.time.delayedCall(400, () => {
      this.flameEmitter.destroy();
    });
    this.graphics.destroy();
    this.rapierWorld.removeRigidBody(this.rigidBody);
  }
}
