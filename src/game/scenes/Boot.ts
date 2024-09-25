import { Scene } from 'phaser';
import { environment } from '../../environments/environment';

export class Boot extends Scene {
  constructor() {
    super('Boot');
  }

  preload() {
    const assetsDir: string = environment.assetsDir;
    //  The Boot Scene is typically used to load in any assets you require for your Preloader, such as a game logo or background.
    //  The smaller the file size of the assets, the better, as the Boot Scene itself has no preloader.
    this.load.setPath(`${assetsDir}/`);
    this.load.image('background', 'bg.png');
  }

  create() {
    this.scene.start('Preloader');
  }
}
