import { AquilaInputService } from '../services/AquilaInputService';
import { UserComponent } from './UserComponent';
import RAPIER from '@dimforge/rapier2d-compat';

export class AquilaObstacle extends UserComponent {
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
    this.setupLargeWallPhysics();
    const cam2 = this.scene.cameras.add(0, 0, 500, 500, false, 'cam2');
    cam2.setZoom(0.1);
    cam2.startFollow(this.image, true, 1, 1);
  }

  setupLargeWallPhysics() {
    const data = this.scene.cache.json.get('largewall-v1-plaintext_convex_sub');

    const sprites: any[] = [];
    data.sprites.forEach((sprite: any) => {
      const spriteNew: any = {};
      spriteNew.name = sprite.name;
      const shapes: Float32Array[] = [];
      //put this back if we can ever do compound shapes
      //https://github.com/dimforge/rapier.js/issues/44
      const fullShape: number[] = [];
      sprite.convexSubPolygons.forEach((convexSubPolygon: any) => {
        fullShape.push(...convexSubPolygon);
        shapes.push(new Float32Array(convexSubPolygon));
      });
      const shapesNew: Float32Array = new Float32Array(fullShape);
      spriteNew.convexSubPolygons = shapes;
      spriteNew.fullHull = shapesNew;
      sprites.push(spriteNew);
    });

    const bodyDesc = RAPIER.RigidBodyDesc.fixed();

    // Set its initial position to match our Phaser Game Object
    bodyDesc.setTranslation(this.image.x, this.image.y);

    // Store the Phaser Game Object in the rigid body's user data so we can sync its position and rotation
    bodyDesc.setUserData(this.image);

    // Finally, create the rigid body in the Rapier world from the body description
    const rigidBody = this.rapierWorld.createRigidBody(bodyDesc);

    sprites.forEach((sprite) => {
      const name = sprite.name;
      const colliderDesc = RAPIER.ColliderDesc.convexHull(sprite.fullHull); // Use a circle with radius 0.5
      const collider = this.rapierWorld.createCollider(
        colliderDesc!,
        rigidBody,
      );
      collider.setActiveEvents(RAPIER.ActiveEvents.COLLISION_EVENTS);
    });
  }

  // override update(time: number, delta: number) {}

  // override updateManually(time: number, delta: number) {}

  // override destroy() {}
}
