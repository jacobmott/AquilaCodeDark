import { EventBus } from "../EventBus";
import { Scene } from "phaser";
import * as spine from "@esotericsoftware/spine-phaser";
import RAPIER from "@dimforge/rapier2d-compat";
// import RAPIER from "@dimforge/rapier3d-compat";
import { SkinsAndAnimationBoundsProvider } from "@esotericsoftware/spine-phaser";

export class GameScene extends Scene {
  camera: Phaser.Cameras.Scene2D.Camera;
  background: Phaser.GameObjects.Image;
  gameText: Phaser.GameObjects.Text;
  // game: Phaser.Game;
  label: Phaser.GameObjects.Text;
  label2: Phaser.GameObjects.Text;
  pointer: Phaser.Input.Pointer;
  tileTintOrig: number = 0xffffff;
  eventQueue: RAPIER.EventQueue;

  keyA: Phaser.Input.Keyboard.Key;
  keyS: Phaser.Input.Keyboard.Key;
  keyD: Phaser.Input.Keyboard.Key;
  keyW: Phaser.Input.Keyboard.Key;
  keyE: Phaser.Input.Keyboard.Key;
  keyR: Phaser.Input.Keyboard.Key;
  keyC: Phaser.Input.Keyboard.Key;
  keyV: Phaser.Input.Keyboard.Key;

  enemy: Phaser.GameObjects.Image;
  enemyBody: RAPIER.RigidBody;
  enemyCollider: RAPIER.Collider;

  isoDirectionNegativeX: Phaser.Geom.Point;
  isoDirectionPositiveX: Phaser.Geom.Point;
  isoDirectionPositiveY: Phaser.Geom.Point;
  isoDirectionNegativeY: Phaser.Geom.Point;

  isoDirections: Map<number, Phaser.Geom.Point> = new Map();

  roof_five_layer: Phaser.Tilemaps.TilemapLayer;

  speed: number = 1000;
  rotationSpeed: number = 300;
  ready: boolean = false;
  currentRotation: number = 0;

  spineObject: spine.SpineGameObject;
  spineTrackEntry: spine.TrackEntry;

  rapierWorld: RAPIER.World;

  debugGraphics: Phaser.GameObjects.Graphics;
  playerRigidBody: RAPIER.RigidBody;
  playerCollider: RAPIER.Collider;
  // playerColliders: {} = { "defaut": RAPIER.Collider[]};
  playerColliders: {} = {};

  characterController: RAPIER.KinematicCharacterController;

  constructor() {
    super("Game2");
  }

  editorCreate(): void {
    // iso
    const iso = this.add.tilemap("iso");
    iso.addTilesetImage("iso-64x64-building", "iso-64x64-building");
    iso.addTilesetImage("iso-64x64-outside", "iso-64x64-outside");
    iso.addTilesetImage("64x64grid", "64x64grid");
    iso.addTilesetImage("64x64grid2", "64x64grid2");
    iso.addTilesetImage("64x64bottom-iso-grid", "64x64bottom-iso-grid");
    iso.addTilesetImage("singleskele", "singleskele");
    iso.addTilesetImage("ship3.png", "ship3", 1024, 1024);
    iso.orientation = "isometric";

    this.iso = iso;

    // this.events.emit("scene-awake");
  }

  private iso!: Phaser.Tilemaps.Tilemap;

  // private controls!: Phaser.Cameras.Controls.SmoothedKeyControl;
  private controls!: Phaser.Cameras.Controls.FixedKeyControl;

  async create() {
    const point = new Phaser.Geom.Point(-1, 0);

    this.isoDirectionNegativeX = this.cartesianToIsometric(point);
    const point2 = new Phaser.Geom.Point(1, 0);
    this.isoDirectionPositiveX = this.cartesianToIsometric(point2);
    const point3 = new Phaser.Geom.Point(0, 1);
    this.isoDirectionPositiveY = this.cartesianToIsometric(point3);
    const point4 = new Phaser.Geom.Point(0, -1);
    this.isoDirectionNegativeY = this.cartesianToIsometric(point4);

    // 0 to 90
    let y2 = 0;
    let x2 = 90;
    for (let i: number = 0; i <= 90; i++) {
      // y goes from 0 in the right direction.. to -1 in the up direction
      let ypercent = 0;
      if (y2 === 0) {
        ypercent = 0;
      } else {
        ypercent = y2 / 90;
      }

      // x goes from 1 in the right direction.. to 0 in the up direction
      const xpercent = x2 === 0 ? 0 : x2 / 90;
      this.isoDirections.set(
        i,
        new Phaser.Geom.Point(1 * xpercent, -1 * ypercent),
      );
      y2++;
      x2--;
    }

    // 90 to 180
    let y3 = 90;
    let x3 = 0;
    for (let i: number = 90; i <= 180; i++) {
      // Y goes from -1 in the up direction.. to 0 in the left direction
      let ypercent = 0;
      if (x3 === 0) {
        ypercent = 0;
      } else {
        ypercent = 90 / x3;
      }

      // X goes from 0 in the up direction.. to -1 in the left direction
      const xpercent = x3 === 0 ? 0 : x3 / 90;
      this.isoDirections.set(
        i,
        new Phaser.Geom.Point(-1 * xpercent, -1 * ypercent),
      );
      y3--;
      x3++;
    }

    // 180 to 270
    let y4 = 0;
    let x4 = 90;
    for (let i: number = 180; i <= 270; i++) {
      // X goes from -1 in the left direction.. to 0 in the down direction
      let xpercent = 0;
      if (x4 === 0) {
        xpercent = 0;
      } else {
        xpercent = 90 / x4;
      }

      // Y goes from 0 in the left direction.. to 1 in the down direction
      const ypercent = y4 === 0 ? 0 : y4 / 90;
      this.isoDirections.set(
        i,
        new Phaser.Geom.Point(-1 * xpercent, 1 * ypercent),
      );
      y4++;
      x4--;
    }

    // 270 to 360
    let y5 = 90;
    let x5 = 0;
    for (let i: number = 270; i <= 360; i++) {
      // Y goes from 1 in the down direction.. to 0 in the right direction
      let ypercent = 0;
      if (y5 === 0) {
        ypercent = 0;
      } else {
        ypercent = 90 / y5;
      }

      // X goes from 0 in the down direction.. to 1 in the right direction
      const xpercent = x5 === 0 ? 0 : x5 / 90;
      this.isoDirections.set(
        i,
        new Phaser.Geom.Point(1 * xpercent, 1 * ypercent),
      );
      y5--;
      x5++;
    }

    // const list: number[] = [
    //   315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329,
    //   330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344,
    //   345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359,
    //   360, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    //   20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    //   38, 39, 40, 41, 42, 43, 44, 45,
    // ];
    // console.log("LIST length");
    // console.log(list.length);
    // // 315 to 45 (315 to 360 - 0 to 45)
    // let y1 = 0;
    // let x1 = 90;
    // list.forEach((number: number) => {
    //   // X goes from -1 in the left direction.. to 0 in the down direction
    //   let xpercent = 0;
    //   if (x1 === 0) {
    //     xpercent = 0;
    //   } else {
    //     xpercent = 90 / x1;
    //   }

    //   // Y goes from 0 in the left direction.. to 1 in the down direction
    //   const ypercent = y1 === 0 ? 0 : y1 / 90;
    //   this.isoDirections.set(
    //     number,
    //     this.cartesianToIsometric(
    //       new Phaser.Geom.Point(-1 * xpercent, 1 * ypercent),
    //     ),
    //   );
    //   y1++;
    //   x1--;
    // });
    console.log("iso directions");
    console.dir(this.isoDirections);

    this.editorCreate();

    this.iso.createLayer("1-ground", ["iso-64x64-outside"], 0, 0);
    // 2_grid_1
    const grid_one_layer = this.iso.createLayer(
      "1-grid",
      ["64x64grid2", "64x64bottom-iso-grid"],
      0,
      0,
    );

    // const player_layer = this.iso.createLayer("player", ["singleskele"], 0, 0);
    const player_layer = this.iso.createLayer("player", ["singleskele"], 0, 0);
    const ships_layer: Phaser.Tilemaps.TilemapLayer = this.iso.createLayer(
      "ships",
      ["ship3.png"],
      0,
      -960, //We have to do this for some reason since this is an image and its 1024 instead of 64 width.. 1024 -64 = 960
    );
    //https://github.com/phaserjs/phaser/issues/5494
    ships_layer.setSkipCull(true);

    this.iso.createLayer(
      "2-stonebase",
      ["iso-64x64-building", "iso-64x64-outside"],
      0,
      0,
    );

    // 2_grid_1
    const grid_two_layer = this.iso.createLayer("2-grid", ["64x64grid"], 0, 0);

    this.iso.createLayer(
      "3-wall",
      ["iso-64x64-building", "iso-64x64-outside"],
      0,
      0,
    );
    this.iso.createLayer(
      "4-roof",
      ["iso-64x64-building", "iso-64x64-outside"],
      0,
      0,
    );

    // 5_roof_1
    this.roof_five_layer = this.iso.createLayer(
      "5-roof",
      ["iso-64x64-building", "iso-64x64-outside"],
      0,
      0,
    );

    // 5_grid_1
    const grid_five_layer = this.iso.createLayer("5-grid", ["64x64grid"], 0, 0);

    // this.player = new Phaser.GameObjects.Image(
    //   this,
    //   930,
    //   1066,
    //   "ship3",
    //   0,
    // ).setDepth(100);
    // this.add.existing(this.player);
    this.enemy = new Phaser.GameObjects.Image(
      this,
      930,
      1066,
      "ship3",
      0,
    ).setDepth(100);
    this.add.existing(this.enemy);
    this.ready = true;

    this.label = this.add.text(512, 384, "(x, y)", {
      fontFamily: '"Monospace"',
    });
    this.label2 = this.add.text(512, 384, "(x, y)", {
      fontFamily: '"Monospace"',
    });
    this.pointer = this.input.activePointer;

    const rect1 = this.add.rectangle(0, 0, 20, 20, 0x6666ff, 1);
    this.add
      .text(0, 0, "0,0", {
        fontFamily: "Arial Black",
        fontSize: 8,
        color: "#ffffff",
        stroke: "#000000",
        strokeThickness: 8,
        align: "center",
      })
      .setOrigin(0.5)
      .setDepth(100);

    this.iso;
    const rect2 = this.add.rectangle(4096, 0, 20, 20, 0x6666ff, 1);
    this.add
      .text(4096, 0, "4096,0", {
        fontFamily: "Arial Black",
        fontSize: 8,
        color: "#ffffff",
        stroke: "#000000",
        strokeThickness: 8,
        align: "center",
      })
      .setOrigin(0.5)
      .setDepth(100);

    this.loadSpineAssets();

    this.setupInputListeners();

    this.setupCameras();

    await this.setupRapierPhysics();

    this.setupDebugRapierGraphics();

    this.setupObstacles();

    this.characterController = this.rapierWorld.createCharacterController(0.01);

    EventBus.emit("current-scene-ready", this);
  }

  async setupRapierPhysics() {
    // Initialization: Initialize Rapier with await RAPIER.init().
    // Then, create a new Rapier world with gravity set to 9.81:
    await RAPIER.init();

    this.rapierWorld = new RAPIER.World(new RAPIER.Vector2(0.0, 0));
    this.rapierWorld;
    this.eventQueue = new RAPIER.EventQueue(false);
    // Create a Rapier dynamic rigid body
    // RAPIER.RigidBodyDesc.kinematicPositionBased()
    // const bodyDesc = RAPIER.RigidBodyDesc.dynamic();
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

    this.getPlayerColliderBox();
    // const playerColliderBox = this.getPlayerColliderBox();
    // console.dir(playerColliderBox);
    // const playerColliderDesc =
    //   RAPIER.ColliderDesc.convexHull(playerColliderBox); // Use a circle with radius 0.5
    // this.playerCollider = this.rapierWorld.createCollider(
    //   playerColliderDesc,
    //   this.playerRigidBody,
    // );

    const circleColliderDesc = RAPIER.ColliderDesc.ball(this.enemy.width / 2);
    const circleRigidBodyDesc = RAPIER.RigidBodyDesc.dynamic().setTranslation(
      this.enemy.x,
      this.enemy.y,
    );
    circleRigidBodyDesc.setUserData(this.enemy);

    this.enemyBody = this.rapierWorld.createRigidBody(circleRigidBodyDesc);

    // Create the collider.
    this.enemyCollider = this.rapierWorld.createCollider(
      circleColliderDesc,
      this.enemyBody,
    );
    // this.enemyCollider.setFriction(0);
    this.enemyCollider.setRestitution(1);
  }

  setupObstacles() {
    const obstacle = new Phaser.GameObjects.Image(
      this,
      2000,
      1066,
      "largewall",
      0,
    ).setDepth(100);
    this.add.existing(obstacle);

    const data = this.cache.json.get("largewall-v1-plaintext_convex_sub");

    const sprites: any[] = [];
    console.log("json data");
    console.dir(data);
    data.sprites.forEach((sprite) => {
      const spriteNew: any = {};
      spriteNew.name = sprite.name;
      const shapes: Float32Array[] = [];
      //put this back if we can ever do compound shapes
      //https://github.com/dimforge/rapier.js/issues/44
      const fullShape: number[] = [];
      sprite.convexSubPolygons.forEach((convexSubPolygon) => {
        fullShape.push(...convexSubPolygon);
        shapes.push(new Float32Array(convexSubPolygon));
      });
      const shapesNew: Float32Array = new Float32Array(fullShape);
      spriteNew.convexSubPolygons = shapes;
      spriteNew.fullHull = shapesNew;
      sprites.push(spriteNew);
    });
    console.log("sprites");
    console.dir(sprites);

    const bodyDesc = RAPIER.RigidBodyDesc.fixed();

    // Set its initial position to match our Phaser Game Object
    bodyDesc.setTranslation(obstacle.x, obstacle.y);

    // Store the Phaser Game Object in the rigid body's user data so we can sync its position and rotation
    bodyDesc.setUserData(obstacle);

    // Finally, create the rigid body in the Rapier world from the body description
    const rigidBody = this.rapierWorld.createRigidBody(bodyDesc);

    // this.spineObject.updateSize;
    sprites.forEach((sprite) => {
      // const shape = sprite.convexSubPolygons as Float32Array[];
      const name = sprite.name;
      // console.log(score);
      // const playerColliderBox = this.getPlayerColliderBox();
      // console.dir(playerColliderBox);
      // const playerColliderDesc = RAPIER.ColliderDesc.ball(500); // Use a circle with radius 0.5
      //We have to do this for now so we have a single collider for character controller
      //https://github.com/dimforge/rapier.js/issues/44
      // sprite.convexSubPolygons.forEach((shape) => {
      //   console.log("shape");
      //   console.dir(shape);
      //   const wallColliderDesc = RAPIER.ColliderDesc.polyline(shape); // Use a circle with radius 0.5
      //   // playerColliderDesc.setTranslation(-512, -256);
      //   const collider = this.rapierWorld.createCollider(
      //     wallColliderDesc,
      //     rigidBody,
      //   );
      //   // this.playerColliders[sprite.name as string] = collider;
      //   // this.playerCollider = collider;
      //   // this.playerCollider.setEnabled(false);
      // });

      const colliderDesc = RAPIER.ColliderDesc.convexHull(sprite.fullHull); // Use a circle with radius 0.5
      // playerColliderDesc.setTranslation(-512, -256);
      const collider = this.rapierWorld.createCollider(colliderDesc, rigidBody);
      collider.setActiveEvents(RAPIER.ActiveEvents.COLLISION_EVENTS);
    });

    // RAPIER.ColliderDesc.cuboid(0.5, 0.2);
  }

  getPlayerColliderBox() {
    const data = this.cache.json.get("ship3-v1-plaintext_convex_sub");

    const sprites: any[] = [];
    console.log("json data");
    console.dir(data);
    data.sprites.forEach((sprite) => {
      const spriteNew: any = {};
      spriteNew.name = sprite.name;
      const shapes: Float32Array[] = [];
      //put this back if we can ever do compound shapes
      //https://github.com/dimforge/rapier.js/issues/44
      const fullShape: number[] = [];
      sprite.convexSubPolygons.forEach((convexSubPolygon) => {
        fullShape.push(...convexSubPolygon);
        shapes.push(new Float32Array(convexSubPolygon));
      });
      const shapesNew: Float32Array = new Float32Array(fullShape);
      spriteNew.convexSubPolygons = shapes;
      spriteNew.fullHull = shapesNew;
      sprites.push(spriteNew);
    });
    console.log("sprites");
    console.dir(sprites);

    // this.spineObject.updateSize;
    sprites.forEach((sprite) => {
      // const shape = sprite.convexSubPolygons as Float32Array[];
      const name = sprite.name;
      // console.log(score);
      // const playerColliderBox = this.getPlayerColliderBox();
      // console.dir(playerColliderBox);
      // const playerColliderDesc = RAPIER.ColliderDesc.ball(500); // Use a circle with radius 0.5
      //We have to do this for now so we have a single collider for character controller
      //https://github.com/dimforge/rapier.js/issues/44
      // sprite.convexSubPolygons.forEach((shape) => {
      //   console.log("shape");
      //   console.dir(shape);
      //   const playerColliderDesc = RAPIER.ColliderDesc.polyline(shape); // Use a circle with radius 0.5
      //   // playerColliderDesc.setTranslation(-512, -256);
      //   const collider = this.rapierWorld.createCollider(
      //     playerColliderDesc,
      //     this.playerRigidBody,
      //   );
      //   this.playerColliders[sprite.name as string] = collider;
      //   this.playerCollider = collider;
      //   this.playerCollider.setEnabled(false);
      // });

      this.playerColliders[sprite.name as string] = this.playerCollider;
      const playerColliderDesc = RAPIER.ColliderDesc.convexHull(
        sprite.fullHull,
      ); // Use a circle with radius 0.5
      // playerColliderDesc.setTranslation(-512, -256);
      const playerCollider = this.rapierWorld.createCollider(
        playerColliderDesc,
        this.playerRigidBody,
      );

      playerCollider.setEnabled(false);
      this.playerColliders[sprite.name as string] = playerCollider;
      // this.playerColliders[sprite.name as string].setCollisionGroups(
      //   0x00010001,
      // );
      this.playerColliders[sprite.name as string].setActiveEvents(
        RAPIER.ActiveEvents.COLLISION_EVENTS,
      );
      this.rapierWorld.contactPairsWith(playerCollider, (otherCollider) => {
        console.log("contact pair");
        console.dir(otherCollider);
      });
    });

    this.playerCollider = this.playerColliders["ship3IsoDL"];
    this.playerCollider.setEnabled(true);
  }

  handleRapierPhysicsUpdate() {
    // Check if the Rapier world is initialized.
    if (this.rapierWorld !== undefined) {
      // Step the physics simulation.

      const eventQueue = new RAPIER.EventQueue(true);
      this.rapierWorld.step(eventQueue);

      eventQueue.drainCollisionEvents((handle1, handle2, started) => {
        /* Handle the collision event. */
        console.log("contact collision event");
        console.log("handle1");
        console.dir(handle1);
        console.log("handle2");
        console.dir(handle2);
        console.log("started");
        console.dir(started);
      });

      eventQueue.drainContactForceEvents((event) => {
        const handle1 = event.collider1(); // Handle of the first collider involved in the event.
        const handle2 = event.collider2(); // Handle of the second collider involved in the event.
        console.log("contact force event");
        console.log("handle1");
        console.dir(handle1);
        console.log("handle2");
        console.dir(handle2);
        /* Handle the contact force event. */
      });
    }
  }
  updateGameObjectsForRapierPhysics() {
    // Update the Phaser game objects based on the physics simulation.
    this.rapierWorld.bodies.forEach((rigidBody) => {
      const gameObject = rigidBody.userData as
        | Phaser.GameObjects.Image
        | spine.SpineGameObject;
      if (gameObject !== undefined) {
        const position = rigidBody.translation();
        const angle = rigidBody.rotation();
        gameObject.x = position.x;
        gameObject.y = position.y;
        gameObject.setRotation(angle);
      }
    });
  }

  setupCameras() {
    this.cameras.main.setZoom(1);
    this.cameras.main.setScroll(-200, 200);
    this.cameras.main.setSize(1920, 1080);
  }

  setupInputListeners() {
    this.keyA = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.A);
    this.keyS = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.S);
    this.keyD = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.D);
    this.keyW = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.W);
    this.keyE = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.E);
    this.keyR = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.R);
    this.keyC = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.C);
    this.keyV = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.V);

    // const cursors = this.input.keyboard!.createCursorKeys();

    const cursors = this.input.keyboard.addKeys({
      up: Phaser.Input.Keyboard.KeyCodes.W,
      down: Phaser.Input.Keyboard.KeyCodes.S,
      left: Phaser.Input.Keyboard.KeyCodes.A,
      right: Phaser.Input.Keyboard.KeyCodes.D,
    });

    // const controlConfig = {
    //   camera: this.cameras.main,
    //   zoomSpeed: 0.05,
    //   left: cursors.left,
    //   right: cursors.right,
    //   up: cursors.up,
    //   down: cursors.down,
    //   acceleration: 0.04,
    //   zoomIn: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.Q),
    //   zoomOut: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.E),
    //   drag: 0.0007,
    //   maxSpeed: 0.7,
    // };

    const controlConfig = {
      camera: this.cameras.main,
      zoomSpeed: 0.05,
      left: cursors["left"],
      right: cursors["right"],
      up: cursors["up"],
      down: cursors["down"],
      zoomIn: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.Q),
      zoomOut: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.E),
      speed: 1,
    };

    this.input.on("wheel", (pointer, gameObjects, deltaX, deltaY, deltaZ) => {
      if (deltaY > 0) {
        const newZoom = this.cameras.main.zoom - 0.1;
        if (newZoom > 0.00001) {
          this.cameras.main.zoom = newZoom;
        }
      }

      if (deltaY < 0) {
        const newZoom = this.cameras.main.zoom + 0.1;
        if (newZoom < 5) {
          this.cameras.main.zoom = newZoom;
        }
      }

      // this.camera.centerOn(pointer.worldX, pointer.worldY);
      // this.camera.pan(pointer.worldX, pointer.worldY, 2000, "Power2");
    });

    this.input.on("pointermove", (pointer) => {
      if (!pointer.isDown) return;

      this.cameras.main.scrollX -=
        (pointer.x - pointer.prevPosition.x) / this.cameras.main.zoom;
      this.cameras.main.scrollY -=
        (pointer.y - pointer.prevPosition.y) / this.cameras.main.zoom;
    });

    // this.controls = new Phaser.Cameras.Controls.SmoothedKeyControl(
    //   controlConfig,
    // );
    // this.controls = new Phaser.Cameras.Controls.FixedKeyControl(controlConfig);

    // this.gameText = this.add.text(512, 384, 'Make something fun!\nand share it with us:\nsupport@phaser.io', {
    //     fontFamily: 'Arial Black', fontSize: 38, color: '#ffffff',
    //     stroke: '#000000', strokeThickness: 8,
    //     align: 'center'
    // }).setOrigin(0.5).setDepth(100);
    // const r1 = this.add.rectangle(200, 200, 148, 148, 0x6666ff);
    this.input.on("pointerup", (pointer) => {
      // const tile = layer.getTileAtWorldXY(pointer.worldX, pointer.worldY);

      // const x = pointer.worldX;
      // const y = pointer.worldY;

      // const tileXY = layer.worldToTileXY(pointer.worldX, pointer.worldY, true);

      // const tile = layer.getTileAt(tileXY.x, tileXY.y);

      const tile = this.roof_five_layer.getIsoTileAtWorldXY(
        pointer.worldX,
        pointer.worldY,
        false,
      );

      if (tile) {
        tile.tint = 0xffffff;
      }

      // const tileX = layer.worldToTileX(x, true);
      // const tileY = layer.worldToTileX(y, true);
      // console.log(tileXY, tileX, tileY);
      // console.log(tileXY);
    });

    this.input.on("pointerdown", (pointer) => {
      // const tile = layer.getTileAtWorldXY(pointer.worldX, pointer.worldY);

      // const x = pointer.worldX;
      // const y = pointer.worldY;

      // const tileXY = layer.worldToTileXY(pointer.worldX, pointer.worldY, true);

      // const tile = layer.getTileAt(tileXY.x, tileXY.y);

      const tile = this.roof_five_layer.getIsoTileAtWorldXY(
        pointer.worldX,
        pointer.worldY,
        false,
        false,
        this.cameras.main,
      );

      if (tile) {
        // if (tile.tint !== 0xff0000) {
        //   this.tileTintOrig = tile.tint;
        // }
        tile.tint = 0xff0000;
      }

      // const tileX = layer.worldToTileX(x, true);
      // const tileY = layer.worldToTileX(y, true);
      // console.log(tileXY, tileX, tileY);
      // console.log(tileXY);
    });

    this.input.on("pointermove", (pointer) => {
      const px = Math.trunc(pointer.x);
      const py = Math.trunc(pointer.y);
      const pwx = Math.trunc(pointer.worldX);
      const pwy = Math.trunc(pointer.worldY);
      let xOffsetAmount = 40;
      let yOffsetAmount = 40;
      if (px > this.game.canvas.width / 2) {
        xOffsetAmount = -160;
      }
      if (py > this.game.canvas.height / 2) {
        yOffsetAmount = -40;
      }
      this.label.setDepth(1001);
      this.label.setText("world(iso)X/Y: (" + pwx + ", " + pwy + ")");
      this.label2.setText("pointerX/Y: (" + px + ", " + py + ")");
      this.label2.setDepth(1001);
      this.label.x = pwx + xOffsetAmount;
      this.label.y = pwy + yOffsetAmount;
      this.label2.x = pwx + xOffsetAmount;
      this.label2.y = pwy + yOffsetAmount + 20;
    });
  }

  lostFocus() {
    // console.log("lost focus");
    this.input.keyboard.manager.enabled = true;
  }

  gainFocus() {
    // console.log("gained focus");
    this.input.keyboard.manager.enabled = false;
  }

  loadSpineAssets() {
    // Create a SpineGameObject through the GameObjectFactory and add it to the scene
    this.spineObject = this.add.spine(
      1545,
      100,
      "Ship0-360-data",
      "Ship0-360-atlas",
    );

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

    this.spineObject.skeleton.setSkinByName("defaultskin");
    this.spineObject.setScale(1);
    this.spineObject.setInteractive();

    this.input.enableDebug(this.spineObject, 0xff00ff);

    this.spineTrackEntry = this.spineObject.animationState.setAnimation(
      0,
      "1",
      false,
    );
  }

  setupDebugRapierGraphics() {
    this.debugGraphics = this.add.graphics();
    this.debugGraphics.setDepth(1000);
  }

  override update(time: number, delta: number) {
    if (!this.ready) {
      return;
    }
    if (!this.characterController) {
      return;
    }
    const previousPlayerXPosition = this.playerRigidBody?.translation()?.x;
    const previousPlayerYPosition = this.playerRigidBody?.translation()?.y;

    this.handleRapierPhysicsUpdate();

    // Render debug graphics
    this.debug();

    const wDown = this.keyW.isDown; //upRight
    const aDown = this.keyA.isDown; //upLeft
    const sDown = this.keyS.isDown; //downLeft
    const dDown = this.keyD.isDown; //downRight
    const eDown = this.keyE.isDown; //speed increase
    const rDown = this.keyR.isDown; //speed decrease
    const cDown = this.keyC.isDown; //rotation speed increase
    const vDown = this.keyV.isDown; //rotation speed decrease

    const gameObject = this.playerRigidBody;

    let moved = false;
    let rotated = false;
    const desiredTranslation = { x: 0, y: 0 };

    this.characterController.setSlideEnabled(true);
    if (wDown) {
      // moved = true;
    }
    if (sDown) {
      // moved = true;
    }

    if (aDown) {
      rotated = true;
      // this.spineTrackEntry.reverse = true;
      this.currentRotation -= this.rotationSpeed * (delta / 1000);
    } else if (dDown) {
      rotated = true;
      // this.spineTrackEntry.reverse = false;
      this.currentRotation += this.rotationSpeed * (delta / 1000);
    }

    if (rotated) {
      const result = this.currentRotation;
      if (result >= 360) {
        this.currentRotation = result - 360;
      } else if (result <= 0) {
        this.currentRotation = 360 + result;
      }
      const rotationTrunc = Math.trunc(this.currentRotation);
      const animation = rotationTrunc.toString();
      this.spineObject.animationState.setAnimation(0, animation, false);
    }

    // this.spineTrackEntry.timeScale = delta / 1000;
    // if (!moved) {
    // this.spineTrackEntry.animationEnd;
    // this.spineTrackEntry.timeScale = 0;
    // this.spineObject.animationState.update(0);
    // } else {
    // this.spineTrackEntry.timeScale = this.rotationSpeed * (delta / 1000);
    // console.log("timescale: " + this.spineTrackEntry.timeScale);
    // this.spineTrackEntry.timeScale = 1;
    // this.spineObject.animationState.update(delta);
    // }

    // if (aDown) {
    //   moved = true;
    //   // this.spineObject.x += this.isoDirectionNegativeX.x * this.speed;
    //   // this.spineObject.y += this.isoDirectionNegativeX.y * this.speed;
    //   desiredTranslation.x +=
    //     this.isoDirectionNegativeX.x * this.speed * (delta / 1000);
    //   desiredTranslation.y +=
    //     this.isoDirectionNegativeX.y * this.speed * (delta / 1000);
    //   EventBus.emit("add-scrolled-data", this, {
    //     identifier: "console log info",
    //     data: "A key pressed",
    //   });
    // }
    if (sDown) {
      moved = true;
      // desiredTranslation.x +=
      //   this.isoDirectionPositiveY.x * this.speed * (delta / 1000);
      // desiredTranslation.y +=
      //   this.isoDirectionPositiveY.y * this.speed * (delta / 1000);
      //Get the direction we are facing based on our rotation, then add so we go backwrads
      const rotationTrunc = Math.trunc(this.currentRotation);

      const isoDirection = this.isoDirections.get(rotationTrunc);
      const directionX = isoDirection.x;
      const directionY = isoDirection.y;
      console.log("directionX: " + directionX);
      console.log("directionY: " + directionY);

      desiredTranslation.x += this.speed * (delta / 1000) * directionX;
      desiredTranslation.y += this.speed * (delta / 1000) * directionY;

      // this.spineObject.x += this.isoDirectionPositiveY.x * this.speed;
      // this.spineObject.y += this.isoDirectionPositiveY.y * this.speed;
      EventBus.emit("add-scrolled-data", this, {
        identifier: "console log info",
        data: "S key pressed",
      });
    }
    // if (dDown) {
    //   moved = true;
    //   desiredTranslation.x +=
    //     this.isoDirectionPositiveX.x * this.speed * (delta / 1000);
    //   desiredTranslation.y +=
    //     this.isoDirectionPositiveX.y * this.speed * (delta / 1000);
    //   // this.spineObject.x += this.isoDirectionPositiveX.x * this.speed;
    //   // this.spineObject.y += this.isoDirectionPositiveX.y * this.speed;
    //   EventBus.emit("add-scrolled-data", this, {
    //     identifier: "console log info",
    //     data: "D key pressed",
    //   });
    // }
    if (wDown) {
      moved = true;
      // this.spineObject.x += this.isoDirectionNegativeY.x * this.speed;
      // this.spineObject.y += this.isoDirectionNegativeY.y * this.speed;
      // desiredTranslation.x +=
      //   this.isoDirectionNegativeY.x * this.speed * (delta / 1000);
      // desiredTranslation.y +=
      //   this.isoDirectionNegativeY.y * this.speed * (delta / 1000);

      //Get the direction we are facing based on our rotation, then subtract so we go forwards
      const rotationTrunc = Math.trunc(this.currentRotation);

      console.log("W down");
      console.log("this.isoDirections");
      const isoDirection = this.isoDirections.get(rotationTrunc);
      const directionX = isoDirection.x;
      const directionY = isoDirection.y;
      console.log("directionX: " + directionX);
      console.log("directionY: " + directionY);

      desiredTranslation.x -= this.speed * (delta / 1000) * directionX;
      desiredTranslation.y -= this.speed * (delta / 1000) * directionY;

      EventBus.emit("add-scrolled-data", this, {
        identifier: "console log info",
        data: "W key pressed",
      });
    }
    if (eDown) {
      this.speed += 100;
      if (this.speed >= 10000) {
        this.speed = 10000;
      }
      console.log("speed: " + this.speed);
    } else if (rDown) {
      this.speed -= 100;
      if (this.speed <= 0) {
        this.speed = 100;
      }
      console.log("speed: " + this.speed);
    }

    if (cDown) {
      this.rotationSpeed += 2;
      if (this.rotationSpeed >= 300) {
        this.rotationSpeed = 300;
      }
      console.log("rotationSpeed: " + this.rotationSpeed);
    } else if (vDown) {
      this.rotationSpeed -= 2;
      if (this.rotationSpeed <= 0) {
        this.rotationSpeed = 2;
      }
      console.log("rotationSpeed: " + this.rotationSpeed);
    }

    // console.log("currentPlayerXPosition: " + currentPlayerXPosition);
    // console.log("currentPlayerYPosition: " + currentPlayerYPosition);
    // console.log("previousPlayerXPosition: " + previousPlayerXPosition);
    // console.log("previousPlayerYPosition: " + previousPlayerYPosition);

    // const previousPlayerYPosition = this.playerRigidBody.translation().y;
    // const previousPlayerXPosition = this.playerRigidBody.translation().x;

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

    const currentPlayerXPosition = this.playerRigidBody.translation().x;
    const currentPlayerYPosition = this.playerRigidBody.translation().y;

    // this.animatePlayer(
    //   moved,
    //   currentPlayerXPosition,
    //   currentPlayerYPosition,
    //   previousPlayerXPosition,
    //   previousPlayerYPosition,
    // );

    this.updateGameObjectsForRapierPhysics();
  }

  // animatePlayer(
  //   moved,
  //   currentPlayerXPosition,
  //   currentPlayerYPosition,
  //   previousPlayerXPosition,
  //   previousPlayerYPosition,
  // ) {
  //   if (!moved) {
  //     return;
  //   }

  //   let positiveXMovement = false;
  //   let positiveYMovement = false;
  //   let negativeYMovement = false;
  //   let negativeXMovement = false;

  //   EventBus.emit(
  //     "update-data-point",
  //     this,
  //     "player currentPlayerPosition position: " +
  //       "X: " +
  //       currentPlayerXPosition +
  //       " Y: " +
  //       currentPlayerYPosition +
  //       "       player previousPlayerPosition position: " +
  //       "X: " +
  //       previousPlayerXPosition +
  //       " Y: " +
  //       previousPlayerYPosition,
  //   );

  //   if (currentPlayerXPosition > previousPlayerXPosition) {
  //     positiveXMovement = true;
  //   } else if (currentPlayerXPosition < previousPlayerXPosition) {
  //     negativeXMovement = true;
  //   }
  //   if (currentPlayerYPosition > previousPlayerYPosition) {
  //     positiveYMovement = true;
  //   } else if (currentPlayerYPosition < previousPlayerYPosition) {
  //     negativeYMovement = true;
  //   }

  //   this.playerCollider.setEnabled(false);
  //   if (positiveXMovement && positiveYMovement) {
  //     this.spineObject.animationState.setAnimation(0, "downright", true);
  //     this.playerCollider = this.playerColliders["ship3IsoDR"];
  //   } else if (negativeXMovement && negativeYMovement) {
  //     this.spineObject.animationState.setAnimation(0, "upleft", true);
  //     this.playerCollider = this.playerColliders["ship3IsoUL"];
  //   } else if (positiveXMovement && negativeYMovement) {
  //     this.spineObject.animationState.setAnimation(0, "upright", true);
  //     this.playerCollider = this.playerColliders["ship3IsoUR"];
  //   } else if (negativeXMovement && positiveYMovement) {
  //     this.spineObject.animationState.setAnimation(0, "downleft", true);
  //     this.playerCollider = this.playerColliders["ship3IsoDL"];
  //   } else if (positiveXMovement) {
  //     this.spineObject.animationState.setAnimation(0, "right", true);
  //     this.playerCollider = this.playerColliders["ship3IsoR"];
  //   } else if (negativeXMovement) {
  //     this.spineObject.animationState.setAnimation(0, "left", true);
  //     this.playerCollider = this.playerColliders["ship3IsoL"];
  //   } else if (positiveYMovement) {
  //     this.spineObject.animationState.setAnimation(0, "down", true);
  //     this.playerCollider = this.playerColliders["ship3IsoD"];
  //   } else if (negativeYMovement) {
  //     this.spineObject.animationState.setAnimation(0, "up", true);
  //     this.playerCollider = this.playerColliders["ship3IsoU"];
  //   }
  //   this.playerCollider.setEnabled(true);
  //   // console.log("bounds");
  //   // console.dir(this.spineObject.skeleton.getBoundsRect());
  // }

  animatePlayer(
    moved,
    currentPlayerXPosition,
    currentPlayerYPosition,
    previousPlayerXPosition,
    previousPlayerYPosition,
  ) {
    if (!moved) {
      return;
    }

    let positiveXMovement = false;
    let positiveYMovement = false;
    let negativeYMovement = false;
    let negativeXMovement = false;

    EventBus.emit(
      "update-data-point",
      this,
      "player currentPlayerPosition position: " +
        "X: " +
        currentPlayerXPosition +
        " Y: " +
        currentPlayerYPosition +
        "       player previousPlayerPosition position: " +
        "X: " +
        previousPlayerXPosition +
        " Y: " +
        previousPlayerYPosition,
    );

    if (currentPlayerXPosition > previousPlayerXPosition) {
      positiveXMovement = true;
    } else if (currentPlayerXPosition < previousPlayerXPosition) {
      negativeXMovement = true;
    }
    if (currentPlayerYPosition > previousPlayerYPosition) {
      positiveYMovement = true;
    } else if (currentPlayerYPosition < previousPlayerYPosition) {
      negativeYMovement = true;
    }

    this.playerCollider.setEnabled(false);
    if (positiveXMovement && positiveYMovement) {
      this.spineObject.animationState.setAnimation(0, "downright", true);
      this.playerCollider = this.playerColliders["ship3IsoDR"];
    } else if (negativeXMovement && negativeYMovement) {
      this.spineObject.animationState.setAnimation(0, "upleft", true);
      this.playerCollider = this.playerColliders["ship3IsoUL"];
    } else if (positiveXMovement && negativeYMovement) {
      this.spineObject.animationState.setAnimation(0, "upright", true);
      this.playerCollider = this.playerColliders["ship3IsoUR"];
    } else if (negativeXMovement && positiveYMovement) {
      this.spineObject.animationState.setAnimation(0, "downleft", true);
      this.playerCollider = this.playerColliders["ship3IsoDL"];
    } else if (positiveXMovement) {
      this.spineObject.animationState.setAnimation(0, "right", true);
      this.playerCollider = this.playerColliders["ship3IsoR"];
    } else if (negativeXMovement) {
      this.spineObject.animationState.setAnimation(0, "left", true);
      this.playerCollider = this.playerColliders["ship3IsoL"];
    } else if (positiveYMovement) {
      this.spineObject.animationState.setAnimation(0, "down", true);
      this.playerCollider = this.playerColliders["ship3IsoD"];
    } else if (negativeYMovement) {
      this.spineObject.animationState.setAnimation(0, "up", true);
      this.playerCollider = this.playerColliders["ship3IsoU"];
    }
    this.playerCollider.setEnabled(true);
    // console.log("bounds");
    // console.dir(this.spineObject.skeleton.getBoundsRect());
  }

  debug() {
    // Clear the previous debug graphics
    if (!this.debugGraphics) {
      return;
    }
    this.debugGraphics?.clear();

    // Get the debug render information from RAPIER
    const debugRender = this.rapierWorld?.debugRender();
    const vertices = debugRender?.vertices;
    const colors = debugRender?.colors;

    // Draw the debug lines for all objects in the RAPIER world
    for (let i = 0; i < vertices.length; i += 4) {
      const x1 = vertices[i];
      const y1 = vertices[i + 1];
      const x2 = vertices[i + 2];
      const y2 = vertices[i + 3];

      const colorIndex = i * 2;
      const r = colors[colorIndex];
      const g = colors[colorIndex + 1];
      const b = colors[colorIndex + 2];
      const a = colors[colorIndex + 3];

      this.debugGraphics.lineStyle(
        2,
        Phaser.Display.Color.GetColor(r * 255, g * 255, b * 255),
        a,
      );
      this.debugGraphics.lineBetween(x1, y1, x2, y2);
    }
  }

  changeScene() {
    this.scene.start("GameOver");
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
}
