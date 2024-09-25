import { AquilaInputService } from "../services/AquilaInputService";
import RAPIER from "@dimforge/rapier2d-compat";

export class UserComponent {
  scene: Phaser.Scene;
  aquilaInputService: AquilaInputService;
  rapierWorld: RAPIER.World;
  gameObject: Phaser.GameObjects.GameObject;
  constructor(
    gameObject: Phaser.GameObjects.GameObject,
    inputService: AquilaInputService,
    rapierWorld: RAPIER.World,
  ) {
    this.scene = gameObject.scene;
    this.aquilaInputService = inputService;
    this.rapierWorld = rapierWorld;
    this.gameObject = gameObject;

    const listenAwake = this.awake !== UserComponent.prototype.awake;
    const listenStart = this.start !== UserComponent.prototype.start;
    const listenUpdate = this.update !== UserComponent.prototype.update;
    const listenDestroy = this.destroy !== UserComponent.prototype.destroy;

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
      gameObject.on(Phaser.GameObjects.Events.DESTROY, () => {
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
