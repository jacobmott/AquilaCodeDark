import { AquilaInputService } from "../services/AquilaInputService";
import { UserComponent } from "./UserComponent";
import RAPIER from "@dimforge/rapier2d-compat";

export class AquilaEnemy extends UserComponent {
  image: Phaser.GameObjects.Image;
  constructor(
    gameObject: Phaser.GameObjects.GameObject,
    inputService: AquilaInputService,
    rapierWorld: RAPIER.World,
  ) {
    super(gameObject, inputService, rapierWorld);
    this.image = gameObject as Phaser.GameObjects.Image;
    this.image.setDepth(100);
    this.scene.add.existing(gameObject);
  }

  // override awake() {}

  override start() {
    this.setupEnemyPhysics();
  }

  setupEnemyPhysics() {
    const circleColliderDesc = RAPIER.ColliderDesc.ball(this.image.width / 2);
    const circleRigidBodyDesc = RAPIER.RigidBodyDesc.dynamic().setTranslation(
      this.image.x,
      this.image.y,
    );
    circleRigidBodyDesc.setUserData(this.image);

    const enemyBody = this.rapierWorld.createRigidBody(circleRigidBodyDesc);

    // Create the collider.
    const enemyCollider = this.rapierWorld.createCollider(
      circleColliderDesc,
      enemyBody,
    );
    enemyCollider.setRestitution(1);
  }

  // override update(time: number, delta: number) {}

  // override updateManually(time: number, delta: number) {}

  // override destroy() {}
}
