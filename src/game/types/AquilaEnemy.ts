import { AquilaInputService } from "../services/AquilaInputService";
import { AquilaSoundService } from "../services/AquilaSoundService";
import { UserComponent } from "./UserComponent";
import RAPIER from "@dimforge/rapier2d-compat";
import { AquilaEnemyProjectile } from "./AquilaEnemyProjectile";
import * as spine from '@esotericsoftware/spine-phaser';

export class AquilaEnemy extends UserComponent {
  spineObject: spine.SpineGameObject;
  enemyBody!: RAPIER.RigidBody;
  enemyCollider!: RAPIER.Collider;
  colliderHandle: number = -1;
  alive: boolean = true;

  // AI properties
  detectionRange: number = 4000;
  moveSpeed: number = 300;
  shootCooldown: number = 1500;
  canShoot: boolean = true;
  projectiles: AquilaEnemyProjectile[] = [];
  aggroActive: boolean = false;
  colliderRadius: number = 512;
  lastPosition: { x: number; y: number } = { x: 0, y: 0 };
  private aura!: Phaser.GameObjects.Graphics;
  private exhaustEmitterL!: Phaser.GameObjects.Particles.ParticleEmitter;
  private exhaustEmitterR!: Phaser.GameObjects.Particles.ParticleEmitter;
  private engineSound: { setIntensity: (t: number) => void; stop: () => void } | null = null;

  private static explosionTextureCreated = false;

  constructor(
    gameObject: Phaser.GameObjects.GameObject,
    inputService: AquilaInputService,
    rapierWorld: RAPIER.World,
  ) {
    super(gameObject, inputService, rapierWorld);
    this.spineObject = gameObject as spine.SpineGameObject;
    this.spineObject.setDepth(100);
  }

  // override awake() {}

  override start() {
    this.spineObject.skeleton.setSkinByName('defaultskin');
    this.spineObject.setScale(1);
    this.spineObject.animationState.setAnimation(0, '0', false);
    this.setupEnemyPhysics();

    // Red aura around enemy
    this.aura = this.scene.add.graphics();
    this.aura.setDepth(99);
    this.drawEnemyAura();
    this.scene.tweens.add({
      targets: this.aura,
      alpha: { from: 1, to: 0.5 },
      duration: 1000 + Math.random() * 400,
      yoyo: true,
      repeat: -1,
      ease: 'Sine.easeInOut',
    });

    // Engine exhaust emitters (left and right engines)
    const exhaustConfig = {
      speed: { min: 60, max: 150 },
      scale: { start: 2, end: 0 },
      alpha: { start: 0.7, end: 0 },
      lifespan: { min: 150, max: 300 },
      frequency: 50,
      tint: [0xff4400, 0xff6600, 0xff8800, 0xffaa00],
      emitting: true,
    };
    this.exhaustEmitterL = this.scene.add.particles(0, 0, 'explosion_particle', { ...exhaustConfig });
    this.exhaustEmitterL.setDepth(98);
    this.exhaustEmitterR = this.scene.add.particles(0, 0, 'explosion_particle', { ...exhaustConfig });
    this.exhaustEmitterR.setDepth(98);

    // Engine sound
    this.engineSound = AquilaSoundService.createEngineSound(65, 0.06);
  }

  private drawEnemyAura() {
    this.aura.clear();
    const radius = 600;
    const steps = 6;
    for (let i = steps; i >= 1; i--) {
      const r = radius * (i / steps);
      const alpha = 0.25 * (1 - (i - 1) / steps);
      this.aura.fillStyle(0xff3333, alpha);
      this.aura.fillCircle(0, 0, r);
    }
  }

  setupEnemyPhysics() {
    const circleColliderDesc = RAPIER.ColliderDesc.ball(this.colliderRadius);
    const circleRigidBodyDesc = RAPIER.RigidBodyDesc.dynamic().setTranslation(
      this.spineObject.x,
      this.spineObject.y,
    );
    circleRigidBodyDesc.setUserData(this.spineObject);

    this.enemyBody = this.rapierWorld.createRigidBody(circleRigidBodyDesc);
    this.enemyBody.lockRotations(true, true);

    // Create the collider.
    this.enemyCollider = this.rapierWorld.createCollider(
      circleColliderDesc,
      this.enemyBody,
    );
    this.enemyCollider.setRestitution(1);
    this.enemyCollider.setActiveEvents(RAPIER.ActiveEvents.COLLISION_EVENTS);
    // Collision group 0x0004 interacts with player (0x0001) and projectiles (0x0002)
    this.enemyCollider.setCollisionGroups(0x00040003);
    // Prevent enemies from pushing each other via the physics solver
    this.enemyCollider.setSolverGroups(0x00040003);
    this.colliderHandle = this.enemyCollider.handle;

    // Create explosion texture once
    if (!AquilaEnemy.explosionTextureCreated) {
      if (!this.scene.textures.exists('explosion_particle')) {
        const gfx = this.scene.make.graphics({ x: 0, y: 0 }, false);
        gfx.fillStyle(0xffffff, 1);
        gfx.fillCircle(8, 8, 8);
        gfx.generateTexture('explosion_particle', 16, 16);
        gfx.destroy();
      }
      AquilaEnemy.explosionTextureCreated = true;
    }
  }

  explode() {
    if (!this.alive) return;
    this.alive = false;
    AquilaSoundService.playShipExplodeSound();

    // Destroy aura
    if (this.aura) {
      this.scene.tweens.killTweensOf(this.aura);
      this.aura.destroy();
    }

    // Destroy exhaust
    if (this.exhaustEmitterL) {
      this.exhaustEmitterL.stop();
      this.exhaustEmitterR.stop();
      const emL = this.exhaustEmitterL;
      const emR = this.exhaustEmitterR;
      (this as any).exhaustEmitterL = null;
      (this as any).exhaustEmitterR = null;
      this.scene.time.delayedCall(500, () => {
        emL.destroy();
        emR.destroy();
      });
    }

    // Stop engine sound
    this.engineSound?.stop();
    this.engineSound = null;

    // Destroy all enemy projectiles
    for (const p of this.projectiles) {
      p.destroy();
    }
    this.projectiles = [];

    const pos = this.enemyBody.translation();
    this.lastPosition = { x: pos.x, y: pos.y };

    // Remove physics body (but keep spine visual for wreckage)
    this.rapierWorld.removeRigidBody(this.enemyBody);

    // --- Shake the ship ---
    const origX = this.spineObject.x;
    const origY = this.spineObject.y;
    this.scene.tweens.add({
      targets: this.spineObject,
      x: { from: origX - 15, to: origX + 15 },
      y: { from: origY - 10, to: origY + 10 },
      duration: 50,
      yoyo: true,
      repeat: 8,
      onComplete: () => {
        this.spineObject.setPosition(origX, origY);
      },
    });

    // --- Initial explosion burst (after shake) ---
    this.scene.time.delayedCall(450, () => {
      // Inner bright flash
      const flashEmitter = this.scene.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 50, max: 300 },
        scale: { start: 12, end: 0 },
        alpha: { start: 1, end: 0 },
        lifespan: { min: 150, max: 350 },
        quantity: 15,
        tint: [0xffffff, 0xffffaa, 0xffff44],
        emitting: false,
        maxParticles: 15,
      });
      flashEmitter.setDepth(202);
      flashEmitter.explode();

      // Main fireball
      const burstEmitter = this.scene.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 200, max: 800 },
        scale: { start: 8, end: 1 },
        alpha: { start: 1, end: 0 },
        lifespan: { min: 400, max: 900 },
        quantity: 35,
        tint: [0xff4400, 0xff6600, 0xff8800, 0xffaa00],
        emitting: false,
        maxParticles: 35,
      });
      burstEmitter.setDepth(201);
      burstEmitter.explode();

      this.scene.time.delayedCall(1500, () => {
        flashEmitter.destroy();
        burstEmitter.destroy();
      });

      // --- Burning wreckage: continuous fire + smoke on the hull ---
      // Tint the wreck dark
      this.spineObject.skeleton.color.r = 0.33;
      this.spineObject.skeleton.color.g = 0.13;
      this.spineObject.skeleton.color.b = 0;

      const fireEmitter = this.scene.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 30, max: 120 },
        scale: { start: 5, end: 0 },
        alpha: { start: 0.8, end: 0 },
        lifespan: { min: 400, max: 800 },
        frequency: 60,
        tint: [0xff4400, 0xff6600, 0xff2200, 0xffaa00],
        emitZone: { type: 'random', source: new Phaser.Geom.Circle(0, 0, 200) } as any,
      });
      fireEmitter.setDepth(203);

      const smokeEmitter = this.scene.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 10, max: 50 },
        scale: { start: 4, end: 8 },
        alpha: { start: 0.3, end: 0 },
        lifespan: { min: 800, max: 1500 },
        frequency: 120,
        tint: [0x333333, 0x444444, 0x222222],
        emitZone: { type: 'random', source: new Phaser.Geom.Circle(0, 0, 150) } as any,
      });
      smokeEmitter.setDepth(200);

      // Fade out and remove wreckage after 15 seconds
      this.scene.tweens.add({
        targets: this.spineObject,
        alpha: 0,
        duration: 3000,
        delay: 12000,
        onComplete: () => {
          this.spineObject.destroy();
          fireEmitter.destroy();
          smokeEmitter.destroy();
        },
      });

      // Also fade out fire/smoke particles near the end
      this.scene.time.delayedCall(12000, () => {
        fireEmitter.stop();
        smokeEmitter.stop();
      });
    });
  }

  updateAI(playerX: number, playerY: number, delta: number) {
    if (!this.alive || !this.enemyBody) return;

    const pos = this.enemyBody.translation();
    const dx = playerX - pos.x;
    const dy = playerY - pos.y;
    const dist = Math.sqrt(dx * dx + dy * dy);

    // Sync aura position
    this.aura?.setPosition(pos.x, pos.y);

    // Update projectiles
    this.projectiles.forEach((p) => p.update(delta));
    this.projectiles = this.projectiles.filter((p) => p.alive);

    if (dist > this.detectionRange) {
      this.aggroActive = false;
      // Idle exhaust
      for (const em of [this.exhaustEmitterL, this.exhaustEmitterR]) {
        if (!em) continue;
        em.setParticleSpeed(450, 1200);
        em.setParticleScale(9, 0);
        em.setFrequency(5);
        em.setParticleLifespan(1200);
      }
      this.engineSound?.setIntensity(0);
      return;
    }

    this.aggroActive = true;
    const dirX = dx / dist;
    const dirY = dy / dist;

    // Rotate to face the player using Spine animation frames (0-359 degrees)
    const angleDeg = Math.atan2(dy, dx) * 180 / Math.PI;
    const frame = Math.trunc(((360 - angleDeg) % 360 + 360) % 360);
    this.spineObject.animationState.setAnimation(0, frame.toString(), false);

    // Update exhaust position — tail is opposite of facing direction
    const tailOffset = 400;
    const engineSpread = 110;
    const perpX = -dirY;
    const perpY = dirX;
    const tailCenterX = pos.x - dirX * tailOffset;
    const tailCenterY = pos.y - dirY * tailOffset;
    this.exhaustEmitterL?.setPosition(tailCenterX + perpX * engineSpread, tailCenterY + perpY * engineSpread);
    this.exhaustEmitterR?.setPosition(tailCenterX - perpX * engineSpread, tailCenterY - perpY * engineSpread);
    // Moving exhaust — 3x larger
    for (const em of [this.exhaustEmitterL, this.exhaustEmitterR]) {
      if (!em) continue;
      em.setParticleSpeed(1350, 3600);
      em.setParticleScale(27, 0);
      em.setFrequency(2);
      em.setParticleLifespan(3600);
    }
    this.engineSound?.setIntensity(1);

    // Move toward player (apply velocity)
    this.enemyBody.setLinvel(
      { x: dirX * this.moveSpeed, y: dirY * this.moveSpeed },
      true,
    );

    // Shoot at player
    if (this.canShoot && dist < this.detectionRange) {
      this.canShoot = false;
      this.scene.time.delayedCall(this.shootCooldown, () => {
        this.canShoot = true;
      });

      const spawnOffset = this.colliderRadius + 40;
      const spawnX = pos.x + dirX * spawnOffset;
      const spawnY = pos.y + dirY * spawnOffset;

      const projectile = new AquilaEnemyProjectile(
        this.scene,
        this.rapierWorld,
        spawnX,
        spawnY,
        dirX,
        dirY,
      );
      this.projectiles.push(projectile);
    }
  }

  getProjectileByColliderHandle(handle: number): AquilaEnemyProjectile | undefined {
    return this.projectiles.find((p) => p.alive && p.colliderHandle === handle);
  }

  stopEngineSound() {
    this.engineSound?.stop();
    this.engineSound = null;
  }
}
