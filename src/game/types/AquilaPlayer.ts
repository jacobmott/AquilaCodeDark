import { AquilaInputService } from '../services/AquilaInputService';
import { UserComponent } from './UserComponent';
import * as spine from '@esotericsoftware/spine-phaser';
import RAPIER from '@dimforge/rapier2d-compat';
import { SinCosTable } from '../sincostabl';
import { EventBus } from '../EventBus';

type CastShape = {
  shape: Phaser.GameObjects.Polygon;
  maxToi: number;
  direction: RAPIER.Vector;
};

type ColliderMap = {
  [key: string]: RAPIER.Collider;
};

export class AquilaPlayer extends UserComponent {
  spineObject: spine.SpineGameObject;
  spineTrackEntry!: spine.TrackEntry;
  playerRigidBody!: RAPIER.RigidBody;
  playerColliders: Map<string, RAPIER.Collider>;
  playerCollider!: RAPIER.Collider;
  graphics!: Phaser.GameObjects.Graphics;
  sinCosTable!: SinCosTable;
  playerDirection!: Phaser.Math.Vector2;
  speed: number = 1000;
  rotationSpeed: number = 500;
  ready: boolean = false;
  currentRotation: number = 0;
  characterController!: RAPIER.KinematicCharacterController;
  castShapes: CastShape[] = [];
  lastHits!: Map<string, RAPIER.ColliderShapeCastHit>;

  wDown: boolean = false;
  aDown: boolean = false;
  sDown: boolean = false;
  dDown: boolean = false;
  eDown: boolean = false;
  rDown: boolean = false;
  cDown: boolean = false;
  vDown: boolean = false;
  fDown: boolean = false;

  canDoFAction: boolean = true;

  rotationLock: boolean = false;

  constructor(
    gameObject: Phaser.GameObjects.GameObject,
    inputService: AquilaInputService,
    rapierWorld: RAPIER.World,
  ) {
    super(gameObject, inputService, rapierWorld);
    this.rapierWorld = rapierWorld;
    this.spineObject = gameObject as spine.SpineGameObject;
    this.aquilaInputService.getKeys().subscribe((key) => {
      if (key.key === 'w') {
        this.wDown = key.isDown;
      }
      if (key.key === 'a') {
        this.aDown = key.isDown;
      }
      if (key.key === 's') {
        this.sDown = key.isDown;
      }
      if (key.key === 'd') {
        this.dDown = key.isDown;
      }
      if (key.key === 'e') {
        this.eDown = key.isDown;
      }
      if (key.key === 'r') {
        this.rDown = key.isDown;
      }
      if (key.key === 'c') {
        this.cDown = key.isDown;
      }
      if (key.key === 'v') {
        this.vDown = key.isDown;
      }
      if (key.key === 'f') {
        this.fDown = key.isDown;
      }
    });
    this.playerColliders = new Map<string, RAPIER.Collider>();
    this.lastHits = new Map<string, RAPIER.ColliderShapeCastHit>();
  }

  // override awake() {}

  override start() {
    console.log('AquilaPlayer start called');
    // Create a SpineGameObject through the GameObjectFactory and add it to the scene
    // this.spineObject.animationState.addListener({
    //   start: (entry) => {
    //     console.log("Started animation");
    //     console.dir(entry);
    //   },
    //   interrupt: (entry) => {
    //     console.log(`Interrupted animation`);
    //     console.dir(entry);
    //   },
    //   end: (entry) => {
    //     console.log(`Ended animation`);
    //     console.dir(entry);
    //   },
    //   dispose: (entry) => {
    //     console.log("Disposed animation");
    //     console.dir(entry);
    //   },
    //   complete: (entry) => {
    //     console.log("Completed animation");
    //     console.dir(entry);
    //   },
    // });

    this.spineObject.setDepth(101);

    this.spineObject.skeleton.setSkinByName('defaultskin');
    this.spineObject.setScale(1);
    this.spineObject.setInteractive();

    this.computeSinCosTables();

    // Create a Vector2 to represent the direction (moving right, along the positive x axis
    this.playerDirection = new Phaser.Math.Vector2(1, 0);
    this.playerDirection = this.playerDirection.normalize();

    this.setupPhysics();
    this.setupCharacterController();

    this.graphics = this.scene.add.graphics();
    this.spineObject.animationState.setAnimation(0, '0', false);
  }

  setupCharacterController() {
    this.characterController = this.rapierWorld.createCharacterController(0.01);
    this.characterController.setApplyImpulsesToDynamicBodies(true);
  }

  computeSinCosTables() {
    this.sinCosTable = new SinCosTable(361);
  }

  setupPhysics() {
    // Create a Rapier dynamic rigid body
    const bodyDesc = RAPIER.RigidBodyDesc.kinematicPositionBased()
      .setCcdEnabled(true)
      .setCanSleep(false);
    // Set its initial position to match our Phaser Game Object
    bodyDesc.setTranslation(this.spineObject.x, this.spineObject.y);

    // Store the Phaser Game Object in the rigid body's user data so we can sync its position and rotation
    bodyDesc.setUserData(this.spineObject);

    // Finally, create the rigid body in the Rapier world from the body description
    this.playerRigidBody = this.rapierWorld.createRigidBody(bodyDesc);
    this.playerRigidBody.enableCcd(true);
    this.setupPlayerPhysics();
  }

  setupPlayerPhysics() {
    const data = this.scene.cache.json.get('ship0-360-v1-plaintext_convex_sub');

    const sprites: any[] = [];
    data.sprites.forEach((sprite: any) => {
      const spriteNew: any = {};
      spriteNew.name = this.trimLeadingZeros(sprite.name);
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

    sprites.forEach((sprite) => {
      const name: string = sprite.name;
      // if (name !== "0") {
      //   return;
      // }

      const playerColliderDesc = RAPIER.ColliderDesc.convexHull(
        sprite.fullHull,
      ) as RAPIER.ColliderDesc; // Use a circle with radius 0.5
      const playerCollider = this.rapierWorld.createCollider(
        playerColliderDesc,
        this.playerRigidBody,
      );

      playerCollider.setActiveEvents(RAPIER.ActiveEvents.COLLISION_EVENTS);
      playerCollider.setEnabled(false);
      this.playerColliders.set(name, playerCollider);
      // this.rapierWorld.contactPair(
      //   this.playerColliders[name],
      //   this.enemyCollider,
      //   (manifold, flipped) => {
      //     // Contact information can be read from `manifold`.
      //     console.log("contact");
      //   },
      // );
      // this.rapierWorld.contactPairsWith(
      //   this.playerColliders[name],
      //   (otherCollider) => {
      //     console.log("contact pair: " + name);
      //     console.dir(otherCollider);
      //   },
      // );
    });

    this.playerCollider = this.playerColliders.get('0') as RAPIER.Collider;
    this.playerCollider.setEnabled(true);

    this.setupCamera();
  }

  setupCamera() {
    this.scene.cameras.main.startFollow(this.spineObject);
  }

  trimLeadingZeros(str: string): string {
    if (str.match(/^0+$/)) {
      return str.replace(/^0+/, '0');
    }
    return str.replace(/^0+/, '');
  }

  override updateManually(time: number, delta: number) {
    if (!this.characterController) {
      return;
    }

    // this.castShapes.forEach((shape) => {
    //   shape.shape.x += shape.direction.x * 4000 * (delta / 1000);
    //   shape.shape.y += shape.direction.y * 4000 * (delta / 1000);
    // });
    this.lastHits?.forEach((hit) => {
      this.graphics.lineStyle(10, 0xff0000, 1);
      this.graphics.fillStyle(0xff0000, 1);
      this.graphics.fillPoint(hit.witness1.x, hit.witness1.y, 100);
    });
    let moved = false;
    let rotated = false;
    const desiredTranslation = { x: 0, y: 0 };

    this.characterController.setSlideEnabled(true);

    if (this.sDown || this.wDown) {
      this.scene.cameras.main.startFollow(this.spineObject);
      moved = true;
    }
    if (this.aDown) {
      rotated = true;
      this.checkIfCanRotate(delta, 'left');
    } else if (this.dDown) {
      rotated = true;
      this.checkIfCanRotate(delta, 'right');
    }

    if (!this.rotationLock) {
      if (this.aDown) {
        this.currentRotation += this.rotationSpeed * (delta / 1000);
      } else if (this.dDown) {
        this.currentRotation -= this.rotationSpeed * (delta / 1000);
      }
    }

    if (rotated && !this.rotationLock) {
      const result = this.currentRotation;
      if (result >= 360) {
        this.currentRotation = result - 360;
      } else if (result <= 0) {
        this.currentRotation = 360 + result;
      }
      const rotationTrunc = Math.trunc(this.currentRotation);

      const animation = rotationTrunc.toString();
      this.spineObject.animationState.setAnimation(0, animation, false);
      // console.log("currentRotation: " + this.currentRotation);
      // console.log("rotationTrunc: " + rotationTrunc);

      // const desiredTranslation1 = { x: 0, y: 0 };
      // const direction1 = new Phaser.Math.Vector2(
      //   this.sinCosTable.getCos(rotationTrunc),
      //   this.sinCosTable.getSin(rotationTrunc),
      // );

      // desiredTranslation1.x -= 100 * direction1.x;
      // desiredTranslation1.y -= 100 * direction1.y;
      // this.movePlayer(desiredTranslation1);

      // const desiredTranslation2 = { x: 0, y: 0 };
      // desiredTranslation2.x += 100 * direction1.x;
      // desiredTranslation2.y += 100 * direction1.y;
      // this.movePlayer(desiredTranslation2);

      this.playerCollider.setEnabled(false);
      this.playerCollider = this.playerColliders.get(
        animation,
      ) as RAPIER.Collider;
      this.playerCollider.setEnabled(true);

      // const direction = new Phaser.Math.Vector2(
      //   this.sinCosTable.getCos(rotationTrunc),
      //   this.sinCosTable.getSin(rotationTrunc),
      // );

      // desiredTranslation.x -= 100 * direction.x;
      // desiredTranslation.y -= 100 * direction.y;
      // this.movePlayer(desiredTranslation);
    }

    if (this.sDown) {
      const rotationTrunc = Math.trunc(this.currentRotation);
      // console.log("S down");

      const direction = new Phaser.Math.Vector2(
        this.sinCosTable.getCos(rotationTrunc),
        this.sinCosTable.getSin(rotationTrunc),
      );
      desiredTranslation.x -= this.speed * (delta / 1000) * direction.x;
      desiredTranslation.y -= this.speed * (delta / 1000) * direction.y;

      EventBus.emit('add-scrolled-data', this, {
        identifier: 'console log info',
        data: 'S key pressed',
      });
    }
    if (this.wDown) {
      //Get the direction we are facing based on our rotation, then subtract so we go forwards
      const rotationTrunc = Math.trunc(this.currentRotation);
      // console.log("W down");

      const direction = new Phaser.Math.Vector2(
        this.sinCosTable.getCos(rotationTrunc),
        this.sinCosTable.getSin(rotationTrunc),
      );

      desiredTranslation.x += this.speed * (delta / 1000) * direction.x;
      desiredTranslation.y += this.speed * (delta / 1000) * direction.y;

      EventBus.emit('add-scrolled-data', this, {
        identifier: 'console log info',
        data: 'W key pressed',
      });
    }
    if (this.eDown) {
      this.speed += 100;
      if (this.speed >= 10000) {
        this.speed = 10000;
      }
      // console.log("speed: " + this.speed);
    } else if (this.rDown) {
      this.speed -= 100;
      if (this.speed <= 0) {
        this.speed = 100;
      }
      // console.log("speed: " + this.speed);
    }

    if (this.cDown) {
      this.rotationSpeed += 2;
      if (this.rotationSpeed >= 1000) {
        this.rotationSpeed = 1000;
      }
      // console.log("rotationSpeed: " + this.rotationSpeed);
    } else if (this.vDown) {
      this.rotationSpeed -= 2;
      if (this.rotationSpeed <= 0) {
        this.rotationSpeed = 2;
      }
      // console.log("rotationSpeed: " + this.rotationSpeed);
    }

    // if (this.fDown) {
    //   if (this.canDoFAction) {
    //     setTimeout(() => {
    //       this.canDoFAction = true;
    //     }, 1000);
    //     this.canDoFAction = false;
    //     this.checkIfCanRotate(delta);
    //   }
    // }

    this.movePlayer(desiredTranslation, moved);
    this.updateAngularDebugPanel();
    this.rotationLock = false;
  }

  // override update(time: number, delta: number) {}

  movePlayer(desiredTranslation: any, moved: boolean) {
    if (!moved) {
      return;
    }
    // Compute the player's collider movement considering obstacles
    this.characterController.computeColliderMovement(
      this.playerCollider,
      desiredTranslation,
    );
    const correctedMovement = this.characterController.computedMovement();
    // Compute the player's collider movement considering obstacles
    this.playerRigidBody.setNextKinematicTranslation({
      x: this.playerRigidBody.translation().x + correctedMovement.x,
      y: this.playerRigidBody.translation().y + correctedMovement.y,
    });
  }

  checkIfCanRotate(delta: number, direction: string) {
    const collider: RAPIER.Collider = this.playerCollider;
    const shape: RAPIER.ConvexPolygon = collider.shape as RAPIER.ConvexPolygon;
    console.log('Shape:');
    console.dir(shape);

    const scaleFactor = 1.1;
    const scaledVertices = shape.vertices.map(
      (vertex: number, index: number) => {
        vertex = vertex * scaleFactor;
        return vertex;
      },
    );
    const scaledShape: RAPIER.ConvexPolygon = new RAPIER.ConvexPolygon(
      scaledVertices,
      false,
    );

    let shapeVertices: string = scaledVertices.toString();
    shapeVertices = shapeVertices.replace(/,/g, ' ');
    console.log('Vertices:' + shapeVertices);
    console.log(
      'Collider Position:' +
        this.playerCollider.translation().x +
        ' ' +
        this.playerCollider.translation().y,
    );
    const polygon: Phaser.GameObjects.Polygon = new Phaser.GameObjects.Polygon(
      this.scene,
      this.playerCollider.translation().x,
      this.playerCollider.translation().y,
      shapeVertices,
      0x0000ff,
      0.5,
    );

    // polygon.setScale(1.1, 1.1);
    // const newShape: RAPIER.ConvexPolygon = new RAPIER.ConvexPolygon(
    //   polygon.geom.points,
    //   true,
    // );
    polygon.alpha = 1;
    polygon.setOrigin(0, 0);
    // Create two example shapes: one dynamic and one static
    const dynamicShape = new RAPIER.Cuboid(50, 50);
    // const staticShape = new RAPIER.Cuboid(1.0, 1.0);
    // Position and rotation of the dynamic shape (shape being cast)
    const dynamicPosition = new RAPIER.Vector2(
      this.playerRigidBody.translation().x,
      this.playerRigidBody.translation().y,
    );
    const dynamicRotation = RAPIER.RotationOps.identity(); // No rotation

    // Shape cast vector (i.e., the movement vector along which you want to check for collisions)
    // const castDirection = new RAPIER.Vector2(0, -1); // Move downward

    let rotation = this.currentRotation;
    if (this.aDown) {
      rotation += this.rotationSpeed * (delta / 1000);
    } else if (this.dDown) {
      rotation -= this.rotationSpeed * (delta / 1000);
    }

    let rotationTruncLeft = Math.trunc(rotation);
    let rotationTruncRight = rotationTruncLeft;
    // rotationTrunc = rotationTrunc + 90;

    rotationTruncLeft = rotationTruncLeft + 90;
    rotationTruncRight = rotationTruncRight - 90;

    if (rotationTruncLeft >= 360) {
      rotationTruncLeft = rotationTruncLeft - 360;
    } else if (rotationTruncLeft <= 0) {
      rotationTruncLeft = 360 + rotationTruncLeft;
    }

    if (rotationTruncRight >= 360) {
      rotationTruncRight = rotationTruncRight - 360;
    } else if (rotationTruncRight <= 0) {
      rotationTruncRight = 360 + rotationTruncRight;
    }

    const castDirectionLeft = new RAPIER.Vector2(
      this.sinCosTable.getCos(rotationTruncLeft),
      this.sinCosTable.getSin(rotationTruncLeft),
    );
    const castMaxToi = 20.0; // Maximum time of impact (TOI)
    const castDirectionRight = new RAPIER.Vector2(
      this.sinCosTable.getCos(rotationTruncRight),
      this.sinCosTable.getSin(rotationTruncRight),
    );
    // Perform shape cast
    const hit: RAPIER.ColliderShapeCastHit = this.rapierWorld.castShape(
      dynamicPosition,
      dynamicRotation,
      castDirectionLeft,
      scaledShape,
      0,
      castMaxToi,
      false,
      undefined,
      undefined,
      undefined,
      undefined,
      (collider: RAPIER.Collider) => {
        if (collider === this.playerCollider) {
          return false;
        } else return true;
      }, // Collision filter function (returns true for all colliders)
    ) as RAPIER.ColliderShapeCastHit;
    const hit2: RAPIER.ColliderShapeCastHit = this.rapierWorld.castShape(
      dynamicPosition,
      dynamicRotation,
      castDirectionRight,
      scaledShape,
      0,
      castMaxToi,
      false,
      undefined,
      undefined,
      undefined,
      undefined,
      (collider: RAPIER.Collider) => {
        if (collider === this.playerCollider) {
          return false;
        } else return true;
      }, // Collision filter function (returns true for all colliders)
    ) as RAPIER.ColliderShapeCastHit;

    // Check for the result of the shape cast
    if (hit) {
      this.lastHits.set('hit', hit);
      console.dir(hit);
      console.log('Shape cast hit detected!');
      // console.log(`Collider handle: ${hit.colliderHandle}`);
      console.log(`TOI (time of impact): ${hit.normal1}`);
      // console.log(`Impact point: ${hit.witness1.toArray()}`);
      // console.log(`Normal at impact: ${hit.normal1.toArray()}`);
      // const normal1 = hit.normal1;
      // const desiredTranslation = { x: 0, y: 0 };
      // desiredTranslation.x += this.speed * (delta / 1000) * normal1.x;
      // desiredTranslation.y += this.speed * (delta / 1000) * normal1.y;
      // this.movePlayer(desiredTranslation, true);
      this.rotationLock = true;
    } else if (hit2) {
      this.lastHits.set('hit2', hit2);
      console.dir(hit2);
      console.log('Shape cast hit detected!');
      // console.log(`Collider handle: ${hit.colliderHandle}`);
      console.log(`TOI (time of impact): ${hit2.normal1}`);
      // console.log(`Impact point: ${hit.witness1.toArray()}`);
      // console.log(`Normal at impact: ${hit.normal1.toArray()}`);
      // const normal1 = hit.normal1;
      // const desiredTranslation = { x: 0, y: 0 };
      // desiredTranslation.x += this.speed * (delta / 1000) * normal1.x;
      // desiredTranslation.y += this.speed * (delta / 1000) * normal1.y;
      // this.movePlayer(desiredTranslation, true);
      this.rotationLock = true;
    } else {
      console.log('No collision detected.');
    }

    this.drawShapeCast(
      dynamicPosition,
      castDirectionLeft,
      castMaxToi,
      polygon,
      [hit],
    );
    this.drawShapeCast(
      dynamicPosition,
      castDirectionRight,
      castMaxToi,
      polygon,
      [hit2],
    );
  }

  drawShapeCast(
    start: RAPIER.Vector,
    direction: RAPIER.Vector,
    maxToi: number,
    shape: Phaser.GameObjects.Polygon,
    hits: RAPIER.ColliderShapeCastHit[],
  ) {
    this.graphics.clear();

    // Draw cast line
    const endX = start.x + direction.x * this.speed;
    const endY = start.y + direction.y * this.speed;
    this.graphics.lineStyle(10, 0x0000ff, 1);
    this.graphics.lineBetween(
      start.x * 50,
      start.y * 50,
      endX * 100,
      endY * 100,
    );
    const line = new Phaser.Geom.Line(start.x, start.y, endX, endY);
    this.graphics.strokeLineShape(line);
    // Draw cast shape at start
    // this.drawShape(start.x, start.y, shape, 0x0000ff, 0.3);
    // const shapeObject = new Phaser.GameObjects.Image(
    //   this.scene,
    //   start.x,
    //   start.y,
    //   'star',
    //   0,
    // ).setDepth(100);
    this.castShapes.push({
      shape: shape,
      maxToi: maxToi,
      direction: direction,
    });

    // this.scene.add.existing(shape);
    // Draw hits
    hits.forEach((hit, index) => {
      if (!(hit !== null && hit.witness1 !== null)) {
        return;
      }
      console.log('Hit:');
      console.dir(hit);
      // const hitX = start.x + (direction.x * hit.time_of_impact) / maxToi;
      // const hitY = start.y + (direction.y * hit.time_of_impact) / maxToi;
      // Draw hit point
      // this.graphics.fillStyle(0xff0000);
      // this.graphics.fillCircle(hitX * 50, hitY * 50, 5);
      // // Draw normal
      // this.graphics.lineStyle(2, 0x00ff00);
      // this.graphics.lineBetween(
      //   hitX * 50,
      //   hitY * 50,
      //   (hitX + hit.normal1.x * 0.5) * 50,
      //   (hitY + hit.normal1.y * 0.5) * 50,
      // );
      // Draw cast shape at hit point
      // const shape = new RAPIER.Cuboid(2, 2);
      // this.drawShape(hit.witness1.x, hit.witness1.y, shape, 0xff0000, 0.5);
      this.graphics.lineStyle(10, 0xff0000, 1);
      this.graphics.fillStyle(0xff0000, 1);
      this.graphics.fillPoint(hit.witness1.x, hit.witness1.y, 100);
    });
  }

  drawShape(
    x: number,
    y: number,
    shape: RAPIER.Shape,
    color: number,
    alpha: number,
  ) {
    // this.graphics.save();
    // this.graphics.translateCanvas(x * 50, y * 50);
    if (shape.type === RAPIER.ShapeType.Cuboid) {
      const cuboid = shape as RAPIER.Cuboid;
      const width = cuboid.halfExtents.x * 2;
      const height = cuboid.halfExtents.y * 2;
      this.graphics.fillStyle(color, alpha);
      this.graphics.fillRect(-width / 2, -height / 2, width, height);
    }
    // this.graphics.restore();
  }

  updateAngularDebugPanel() {
    const currentPlayerXPosition = this.playerRigidBody.translation().x;
    const currentPlayerYPosition = this.playerRigidBody.translation().y;
    const rotationTrunc = Math.trunc(this.currentRotation);

    const truncCurrentPlayerXPosition = currentPlayerXPosition.toFixed(2);
    const truncCurrentPlayerYPosition = currentPlayerYPosition.toFixed(2);
    // const truncPreviousPlayerXPosition = previousPlayerXPosition.toFixed(2);
    // const truncPreviousPlayerYPosition = previousPlayerYPosition.toFixed(2);
    const currPlayerPos = `curr player pos: X: ${truncCurrentPlayerXPosition} Y: ${truncCurrentPlayerYPosition}`;
    // const prevPlayerPos = `previous player pos: X: ${truncPreviousPlayerXPosition} Y: ${truncPreviousPlayerYPosition}`;
    const currPlayerRotation = `curr player rotation: ${rotationTrunc}`;
    const rotationSpeed = `curr rotation speed: ${this.rotationSpeed}`;
    const speed = `curr speed: ${this.speed}`;

    EventBus.emit(
      'update-data-point',
      this,
      currPlayerPos +
        '\n' +
        currPlayerRotation +
        '\n' +
        speed +
        '\n' +
        rotationSpeed,
    );
  }

  isometricToCartesian(isoPt: Phaser.Geom.Point): Phaser.Geom.Point {
    const tempPt = new Phaser.Geom.Point();
    tempPt.x = (2 * isoPt.y + isoPt.x) / 2;
    tempPt.y = (2 * isoPt.y - isoPt.x) / 2;
    return tempPt;
  }

  cartesianToIsometric(cartPt: Phaser.Geom.Point): Phaser.Geom.Point {
    const tempPt = new Phaser.Geom.Point();
    tempPt.x = cartPt.x - cartPt.y;
    tempPt.y = (cartPt.x + cartPt.y) / 2;
    return tempPt;
  }

  // override destroy() {}
}
