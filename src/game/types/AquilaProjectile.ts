import RAPIER from '@dimforge/rapier2d-compat';

export class AquilaProjectile {
  scene: Phaser.Scene;
  rapierWorld: RAPIER.World;
  graphics: Phaser.GameObjects.Arc;
  rigidBody: RAPIER.RigidBody;
  collider: RAPIER.Collider;
  speed: number = 4000;
  lifetime: number = 3000; // ms before auto-destroy
  elapsed: number = 0;
  alive: boolean = true;

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

    // Visual circle
    this.graphics = scene.add.circle(x, y, 20, 0xff4444);
    this.graphics.setDepth(200);

    // Rapier dynamic rigid body
    const bodyDesc = RAPIER.RigidBodyDesc.dynamic()
      .setTranslation(x, y)
      .setLinvel(directionX * this.speed, directionY * this.speed)
      .setCcdEnabled(true)
      .setGravityScale(0);

    this.rigidBody = rapierWorld.createRigidBody(bodyDesc);

    // Ball collider — collision group 0x0002 interacts with 0x0004 (environment/enemies), NOT 0x0001 (player)
    const colliderDesc = RAPIER.ColliderDesc.ball(20)
      .setActiveEvents(RAPIER.ActiveEvents.COLLISION_EVENTS)
      .setCollisionGroups(0x00020004)
      .setRestitution(0);
    this.collider = rapierWorld.createCollider(colliderDesc, this.rigidBody);
  }

  update(delta: number) {
    if (!this.alive) return;

    this.elapsed += delta;
    if (this.elapsed >= this.lifetime) {
      this.destroy();
      return;
    }

    // Sync visual to physics position
    const pos = this.rigidBody.translation();
    this.graphics.x = pos.x;
    this.graphics.y = pos.y;
  }

  destroy() {
    if (!this.alive) return;
    this.alive = false;
    this.graphics.destroy();
    this.rapierWorld.removeRigidBody(this.rigidBody);
  }
}
