import { AquilaInputService } from '../services/AquilaInputService';
import { SceneComponent } from './SceneComponent';

export class AquilaTileMap extends SceneComponent {
  iso!: Phaser.Tilemaps.Tilemap;
  roof_five_layer!: Phaser.Tilemaps.TilemapLayer;
  tileTintOrig: number = 0xffffff;

  constructor(scene: Phaser.Scene, inputService: AquilaInputService) {
    super(scene, inputService);
  }

  // override awake() {}

  override start() {
    const iso = this.scene.add.tilemap('iso');
    iso.addTilesetImage('iso-64x64-building', 'iso-64x64-building');
    iso.addTilesetImage('iso-64x64-outside', 'iso-64x64-outside');
    iso.addTilesetImage('64x64grid', '64x64grid');
    iso.addTilesetImage('64x64grid2', '64x64grid2');
    iso.addTilesetImage('64x64bottom-iso-grid', '64x64bottom-iso-grid');
    iso.addTilesetImage('singleskele', 'singleskele');
    iso.addTilesetImage('ship3.png', 'ship3', 1024, 1024);
    iso.orientation = 'isometric';
    this.iso = iso;

    this.iso.createLayer('1-ground', ['iso-64x64-outside'], 0, 0);
    // 2_grid_1
    const grid_one_layer = this.iso.createLayer(
      '1-grid',
      ['64x64grid2', '64x64bottom-iso-grid'],
      0,
      0,
    );

    const skeleton_layer = this.iso.createLayer(
      'player',
      ['singleskele'],
      0,
      0,
    );
    const ships_layer: Phaser.Tilemaps.TilemapLayer = this.iso.createLayer(
      'ships',
      ['ship3.png'],
      0,
      -960, //We have to do this for some reason since this is an image and its 1024 instead of 64 width.. 1024 -64 = 960
    ) as Phaser.Tilemaps.TilemapLayer;
    //https://github.com/phaserjs/phaser/issues/5494
    ships_layer.setSkipCull(true);

    this.iso.createLayer(
      '2-stonebase',
      ['iso-64x64-building', 'iso-64x64-outside'],
      0,
      0,
    );

    // 2_grid_1
    const grid_two_layer = this.iso.createLayer('2-grid', ['64x64grid'], 0, 0);

    this.iso.createLayer(
      '3-wall',
      ['iso-64x64-building', 'iso-64x64-outside'],
      0,
      0,
    );
    this.iso.createLayer(
      '4-roof',
      ['iso-64x64-building', 'iso-64x64-outside'],
      0,
      0,
    );

    // 5_roof_1
    this.roof_five_layer = this.iso.createLayer(
      '5-roof',
      ['iso-64x64-building', 'iso-64x64-outside'],
      0,
      0,
    ) as Phaser.Tilemaps.TilemapLayer;

    // 5_grid_1
    const grid_five_layer = this.iso.createLayer('5-grid', ['64x64grid'], 0, 0);

    // this.aquilaInput.inject('this.roof_five_layer);
    this.aquilaInputService.addPointerDownCallback((pointer) => {
      const tile = this.roof_five_layer.getIsoTileAtWorldXY(
        pointer.worldX,
        pointer.worldY,
        false,
        false,
        this.scene.cameras.main,
      );

      if (tile) {
        tile.tint = 0xff0000;
      }
    });
    this.aquilaInputService.addPointerUpCallback((pointer) => {
      const tile = this.roof_five_layer.getIsoTileAtWorldXY(
        pointer.worldX,
        pointer.worldY,
        false,
        false,
        this.scene.cameras.main,
      );

      if (tile) {
        tile.tint = 0xffffff;
      }
    });
  }

  // override update(time: number, delta: number) {}

  // override destroy() {}
}
