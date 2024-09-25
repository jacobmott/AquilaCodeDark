import { BehaviorSubject, Observable } from 'rxjs';

interface KeyData {
  key: string;
  isDown: boolean;
}

export class AquilaInputService {
  scene: Phaser.Scene;

  keyA!: Phaser.Input.Keyboard.Key;
  keyS!: Phaser.Input.Keyboard.Key;
  keyD!: Phaser.Input.Keyboard.Key;
  keyW!: Phaser.Input.Keyboard.Key;
  keyE!: Phaser.Input.Keyboard.Key;
  keyR!: Phaser.Input.Keyboard.Key;
  keyC!: Phaser.Input.Keyboard.Key;
  keyV!: Phaser.Input.Keyboard.Key;
  keyF!: Phaser.Input.Keyboard.Key;
  keyT!: Phaser.Input.Keyboard.Key;

  wDown: boolean = false;
  aDown: boolean = false;
  sDown: boolean = false;
  dDown: boolean = false;
  eDown: boolean = false;
  rDown: boolean = false;
  cDown: boolean = false;
  vDown: boolean = false;
  fDown: boolean = false;
  tDown: boolean = false;

  label: Phaser.GameObjects.Text;
  label2: Phaser.GameObjects.Text;
  pointer: Phaser.Input.Pointer;

  keys: BehaviorSubject<KeyData>;
  currentKeys: Observable<KeyData>;

  pointerUpCallbacks: { (pointerEvent: any): void }[] = [];
  pointerDownCallbacks: { (pointerEvent: any): void }[] = [];

  constructor(scene: Phaser.Scene) {
    this.scene = scene;

    // declare and initialize the quote property
    this.keys = new BehaviorSubject({ key: '', isDown: false } as KeyData);

    this.setupInputListeners();

    this.label = this.scene.add.text(512, 384, '(x, y)', {
      fontFamily: '"Monospace"',
    });
    this.label2 = this.scene.add.text(512, 384, '(x, y)', {
      fontFamily: '"Monospace"',
    });

    this.pointer = this.scene.input.activePointer;

    // expose the BehaviorSubject as an Observable
    this.currentKeys = this.keys.asObservable();
  }

  setupInputListeners() {
    this.keyA = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.A,
    );
    this.keyS = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.S,
    );
    this.keyD = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.D,
    );
    this.keyW = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.W,
    );
    this.keyE = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.E,
    );
    this.keyR = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.R,
    );
    this.keyC = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.C,
    );
    this.keyV = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.V,
    );
    this.keyF = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.F,
    );
    this.keyT = this.scene.input.keyboard!.addKey(
      Phaser.Input.Keyboard.KeyCodes.T,
    );

    //Need camera class that passes in this logic so its not here, so we decouple this input
    //class from having to know about the camera or interact with it
    this.scene.input.on(
      'wheel',
      (
        pointer: any,
        gameObjects: any,
        deltaX: number,
        deltaY: number,
        deltaZ: number,
      ) => {
        if (deltaY > 0) {
          const newZoom = this.scene.cameras.main.zoom - 0.1;
          if (newZoom > 0.00001) {
            this.scene.cameras.main.zoom = newZoom;
          }
        }

        if (deltaY < 0) {
          const newZoom = this.scene.cameras.main.zoom + 0.1;
          if (newZoom < 5) {
            this.scene.cameras.main.zoom = newZoom;
          }
        }
      },
    );

    this.scene.input.on('pointermove', (pointer: any) => {
      if (!pointer.isDown) return;
      this.scene.cameras.main.stopFollow();
      this.scene.cameras.main.scrollX -=
        (pointer.x - pointer.prevPosition.x) / this.scene.cameras.main.zoom;
      this.scene.cameras.main.scrollY -=
        (pointer.y - pointer.prevPosition.y) / this.scene.cameras.main.zoom;
    });

    this.scene.input.on('pointerup', (pointer: any) => {
      this.pointerUpCallbacks.forEach((callback) => {
        callback(pointer);
      });
    });

    this.scene.input.on('pointerdown', (pointer: any) => {
      this.pointerDownCallbacks.forEach((callback) => {
        callback(pointer);
      });
    });

    this.scene.input.on('pointermove', (pointer: any) => {
      const px = Math.trunc(pointer.x);
      const py = Math.trunc(pointer.y);
      const pwx = Math.trunc(pointer.worldX);
      const pwy = Math.trunc(pointer.worldY);
      let xOffsetAmount = 40;
      let yOffsetAmount = 40;
      if (px > this.scene.game.canvas.width / 2) {
        xOffsetAmount = -160;
      }
      if (py > this.scene.game.canvas.height / 2) {
        yOffsetAmount = -40;
      }
      this.label.setDepth(1001);
      this.label.setText('world(iso)X/Y: (' + pwx + ', ' + pwy + ')');
      this.label2.setText('pointerX/Y: (' + px + ', ' + py + ')');
      this.label2.setDepth(1001);
      this.label.x = pwx + xOffsetAmount;
      this.label.y = pwy + yOffsetAmount;
      this.label2.x = pwx + xOffsetAmount;
      this.label2.y = pwy + yOffsetAmount + 20;
    });
  }

  addPointerUpCallback(callback: (pointerEvent: any) => void) {
    this.pointerUpCallbacks.push(callback);
  }

  addPointerDownCallback(callback: (pointerEvent: any) => void) {
    this.pointerDownCallbacks.push(callback);
  }

  update() {
    const wDown = this.keyW.isDown; //upRight
    const aDown = this.keyA.isDown; //upLeft
    const sDown = this.keyS.isDown; //downLeft
    const dDown = this.keyD.isDown; //downRight
    const eDown = this.keyE.isDown; //speed increase
    const rDown = this.keyR.isDown; //speed decrease
    const cDown = this.keyC.isDown; //rotation speed increase
    const vDown = this.keyV.isDown; //rotation speed decrease
    const fDown = this.keyF.isDown; //cast shape
    const tDown = this.keyT.isDown; //turn on/off debug graphics

    //if something changed.. then notify the observers
    if (wDown !== this.wDown) {
      this.wDown = wDown;
      this.updateKeys({ key: 'w', isDown: wDown } as KeyData);
    }
    if (aDown !== this.aDown) {
      this.aDown = aDown;
      this.updateKeys({ key: 'a', isDown: aDown } as KeyData);
    }
    if (sDown !== this.sDown) {
      this.sDown = sDown;
      this.updateKeys({ key: 's', isDown: sDown } as KeyData);
    }
    if (dDown !== this.dDown) {
      this.dDown = dDown;
      this.updateKeys({ key: 'd', isDown: dDown } as KeyData);
    }
    if (eDown !== this.eDown) {
      this.eDown = eDown;
      this.updateKeys({ key: 'e', isDown: eDown } as KeyData);
    }
    if (rDown !== this.rDown) {
      this.rDown = rDown;
      this.updateKeys({ key: 'r', isDown: rDown } as KeyData);
    }
    if (cDown !== this.cDown) {
      this.cDown = cDown;
      this.updateKeys({ key: 'c', isDown: cDown } as KeyData);
    }
    if (vDown !== this.vDown) {
      this.vDown = vDown;
      this.updateKeys({ key: 'v', isDown: vDown } as KeyData);
    }
    if (fDown !== this.fDown) {
      this.fDown = fDown;
      this.updateKeys({ key: 'f', isDown: fDown } as KeyData);
    }
    if (tDown !== this.tDown) {
      this.tDown = tDown;
      this.updateKeys({ key: 't', isDown: tDown } as KeyData);
    }
  }

  updateKeys(key: KeyData) {
    this.keys.next(key);
  }

  getKeys(): Observable<KeyData> {
    return this.currentKeys;
  }
}
