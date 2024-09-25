import { Scene } from 'phaser';
import { environment } from '../../environments/environment';

export class Preloader extends Scene {
  constructor() {
    super('Preloader');
  }

  init() {
    //  We loaded this image in our Boot Scene, so we can display it here
    this.add.image(512, 384, 'background');

    //  A simple progress bar. This is the outline of the bar.
    this.add.rectangle(512, 384, 468, 32).setStrokeStyle(1, 0xffffff);

    //  This is the progress bar itself. It will increase in size from the left based on the % of progress.
    const bar = this.add.rectangle(512 - 230, 384, 4, 28, 0xffffff);

    //  Use the 'progress' event emitted by the LoaderPlugin to update the loading bar
    this.load.on('progress', (progress: number) => {
      //  Update the progress bar (our bar is 464px wide, so 100% = 464px)
      bar.width = 4 + 460 * progress;
    });
  }

  preload() {
    const assetsDir: string = environment.assetsDir;
    //  Load the assets for the game - Replace with your own assets
    this.load.setPath(`${assetsDir}/`);

    this.load.image('logo', 'logo.png');
    this.load.image('star', 'star.png');
    this.load.image('stars3', 'stars3.png');

    this.load.tilemapTiledJSON('map1', 'map1.json');

    this.load.tilemapTiledJSON('iso', 'iso.json');
    this.load.image('iso-64x64-building', 'iso-64x64-building.png');
    this.load.image('iso-64x64-outside', 'iso-64x64-outside.png');
    this.load.image('64x64grid', '64x64grid.png');
    this.load.image('64x64grid2', '64x64grid2.png');
    this.load.image('64x64bottom-iso-grid', '64x64bottom-iso-grid.png');
    this.load.image('singleskele', 'singleskele.png');
    this.load.image('ship3', 'ship3.png');

    //  Load body shapes from JSON file generated using PhysicsEditor
    this.load.json(
      'ship3-v1-plaintext_convex_sub',
      'physicseditor/ship3-v1-plaintext_convex_sub.json',
    );
    this.load.json(
      'ship0-360-v1-plaintext_convex_sub',
      'physicseditor/ship0-360-v1-plaintext_convex_sub.json',
    );

    this.load.json(
      'largewall-v1-plaintext_convex_sub',
      'physicseditor/largewall-v1-plaintext_convex_sub.json',
    );
    this.load.image('largewall', 'largewall.png');

    this.load.setPath('');

    this.load.spineBinary(
      'Ship0-360-data',
      `${assetsDir}/spine/ship0-360/Ship0-360.skel`,
    );
    this.load.spineAtlas(
      'Ship0-360-atlas',
      `${assetsDir}/spine/ship0-360/Ship0-360.atlas`,
    );

    // this.game.events.addListener(
    //   Phaser.Core.Events.FOCUS,
    //   this.gainFocus,
    //   this,
    // );
    // this.game.events.addListener(Phaser.Core.Events.BLUR, this.lostFocus, this);
    // this.events.on('addImage', this.handler, this);
    // this.input.keyboard.manager.enabled = false;
  }
  // this.game.events.addListener(
  //   Phaser.Core.Events.FOCUS,
  //   this.gainFocus,
  //   this,
  // );
  // this.game.events.addListener(Phaser.Core.Events.BLUR, this.lostFocus, this);
  // this.events.on('addImage', this.handler, this);
  // lostFocus() {
  //   console.log("lost focus");
  //   this.input.keyboard.manager.enabled = false;
  // }

  // gainFocus() {
  //   console.log("gained focus");
  //   this.input.keyboard.manager.enabled = true;
  // }

  create() {
    //  When all the assets have loaded, it's often worth creating global objects here that the rest of the game can use.
    //  For example, you can define global animations here, so we can use them in other scenes.

    //  Move to the MainMenu. You could also swap this for a Scene Transition, such as a camera fade.
    this.scene.start('Game2');
  }
}
