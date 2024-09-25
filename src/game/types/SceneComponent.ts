import { AquilaInputService } from "../services/AquilaInputService";

export class SceneComponent {
  scene: Phaser.Scene;
  aquilaInputService: AquilaInputService;

  constructor(scene: Phaser.Scene, inputService: AquilaInputService) {
    this.scene = scene;
    this.aquilaInputService = inputService;

    const listenAwake = this.awake !== SceneComponent.prototype.awake;
    const listenStart = this.start !== SceneComponent.prototype.start;
    const listenUpdate = this.update !== SceneComponent.prototype.update;
    const listenDestroy = this.destroy !== SceneComponent.prototype.destroy;

    if (listenAwake) {
      this.scene.events.once("scene-awake", this.awake, this);
    }

    if (listenStart) {
      this.scene.events.once(Phaser.Scenes.Events.UPDATE, this.start, this);
    }

    if (listenUpdate) {
      this.scene.events.on(Phaser.Scenes.Events.UPDATE, this.update, this);
    }

    if (listenStart || listenUpdate || listenDestroy) {
      this.scene.events.on(Phaser.Scenes.Events.DESTROY, () => {
        this.scene.events.off(Phaser.Scenes.Events.UPDATE, this.start, this);
        this.scene.events.off(Phaser.Scenes.Events.UPDATE, this.update, this);

        if (listenDestroy) {
          this.destroy();
        }
      });
    }
  }

  awake() {}

  start() {}

  update(time: number, delta: number) {}

  updateManually(time: number, delta: number) {}

  destroy() {}
}
