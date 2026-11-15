import * as THREE from 'three';
import {
  createShip,
  createSky,
  createStars,
  createStarlightEnvironment,
  disposeTree,
} from './universe-art';
import {
  distance,
  FLEET,
  FLAGSHIP,
  HANGARS,
  hullRadius,
  FlightSave,
  HOME_WORLD,
  nearbySectors,
  nearestPlanet,
  Planet,
  Preferences,
  sectorAt,
  sectorKey,
  sectorPlanet,
  Vec3,
  Vessel,
  View,
  QUALITY_BUDGETS,
  orbitAltitude,
  arrivalAltitude,
  descentRange,
  sphereContact,
  flightPath,
} from './universe-model';
import {
  createPlanet,
  configureShaderDepth,
  updatePlanet,
} from './planet-visuals';
import { PlanetTerrain } from './planet-terrain';
import { terrainHeight } from './planet-fields';
import { ShipInterior } from './ship-interior';
import { deckInteraction } from './interior-model';
import {
  PracticeBattle,
  CombatFrame,
  TURRET_PIVOTS,
  BARREL_LENGTH,
  MAX_IMPACTS,
  MAX_PROJECTILES,
} from './combat-model';
import { CombatArt } from './combat-art';
import { combatDestination, HelmOrder, turnToward } from './navigation-model';

export interface FlightState {
  position: Vec3;
  sector: string;
  speed: number;
  altitude: number;
  nearest: Planet;
  distance: number;
  planets: Planet[];
  canDescend: boolean;
  surface: boolean;
  paused: boolean;
  course: string | null;
  navigation: { order: HelmOrder; status: string; canOrder: boolean } | null;
  saved: boolean;
  descending: boolean;
  inspecting: boolean;
  sortie: boolean;
  visitedInterior: boolean;
  interior: {
    mode: 'walk' | 'helm' | 'tactical';
    room: string;
    interaction: 'helm' | 'tactical' | 'shuttle' | null;
    mouseLook: boolean;
  } | null;
  shuttleMarker: { x: number; y: number } | null;
  practiceNotice: string;
  combat: {
    status: PracticeBattle['status'];
    locked: boolean;
    mode: PracticeBattle['mode'];
    shield: number;
    hull: number;
    targetShield: number;
    targetHull: number;
    range: number;
    cooldown: number;
    canChangeMode: boolean;
    reason: string;
    fired: number;
    received: number;
    ammunition: PracticeBattle['ammunition'];
    lastImpact: string;
    armorKnown: boolean;
    activeProjectiles: number;
    marker: { x: number; y: number; offscreen: boolean } | null;
  } | null;
}
interface WorldNode {
  planet: Planet;
  object: THREE.Group;
}
interface Course {
  path: THREE.Vector3[];
  lengths: number[];
  length: number;
  elapsed: number;
  label: string;
  lookAt: Vec3;
}

export class UniverseScene {
  private readonly scene = new THREE.Scene();
  private readonly camera = new THREE.PerspectiveCamera(48, 1, 0.5, 2000000);
  private readonly renderer: THREE.WebGLRenderer;
  private readonly space = new THREE.Group();
  private readonly sky = createSky();
  private readonly stars = createStars();
  private readonly reflections = createStarlightEnvironment();
  private readonly worlds = new Map<string, WorldNode>();
  private readonly traffic: { object: THREE.Group; position: THREE.Vector3 }[] =
    [];
  private readonly position = new THREE.Vector3();
  private readonly velocity = new THREE.Vector3();
  private readonly keys = new Set<string>();
  private readonly cleanup: (() => void)[] = [];
  private readonly resizeObserver: ResizeObserver;
  private viewport = { width: 0, height: 0, pixelRatio: 0 };
  private ship: THREE.Group;
  private shuttle: THREE.Group;
  private vessel: Vessel;
  private preferences: Preferences;
  private view: View = 'home';
  private yaw = 0;
  private pitch = 0;
  private throttle = 0;
  private frame = 0;
  private previous = 0;
  private elapsed = 0;
  private lastEmit = 0;
  private lastSave = 0;
  private loadedSector = '';
  private worldQueue: Planet[] = [];
  private terrain: PlanetTerrain | null = null;
  private terrainPrepared = false;
  private readonly frameTimes: number[] = [];
  private frameWorkMs = 0;
  private peakFrameWorkMs = 0;
  private drag: {
    x: number;
    y: number;
    id: number;
    mode: 'steer' | 'orbit';
  } | null = null;
  private readonly orbit = { yaw: 0, pitch: 0, focus: 0 };
  private chordRelease = false;
  private paused = false;
  private lost = false;
  private disposed = false;
  private saved = false;
  private interior: ShipInterior | null = null;
  private aboard: 'walk' | 'helm' | 'tactical' | null = null;
  private bridgeReturn: 'walk' | 'helm' | 'tactical' = 'walk';
  private combat: PracticeBattle | null = null;
  private combatArt: CombatArt | null = null;
  private practiceNotice = '';
  private combatSector = '';
  private combatPlanets: Planet[] = [];
  private intentionalUnlock = false;
  private course: Course | null = null;
  private helmOrder: HelmOrder = 'hold';
  private helmStatus = 'Holding position';
  private surface: {
    radius: number;
    planet: Planet;
    normal: THREE.Vector3;
    forward: THREE.Vector3;
    altitude: number;
    descent: number;
    entryAltitude: number;
  } | null = null;
  private inspection: {
    position: THREE.Vector3;
    yaw: number;
    pitch: number;
    fromHangar?: boolean;
  } | null = null;
  private readonly inspectionRay = new THREE.Raycaster();
  private readonly orientation = new THREE.Quaternion();
  private readonly targetCamera = new THREE.Vector3();
  private readonly targetLook = new THREE.Vector3();

  constructor(
    private readonly host: HTMLElement,
    preferences: Preferences,
    save: FlightSave | null,
    private readonly onState: (state: FlightState) => void,
    private readonly onError: (message: string | null) => void,
  ) {
    this.preferences = { ...preferences };
    this.vessel = FLEET.find((v) => v.id === save?.vessel) ?? FLAGSHIP;
    if (save) {
      this.position.fromArray(save.position);
      [this.yaw, this.pitch] = save.rotation;
      this.saved = true;
    }
    const canvas = document.createElement('canvas');
    const context = canvas.getContext('webgl2', {
      antialias: true,
      alpha: false,
      powerPreference: 'high-performance',
    });
    if (!context) throw new Error('WebGL 2 unavailable');
    const reversed = !!context.getExtension('EXT_clip_control');
    this.renderer = new THREE.WebGLRenderer({
      canvas,
      context,
      antialias: true,
      alpha: false,
      powerPreference: 'high-performance',
      logarithmicDepthBuffer: !reversed,
      reversedDepthBuffer: reversed,
    });
    this.renderer.outputColorSpace = THREE.SRGBColorSpace;
    this.renderer.toneMapping = THREE.ACESFilmicToneMapping;
    this.renderer.toneMappingExposure = 1.25;
    this.renderer.info.autoReset = false;
    canvas.setAttribute(
      'aria-label',
      'Aquila universe. Command the battleship with navigation orders; drag to look around. Shuttles use W and S for thrust, left drag or arrows to steer, E and Q for height. Aboard, WASD walks and F uses a station. Escape pauses.',
    );
    canvas.tabIndex = 0;
    host.appendChild(canvas);
    this.scene.add(this.sky, this.stars, this.space);
    this.sky.renderOrder = -1000;
    this.stars.renderOrder = -999;
    this.scene.environment = this.reflections;
    this.scene.environmentIntensity = 0.8;
    this.scene.add(new THREE.HemisphereLight(0xb6dbea, 0x1b202e, 1));
    const sun = new THREE.DirectionalLight(0xffeed8, 3);
    sun.position.set(-800, 650, 1000);
    this.scene.add(sun);
    const rim = new THREE.DirectionalLight(0x6c9edb, 1.5);
    rim.position.set(500, 200, -800);
    this.scene.add(rim);
    this.ship = createShip(this.vessel);
    this.scene.add(this.ship);
    this.shuttle = createShip({
      ...(this.vessel.capital ? FLEET[0] : this.vessel),
      color: 0xc9c6b5,
      accent: 0xef936a,
    });
    this.shuttle.visible = false;
    this.scene.add(this.shuttle);
    for (const [index, position] of [
      [1, [-160, 55, -260]],
      [2, [230, -100, -470]],
    ] as const) {
      const object = createShip(FLEET[index]);
      object.rotation.y = 0.35;
      this.space.add(object);
      this.traffic.push({ object, position: new THREE.Vector3(...position) });
    }
    this.camera.position.set(30, 60, 310);
    this.camera.lookAt(80, 15, -550);
    configureShaderDepth(this.scene);
    this.enforceOrbit(this.position.clone());
    this.resizeObserver = new ResizeObserver(() => this.resize());
    this.resizeObserver.observe(host);
    this.resize();
    this.listen(window, 'keydown', (event: Event) =>
      this.key(event as KeyboardEvent, true),
    );
    this.listen(window, 'keyup', (event: Event) =>
      this.key(event as KeyboardEvent, false),
    );
    this.listen(window, 'blur', () => {
      if (this.view === 'play') this.setPaused(true);
    });
    this.listen(document, 'visibilitychange', () => {
      if (document.hidden && this.view === 'play') this.setPaused(true);
    });
    this.listen(window, 'pagehide', () => this.save());
    // Keep browser menus on the HTML interface; only flight owns canvas clicks.
    this.listen(canvas, 'contextmenu', (event: Event) => {
      if (this.view === 'play') event.preventDefault();
    });
    this.listen(canvas, 'pointerdown', (event: Event) => {
      const e = event as PointerEvent;
      if (
        (e.button !== 0 && e.button !== 2) ||
        this.view !== 'play' ||
        this.paused ||
        this.lost
      )
        return;
      e.preventDefault();
      canvas.focus({ preventScroll: true });
      if (document.pointerLockElement === canvas) return;
      // Capture before requesting lock: Chrome can grant lock immediately and reject capture afterward.
      canvas.setPointerCapture(e.pointerId);
      this.drag = {
        x: e.clientX,
        y: e.clientY,
        id: e.pointerId,
        mode: this.capitalCommand() || e.buttons & 2 ? 'orbit' : 'steer',
      };
      if (
        this.aboard === 'walk' &&
        e.pointerType === 'mouse' &&
        e.button === 0
      ) {
        void this.captureLook();
      }
    });
    this.listen(canvas, 'pointermove', (event: Event) => {
      const e = event as PointerEvent;
      if (document.pointerLockElement === canvas) return;
      if (!this.drag || e.pointerId !== this.drag.id || this.paused) return;
      if ((e.buttons & 3) === 0) {
        this.clearPointer();
        return;
      }
      const dx = e.clientX - this.drag.x,
        dy = e.clientY - this.drag.y;
      this.chordRelease =
        (e.button === 2 && e.buttons === 1) ||
        (e.button === 0 && e.buttons === 2);
      this.drag = {
        x: e.clientX,
        y: e.clientY,
        id: e.pointerId,
        mode: this.capitalCommand() || e.buttons & 2 ? 'orbit' : 'steer',
      };
      if (this.aboard === 'walk' || this.aboard === 'tactical') {
        this.interior?.look(dx, dy, this.preferences.sensitivity);
        return;
      }
      if (this.drag.mode === 'orbit') {
        const yaw = this.orbit.yaw - dx * 0.005 * this.preferences.sensitivity;
        this.orbit.yaw = Math.atan2(Math.sin(yaw), Math.cos(yaw));
        this.orbit.pitch = THREE.MathUtils.clamp(
          this.orbit.pitch + dy * 0.004 * this.preferences.sensitivity,
          -1.5,
          1.1,
        );
        return;
      }
      if (this.surface) {
        if (this.surface.descent < 4) return;
        this.surface.forward.applyAxisAngle(
          this.surface.normal,
          -dx * 0.003 * this.preferences.sensitivity,
        );
        this.surface.altitude = THREE.MathUtils.clamp(
          this.surface.altitude - dy * 0.7,
          80,
          6000,
        );
      } else {
        const steering =
          this.preferences.sensitivity *
          (this.inspection ? FLEET[0] : this.vessel).handling;
        this.yaw -= dx * 0.003 * steering;
        this.pitch = THREE.MathUtils.clamp(
          this.pitch - dy * 0.003 * steering,
          -1.48,
          1.48,
        );
      }
      this.course = null;
    });
    // Pointer events report only the first button down and last button up.
    // Mouse events also catch a right-button release while left remains held.
    const syncButtons = (event: Event) => {
      const e = event as MouseEvent;
      if (!this.drag) return;
      if ((e.buttons & 3) === 0) this.clearPointer();
      else
        this.drag.mode =
          this.capitalCommand() || e.buttons & 2 ? 'orbit' : 'steer';
    };
    this.listen(canvas, 'mousedown', syncButtons);
    this.listen(window, 'mouseup', syncButtons);
    this.listen(document, 'mousemove', (event: Event) => {
      if (
        document.pointerLockElement === canvas &&
        this.aboard === 'walk' &&
        !this.paused &&
        this.view === 'play'
      ) {
        const e = event as MouseEvent;
        this.interior?.look(
          e.movementX,
          e.movementY,
          this.preferences.sensitivity,
        );
      }
    });
    this.listen(document, 'pointerlockchange', () => {
      // The browser may move the hidden cursor. Never reuse an absolute drag origin across lock changes.
      this.clearPointer(true);
      if (document.pointerLockElement === canvas) {
        // A delayed browser grant must not capture the cursor after a route or mode change.
        if (this.view !== 'play' || this.paused || this.aboard !== 'walk')
          this.releaseLook();
      } else {
        const intentional = this.intentionalUnlock;
        this.intentionalUnlock = false;
        if (!intentional && this.aboard && this.view === 'play' && !this.paused)
          this.setPaused(true);
      }
      this.emit();
    });
    this.listen(canvas, 'pointerup', () => this.clearPointer());
    this.listen(canvas, 'pointercancel', () => this.clearPointer());
    this.listen(canvas, 'lostpointercapture', (event: Event) => {
      const e = event as PointerEvent;
      // Chrome can drop capture when either button releases during a chord.
      // Preserve the remaining button only for that transition; other losses end the drag.
      if (this.drag && this.chordRelease && e.buttons & 3 && !this.paused) {
        this.chordRelease = false;
        this.drag.x = e.clientX;
        this.drag.y = e.clientY;
        canvas.setPointerCapture(this.drag.id);
      } else this.drag = null;
    });
    this.listen(canvas, 'webglcontextlost', (event: Event) => {
      event.preventDefault();
      this.lost = true;
      this.setPaused(true);
      this.onError(
        'Graphics paused. Your browser lost the 3D connection. Waiting for it to recover.',
      );
    });
    this.listen(canvas, 'webglcontextrestored', () => {
      this.lost = false;
      this.onError(null);
      this.previous = 0;
    });
    this.stream(new THREE.Vector3());
    this.emit();
    // Precompile without blocking input while supported drivers finish their programs.
    void this.renderer.compileAsync(this.scene, this.camera).then(() => {
      if (!this.disposed)
        this.frame = requestAnimationFrame((time) => this.tick(time));
    });
  }

  private listen(
    target: EventTarget,
    type: string,
    callback: EventListener,
  ): void {
    target.addEventListener(type, callback);
    this.cleanup.push(() => target.removeEventListener(type, callback));
  }
  private resize(): void {
    const width = Math.max(1, this.host.clientWidth),
      height = Math.max(1, this.host.clientHeight);
    const profile = QUALITY_BUDGETS[this.preferences.quality];
    const pixelRatio = Math.min(
      window.devicePixelRatio || 1,
      profile.pixelRatio,
      Math.sqrt(profile.maxPixels / (width * height)),
    );
    if (
      width === this.viewport.width &&
      height === this.viewport.height &&
      pixelRatio === this.viewport.pixelRatio
    )
      return;
    // Update the buffer once. setPixelRatio followed by setSize resets it twice.
    this.renderer.setDrawingBufferSize(width, height, pixelRatio);
    this.viewport = { width, height, pixelRatio };
    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();
  }
  setPreferences(preferences: Preferences): void {
    const qualityChanged = this.preferences.quality !== preferences.quality;
    this.preferences = { ...preferences };
    if (qualityChanged) {
      this.resize();
      this.terrain?.setQuality(preferences.quality);
    }
  }
  setView(view: View): void {
    if (this.view === 'play' && view !== 'play') this.save();
    this.view = view;
    this.releaseLook();
    this.keys.clear();
    this.clearPointer(true);
    this.velocity.set(0, 0, 0);
    this.throttle = 0;
    if (!this.capitalCommand()) this.course = null;
    this.paused = false;
    // The surface excursion stays intact while settings or the atlas are open.
    this.emit();
  }
  setShip(id: string): void {
    const vessel = FLEET.find((v) => v.id === id);
    if (!vessel || vessel.id === this.vessel.id) return;
    this.endPractice();
    this.holdPosition();
    this.clearPointer(true);
    if (this.inspection) this.resumeCommand();
    if (this.surface) this.returnToOrbit();
    this.releaseLook();
    this.aboard = null;
    this.bridgeReturn = 'walk';
    this.interior?.atBridge();
    this.scene.remove(this.ship);
    disposeTree(this.ship);
    this.vessel = vessel;
    this.ship = createShip(vessel);
    this.scene.add(this.ship);
    this.scene.remove(this.shuttle);
    disposeTree(this.shuttle);
    this.shuttle = createShip(vessel.capital ? FLEET[0] : vessel);
    this.shuttle.visible = false;
    this.scene.add(this.shuttle);
    this.enforceOrbit(this.position.clone());
    this.camera.position.set(0, vessel.length, vessel.length * 3);
    this.save();
    this.emit();
  }
  focus(): void {
    this.renderer.domElement.focus({ preventScroll: true });
  }
  setPaused(value: boolean): void {
    if (this.lost && !value) return;
    this.paused = value;
    this.releaseLook();
    this.keys.clear();
    this.clearPointer(true);
    if (!this.capitalCommand()) this.course = null;
    this.throttle = 0;
    this.velocity.set(0, 0, 0);
    if (value) this.save();
    this.emit();
  }
  input(code: string, down: boolean): void {
    if (this.view !== 'play' || this.paused) return;
    // Retain manual flight for shuttles. Battleship inputs belong to walking and station look only.
    if (
      this.capitalCommand() &&
      this.aboard !== 'walk' &&
      !(this.aboard === 'tactical' && code.startsWith('Arrow'))
    )
      return;
    down ? this.keys.add(code) : this.keys.delete(code);
    if (down && !this.capitalCommand()) this.course = null;
  }
  private key(event: KeyboardEvent, down: boolean): void {
    if (!down) {
      this.keys.delete(event.code);
      return;
    }
    if (this.view !== 'play' || event.ctrlKey || event.altKey || event.metaKey)
      return;
    const target = event.target as HTMLElement | null;
    if (event.code === 'Escape') {
      event.preventDefault();
      if (!event.repeat) this.setPaused(!this.paused);
      return;
    }
    if (target?.closest('input,select,textarea,button,a,[data-ui]')) return;
    if (!this.paused && event.code === 'KeyF' && this.aboard) {
      event.preventDefault();
      if (!event.repeat) this.interactAboard();
      return;
    }
    if (!this.paused && event.code === 'KeyC' && this.aboard) {
      event.preventDefault();
      if (!event.repeat) this.exteriorView();
      return;
    }
    if (
      [
        'KeyW',
        'KeyS',
        'KeyA',
        'KeyD',
        'KeyQ',
        'KeyE',
        'ArrowUp',
        'ArrowDown',
        'ArrowLeft',
        'ArrowRight',
        'ShiftLeft',
        'ShiftRight',
        'Space',
      ].includes(event.code)
    ) {
      event.preventDefault();
      this.input(event.code, true);
    }
  }
  private clearPointer(resetOrbit = false): void {
    const pointer = this.drag;
    this.drag = null;
    this.chordRelease = false;
    if (pointer && this.renderer.domElement.hasPointerCapture(pointer.id))
      this.renderer.domElement.releasePointerCapture(pointer.id);
    if (resetOrbit) this.orbit.yaw = this.orbit.pitch = this.orbit.focus = 0;
  }

  private async captureLook(): Promise<void> {
    const canvas = this.renderer.domElement;
    // The pinned DOM typings predate raw mouse options and the promise return value.
    const request = canvas.requestPointerLock as (options?: {
      unadjustedMovement: boolean;
    }) => Promise<void> | void;
    try {
      await request.call(canvas, { unadjustedMovement: true });
    } catch (error) {
      if (
        (error as { name?: string })?.name === 'NotSupportedError' &&
        this.aboard === 'walk' &&
        this.view === 'play' &&
        !this.paused &&
        !this.disposed
      ) {
        try {
          await request.call(canvas);
        } catch {
          /* Drag and arrows remain available if mouse lock is denied. */
        }
      }
    }
  }
  private releaseLook(): void {
    if (document.pointerLockElement === this.renderer.domElement) {
      this.intentionalUnlock = true;
      document.exitPointerLock();
    }
  }
  enterBridge(): void {
    if (!this.vessel.capital || this.surface || this.inspection) return;
    this.setPaused(false);
    this.interior ??= new ShipInterior(this.reflections);
    this.aboard = this.bridgeReturn;
    if (this.aboard === 'helm') this.interior.atHelm();
    if (this.aboard === 'tactical') this.interior.atTactical();
    this.save();
    this.emit();
  }
  exteriorView(): void {
    if (!this.aboard) return;
    this.bridgeReturn = this.aboard;
    this.aboard = null;
    this.setPaused(false);
    this.camera.position
      .set(0, this.vessel.length * 0.5, this.vessel.length * 1.35)
      .applyQuaternion(this.orientation);
    this.emit();
  }
  interactAboard(): void {
    if (this.paused || !this.aboard || !this.interior) return;
    if (this.aboard === 'helm' || this.aboard === 'tactical') {
      const station = this.aboard;
      this.aboard = 'walk';
      station === 'helm' ? this.interior.atHelm() : this.interior.atTactical();
      this.setPaused(false);
    } else {
      const action = deckInteraction(this.interior.point);
      if (action === 'helm') {
        this.aboard = 'helm';
        this.interior.atHelm();
        this.setPaused(false);
      } else if (action === 'tactical') {
        this.aboard = 'tactical';
        this.interior.atTactical();
        this.setPaused(false);
      } else if (action === 'shuttle') {
        // A launch order parks the capital before moving control into its shuttle.
        this.holdPosition();
        this.aboard = null;
        this.inspectHull();
        if (this.inspection) this.inspection.fromHangar = true;
        // Face outward through the starboard launch aperture.
        const outward = new THREE.Vector3(1, 0, 0).applyQuaternion(
          new THREE.Quaternion().setFromEuler(
            new THREE.Euler(
              this.inspection?.pitch ?? 0,
              this.inspection?.yaw ?? 0,
              0,
              'YXZ',
            ),
          ),
        );
        this.yaw = Math.atan2(-outward.x, -outward.z);
        this.pitch = Math.asin(outward.y);
      }
    }
    this.emit();
  }
  returnToHangar(): void {
    if (!this.inspection?.fromHangar) return;
    this.resumeCommand();
    this.interior ??= new ShipInterior(this.reflections);
    this.interior.atHangar();
    this.aboard = 'walk';
    this.bridgeReturn = 'walk';
    this.save();
    this.emit();
  }

  private combatControl(): boolean {
    return (
      this.view === 'play' &&
      !this.paused &&
      !this.lost &&
      this.vessel.capital === true &&
      !this.surface &&
      !this.inspection &&
      this.aboard !== 'walk'
    );
  }
  private practiceWorlds(): Planet[] {
    const sector = sectorKey(sectorAt(this.position.toArray()));
    if (sector !== this.combatSector) {
      this.combatSector = sector;
      this.combatPlanets = nearbySectors(this.position.toArray()).map(
        sectorPlanet,
      );
    }
    return this.combatPlanets;
  }
  private combatFrame(): CombatFrame {
    const rotation = new THREE.Quaternion().setFromEuler(
      new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
    );
    const target = new THREE.Vector3(
      ...(this.combat?.target ?? (this.position.toArray() as Vec3)),
    );
    const muzzles = TURRET_PIVOTS.map((at) => {
      const pivot = new THREE.Vector3(...at)
        .applyQuaternion(rotation)
        .add(this.position);
      return pivot
        .add(
          target.clone().sub(pivot).normalize().multiplyScalar(BARREL_LENGTH),
        )
        .toArray() as Vec3;
    }) as [Vec3, Vec3];
    return {
      position: this.position.toArray() as Vec3,
      rotation: rotation.toArray() as [number, number, number, number],
      velocity: this.velocity.toArray() as Vec3,
      muzzles,
      obstacles: this.practiceWorlds(),
      clear: !this.practiceWorlds().some((p) =>
        muzzles.some(
          (muzzle) =>
            sphereContact(
              muzzle,
              target.toArray(),
              p.position,
              p.radius + 1000,
            ) !== null,
        ),
      ),
    };
  }
  startPractice(): void {
    if (!this.combatControl()) return;
    this.holdPosition();
    const rotation = new THREE.Quaternion().setFromEuler(
      new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
    );
    const offsets = [
      [5000, 2500, -26000],
      [0, 23000, -23000],
      [24000, 9000, -16000],
      [-24000, 9000, -16000],
      [0, 0, 30000],
    ];
    const target = offsets
      .map((offset) =>
        new THREE.Vector3(...offset)
          .applyQuaternion(rotation)
          .add(this.position),
      )
      .find(
        (point) =>
          !this.practiceWorlds().some(
            (p) =>
              sphereContact(
                this.position.toArray(),
                point.toArray(),
                p.position,
                p.radius + 2000,
              ) !== null,
          ),
      );
    if (!target) {
      this.practiceNotice = 'Move into open space to begin practice.';
      this.emit();
      return;
    }
    this.setPaused(false);
    this.practiceNotice = '';
    this.combat = new PracticeBattle(target.toArray() as Vec3);
    if (!this.combatArt) {
      this.combatArt = new CombatArt();
      this.scene.add(this.combatArt.root);
    }
    this.emit();
  }
  setWeaponMode(mode: PracticeBattle['mode']): void {
    if (!this.combatControl() || !this.combat) return;
    this.combat.setMode(mode);
    this.emit();
  }
  setAmmunition(ammo: PracticeBattle['ammunition']): void {
    if (!this.combatControl() || !this.combat) return;
    this.combat.setAmmunition(ammo);
    this.emit();
  }
  endPractice(): void {
    if (!this.combat) return;
    this.combat = null;
    if (this.helmOrder === 'range' || this.helmOrder === 'withdraw')
      this.holdPosition();
    if (this.combatArt) this.combatArt.root.visible = false;
    this.emit();
  }
  private stepCombat(dt: number): void {
    if (!this.combat) return;
    const before = this.combat.status;
    this.combat.step(dt, this.combatFrame());
    if (
      before !== this.combat.status &&
      (this.combat.status === 'defeat' || this.combat.status === 'victory')
    )
      this.holdPosition(
        this.combat.status === 'defeat'
          ? 'Ship disabled. Navigation stopped.'
          : 'Target disabled. Holding position.',
      );
  }
  private combatState(): FlightState['combat'] {
    const battle = this.combat;
    if (!battle) return null;
    const frame = this.combatFrame();
    const projected = new THREE.Vector3(...battle.target)
      .sub(this.position)
      .project(this.camera);
    const behind =
      new THREE.Vector3(...battle.target)
        .sub(this.position)
        .applyMatrix4(this.camera.matrixWorldInverse).z >= 0;
    const offscreen =
      behind || Math.abs(projected.x) > 0.88 || Math.abs(projected.y) > 0.8;
    const reason = battle.reason(frame);
    return {
      status: battle.status,
      locked: battle.locked,
      mode: battle.mode,
      shield: battle.player.shield,
      hull: battle.player.hull,
      targetShield: battle.drone.shield,
      targetHull: battle.drone.hull,
      range: Math.round(battle.range(frame)),
      cooldown: battle.cooldown,
      canChangeMode:
        this.combatControl() &&
        (battle.status === 'ready' || battle.status === 'engaged'),
      reason,
      fired: battle.fired,
      received: battle.received,
      ammunition: battle.ammunition,
      lastImpact: battle.lastImpact,
      armorKnown: battle.armorKnown,
      activeProjectiles: battle.activeProjectiles,
      marker:
        battle.status === 'victory'
          ? null
          : {
              x: behind
                ? this.viewport.width * 0.5
                : THREE.MathUtils.clamp(
                    ((projected.x + 1) * this.viewport.width) / 2,
                    70,
                    this.viewport.width - 70,
                  ),
              y: behind
                ? this.viewport.height * 0.35
                : THREE.MathUtils.clamp(
                    ((1 - projected.y) * this.viewport.height) / 2,
                    100,
                    this.viewport.height - 180,
                  ),
              offscreen,
            },
    };
  }

  private orbitCamera(rotation: THREE.Quaternion, dt: number): boolean {
    const held = this.drag?.mode === 'orbit';
    const amount = this.preferences.reducedMotion ? 1 : 1 - Math.exp(-dt * 8);
    if (!held) {
      this.orbit.yaw *= 1 - amount;
      this.orbit.pitch *= 1 - amount;
      if (Math.abs(this.orbit.yaw) + Math.abs(this.orbit.pitch) < 0.0001)
        this.orbit.yaw = this.orbit.pitch = 0;
    }
    this.orbit.focus = THREE.MathUtils.lerp(
      this.orbit.focus,
      held ? 1 : 0,
      amount,
    );
    if (this.orbit.focus < 0.0001) this.orbit.focus = 0;
    const radius = this.targetCamera.length();
    const elevation = THREE.MathUtils.clamp(
      Math.atan2(this.targetCamera.y, this.targetCamera.z) + this.orbit.pitch,
      -1.35,
      1.35,
    );
    this.targetCamera
      .set(
        Math.sin(this.orbit.yaw) * Math.cos(elevation) * radius,
        Math.sin(elevation) * radius,
        Math.cos(this.orbit.yaw) * Math.cos(elevation) * radius,
      )
      .applyQuaternion(rotation);
    this.targetLook
      .multiplyScalar(1 - this.orbit.focus)
      .applyQuaternion(rotation);
    // Return along the same sphere. A linear chord from the bow would cut through the hull.
    return (
      held ||
      this.orbit.focus > 0 ||
      this.orbit.yaw !== 0 ||
      this.orbit.pitch !== 0
    );
  }

  travelTo(planet: Planet): void {
    this.endPractice();
    if (this.inspection) this.resumeCommand();
    if (this.surface) this.returnToOrbit();
    if (!this.vessel.capital) this.aboard = null;
    this.setPaused(false);
    this.holdPosition();
    const toward = this.position
      .clone()
      .sub(new THREE.Vector3(...planet.position));
    if (toward.length() < 1) toward.set(0, 0, 1);
    const end = new THREE.Vector3(...planet.position).add(
      toward
        .normalize()
        .multiplyScalar(
          planet.radius + arrivalAltitude(planet, hullRadius(this.vessel)),
        ),
    );
    this.helmOrder = 'approach';
    this.helmStatus = `Approaching ${planet.name}`;
    this.plotCourse(end, planet.position, planet.name);
    this.emit();
  }
  private capitalCommand(): boolean {
    return !!this.vessel.capital && !this.surface && !this.inspection;
  }
  private holdPosition(status = 'Holding position'): void {
    this.course = null;
    this.helmOrder = 'hold';
    this.helmStatus = status;
    this.velocity.set(0, 0, 0);
    this.throttle = 0;
  }
  setHelmOrder(order: 'hold' | 'range' | 'withdraw'): void {
    if (!this.combatControl() || this.combat?.status === 'defeat') return;
    if (order !== 'hold' && (!this.combat || this.combat.status === 'victory'))
      return;
    this.holdPosition();
    this.helmOrder = order;
    this.helmStatus =
      order === 'range'
        ? 'Keeping 35 km combat distance'
        : order === 'withdraw'
        ? 'Withdrawing to 85 km'
        : 'Holding position';
    this.updateHelm();
    this.emit();
  }
  private updateHelm(): void {
    if (
      this.course ||
      !this.combat ||
      (this.helmOrder !== 'range' && this.helmOrder !== 'withdraw')
    )
      return;
    const end = combatDestination(
      this.position.toArray() as Vec3,
      this.combat.target,
      this.helmOrder === 'withdraw',
    );
    if (end)
      this.plotCourse(
        new THREE.Vector3(...end),
        this.combat.target,
        this.helmOrder === 'range' ? 'Combat distance' : 'Withdrawal',
      );
    else
      this.helmStatus =
        this.helmOrder === 'withdraw'
          ? 'Outside firing range. Holding position.'
          : 'Keeping 35 km combat distance';
  }
  private plotCourse(end: THREE.Vector3, lookAt: Vec3, label: string): void {
    const cells = new Map(
      [
        ...nearbySectors(this.position.toArray()),
        ...nearbySectors(end.toArray()),
        ...nearbySectors(this.position.clone().lerp(end, 0.5).toArray()),
      ].map((c) => [sectorKey(c), c]),
    );
    const path = flightPath(
      this.position.toArray() as Vec3,
      end.toArray() as Vec3,
      [...cells.values()].map(sectorPlanet),
      hullRadius(this.vessel),
    ).map((p) => new THREE.Vector3(...p));
    const lengths = path.slice(1).map((p, i) => p.distanceTo(path[i]));
    this.course = {
      path,
      lengths,
      length: lengths.reduce((a, b) => a + b, 0),
      elapsed: 0,
      label,
      lookAt: [...lookAt],
    };
  }
  descend(): void {
    const state = this.state();
    if (!state.canDescend || this.surface || this.inspection || this.aboard)
      return;
    this.endPractice();
    this.holdPosition();
    this.save();
    this.setPaused(false);
    const normal = this.position
      .clone()
      .sub(new THREE.Vector3(...state.nearest.position))
      .normalize();
    const forward = new THREE.Vector3(0, 0, -1).applyQuaternion(
      this.orientation,
    );
    forward.addScaledVector(normal, -forward.dot(normal));
    if (forward.lengthSq() < 0.01)
      forward.crossVectors(normal, new THREE.Vector3(0, 1, 0));
    if (forward.lengthSq() < 0.01) forward.set(1, 0, 0);
    forward.normalize();
    const entryAltitude =
      state.distance -
      terrainHeight(normal.x, normal.y, normal.z, state.nearest.seed);
    this.surface = {
      radius: state.nearest.radius,
      planet: state.nearest,
      normal,
      forward,
      altitude: this.preferences.reducedMotion ? 350 : entryAltitude,
      descent: this.preferences.reducedMotion ? 4 : 0,
      entryAltitude,
    };
    this.camera.position.set(0, 0, 0);
    this.velocity.set(0, 0, 0);
    this.throttle = 0;
    this.emit();
  }
  returnToOrbit(): void {
    if (!this.surface) return;
    this.surface = null;
    this.scene.fog = null;
    this.scene.background = null;
    this.ship.scale.setScalar(1);
    this.setPaused(false);
    this.emit();
  }
  inspectHull(): void {
    if (!this.vessel.capital || this.surface || this.inspection || this.course)
      return;
    this.endPractice();
    this.holdPosition();
    this.setPaused(false);
    this.aboard = null;
    this.save();
    this.inspection = {
      position: this.position.clone(),
      yaw: this.yaw,
      pitch: this.pitch,
    };
    const rotation = new THREE.Quaternion().setFromEuler(
      new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
    );
    // Start outside the starboard mouth, facing the open gallery. This is inspection, not a boarding simulation.
    this.position.add(
      new THREE.Vector3(...HANGARS[1].approach).applyQuaternion(rotation),
    );
    const direction = new THREE.Vector3(-1, 0, 0).applyQuaternion(rotation);
    this.yaw = Math.atan2(-direction.x, -direction.z);
    this.pitch = Math.asin(direction.y);
    this.camera.position
      .set(0, 55, 220)
      .applyQuaternion(
        new THREE.Quaternion().setFromEuler(
          new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
        ),
      );
    this.emit();
  }
  resumeCommand(): void {
    if (!this.inspection) return;
    this.position.copy(this.inspection.position);
    this.yaw = this.inspection.yaw;
    this.pitch = this.inspection.pitch;
    this.inspection = null;
    this.setPaused(false);
    this.camera.position
      .set(0, this.vessel.length * 0.5, this.vessel.length * 1.35)
      .applyQuaternion(
        new THREE.Quaternion().setFromEuler(
          new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
        ),
      );
    this.save();
    this.emit();
  }
  resetVoyage(): void {
    this.endPractice();
    this.holdPosition();
    if (this.inspection) this.resumeCommand();
    if (this.surface) this.returnToOrbit();
    this.aboard = null;
    this.bridgeReturn = 'walk';
    this.interior?.atBridge();
    this.releaseLook();
    this.position.set(0, 0, 0);
    this.yaw = 0;
    this.pitch = 0;
    this.velocity.set(0, 0, 0);
    this.throttle = 0;
    this.course = null;
    this.save();
    this.emit();
  }
  private save(): void {
    const yaw =
      THREE.MathUtils.euclideanModulo(
        (this.inspection?.yaw ?? this.yaw) + Math.PI,
        Math.PI * 2,
      ) - Math.PI;
    const value: FlightSave = {
      version: 2,
      position: (this.inspection?.position ?? this.position).toArray() as Vec3,
      rotation: [yaw, this.inspection?.pitch ?? this.pitch],
      vessel: this.vessel.id,
    };
    try {
      localStorage.setItem('aquila.voyage.v1', JSON.stringify(value));
      this.saved = true;
    } catch {
      this.saved = false;
    }
  }
  private stream(center: THREE.Vector3): void {
    const key = sectorKey(sectorAt(center.toArray()));
    if (key !== this.loadedSector) {
      this.loadedSector = key;
      const cells = nearbySectors(center.toArray()),
        keep = new Set(cells.map(sectorKey));
      for (const [id, node] of this.worlds)
        if (!keep.has(id)) {
          this.space.remove(node.object);
          disposeTree(node.object);
          this.worlds.delete(id);
        }
      this.worldQueue = cells
        .map(sectorPlanet)
        .filter((p) => !this.worlds.has(p.id))
        .sort(
          (a, b) =>
            distance(a.position, center.toArray()) -
            distance(b.position, center.toArray()),
        );
    }
    // Spread creation over frames; first item is always the nearest world.
    const planet = this.worldQueue.shift();
    if (planet) {
      const object = createPlanet(planet);
      this.space.add(object);
      this.worlds.set(planet.id, { planet, object });
    }
  }
  private state(): FlightState {
    const coords = this.position.toArray() as Vec3;
    const planets = nearbySectors(coords)
      .map(sectorPlanet)
      .sort(
        (a, b) =>
          distance(a.position, coords) -
          a.radius -
          distance(b.position, coords) +
          b.radius,
      );
    const nearest = nearestPlanet(coords, planets);
    const range = Math.max(
      0,
      distance(nearest.position, coords) - nearest.radius,
    );
    return {
      position: coords,
      sector: sectorKey(sectorAt(coords)),
      speed: Math.round(this.surface ? this.throttle : this.velocity.length()),
      altitude: Math.round(this.surface?.altitude ?? 0),
      nearest: this.surface?.planet ?? nearest,
      distance: Math.round(range),
      planets: planets.slice(0, 5),
      canDescend:
        !this.surface &&
        !this.inspection &&
        !this.aboard &&
        range < descentRange(nearest, hullRadius(this.vessel)) &&
        !this.course &&
        !!this.terrain?.ready &&
        this.terrainPrepared,
      surface: !!this.surface,
      paused: this.paused,
      course: this.course?.label ?? null,
      navigation: this.capitalCommand()
        ? {
            order: this.helmOrder,
            status: this.helmStatus,
            canOrder: this.combatControl() && this.combat?.status !== 'defeat',
          }
        : null,
      saved: this.saved,
      descending: !!this.surface && this.surface.descent < 4,
      inspecting: !!this.inspection,
      sortie: !!this.inspection?.fromHangar,
      visitedInterior: !!this.interior,
      combat: this.combatState(),
      practiceNotice: this.practiceNotice,
      interior:
        this.aboard && this.interior
          ? {
              mode: this.aboard,
              room: this.interior.room,
              interaction:
                this.aboard === 'walk'
                  ? deckInteraction(this.interior.point)
                  : null,
              mouseLook:
                document.pointerLockElement === this.renderer.domElement,
            }
          : null,
      shuttleMarker:
        this.view === 'ships' && this.vessel.capital
          ? (() => {
              const point = this.shuttle.position.clone().project(this.camera);
              return point.z >= -1 && point.z <= 1
                ? {
                    x: ((point.x + 1) * this.viewport.width) / 2,
                    y: ((1 - point.y) * this.viewport.height) / 2,
                  }
                : null;
            })()
          : null,
    };
  }
  private emit(): void {
    this.onState(this.state());
  }

  private fly(dt: number): void {
    if (this.capitalCommand()) this.updateHelm();
    if (this.course) {
      this.course.elapsed += dt;
      const t = Math.min(1, this.course.elapsed / 6);
      const ease = t * t * (3 - 2 * t);
      const before = this.position.clone();
      let remaining = this.course.length * ease,
        part = 0;
      while (
        part < this.course.lengths.length - 1 &&
        remaining > this.course.lengths[part]
      )
        remaining -= this.course.lengths[part++];
      this.position.lerpVectors(
        this.course.path[part],
        this.course.path[part + 1],
        Math.min(1, remaining / Math.max(0.001, this.course.lengths[part])),
      );
      this.velocity
        .copy(this.position)
        .sub(before)
        .divideScalar(Math.max(dt, 0.001));
      const direction = new THREE.Vector3(...this.course.lookAt)
        .sub(this.position)
        .normalize();
      if (this.capitalCommand())
        [this.yaw, this.pitch] = turnToward(
          this.yaw,
          this.pitch,
          direction.toArray() as Vec3,
          dt,
        );
      else {
        this.yaw = Math.atan2(-direction.x, -direction.z);
        this.pitch = Math.asin(direction.y);
      }
      if (this.enforceOrbit(before)) {
        this.holdPosition('Course blocked. Holding position.');
        this.save();
        return;
      }
      if (t === 1) {
        const destination = this.course.label;
        this.course = null;
        this.velocity.set(0, 0, 0);
        if (this.helmOrder === 'approach')
          this.holdPosition(`Station keeping at ${destination}`);
        this.save();
      }
      return;
    }
    if (this.capitalCommand()) {
      this.velocity.set(0, 0, 0);
      return;
    }
    const axis = (positive: string, negative: string) =>
      Number(this.keys.has(positive)) - Number(this.keys.has(negative));
    const pilot = this.inspection ? FLEET[0] : this.vessel;
    const steering = pilot.handling * this.preferences.sensitivity;
    this.yaw -= axis('ArrowRight', 'ArrowLeft') * dt * steering;
    this.yaw =
      THREE.MathUtils.euclideanModulo(this.yaw + Math.PI, Math.PI * 2) -
      Math.PI;
    this.pitch = THREE.MathUtils.clamp(
      this.pitch + axis('ArrowUp', 'ArrowDown') * dt * steering,
      -1.48,
      1.48,
    );
    this.orientation.setFromEuler(
      new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
    );
    const boost = this.keys.has('ShiftLeft') || this.keys.has('ShiftRight');
    const local = new THREE.Vector3(
      axis('KeyD', 'KeyA'),
      axis('KeyE', 'KeyQ'),
      -axis('KeyW', 'KeyS'),
    );
    if (local.lengthSq() > 1) local.normalize();
    const nearby = nearbySectors(this.position.toArray()).map(sectorPlanet);
    const nearest = nearestPlanet(this.position.toArray(), nearby);
    const clearance =
      this.position.distanceTo(new THREE.Vector3(...nearest.position)) -
      nearest.radius -
      hullRadius(pilot);
    const cruise = 7 + 443 * THREE.MathUtils.smoothstep(clearance, 8000, 80000);
    const desired = local
      .applyQuaternion(this.orientation)
      .multiplyScalar(
        pilot.speed * (boost ? (this.inspection ? 7 : cruise) : 1),
      );
    if (this.keys.has('Space')) desired.set(0, 0, 0);
    this.velocity.lerp(
      desired,
      1 -
        Math.exp(
          -dt * (this.keys.has('Space') ? 12 : pilot.capital ? 0.7 : 2.6),
        ),
    );
    const before = this.position.clone();
    this.position.addScaledVector(this.velocity, dt);
    this.enforceOrbit(before);
    if (this.inspection) this.enforceHull(before);
  }
  private enforceOrbit(before: THREE.Vector3): boolean {
    // Capital ships stop outside planetary terrain; a shuttle handles descent.
    const candidates = new Map(
      [
        ...nearbySectors(before.toArray()),
        ...nearbySectors(this.position.toArray()),
      ].map((c) => [sectorKey(c), c]),
    );
    let collided = false;
    for (const cell of candidates.values()) {
      const planet = sectorPlanet(cell),
        center = new THREE.Vector3(...planet.position);
      const offset = this.position.clone().sub(center),
        minimum =
          planet.radius +
          orbitAltitude(
            planet,
            hullRadius(this.inspection ? FLEET[0] : this.vessel),
          );
      const contact = sphereContact(
        before.toArray(),
        this.position.toArray(),
        planet.position,
        minimum,
      );
      if (contact !== null) {
        this.position.lerpVectors(
          before,
          this.position,
          Math.max(0, contact - 0.00001),
        );
        offset.copy(this.position).sub(center);
        this.velocity.set(0, 0, 0);
        collided = true;
      }
      if (offset.length() < minimum) {
        if (offset.length() < 0.001) offset.set(0, 0, 1);
        this.position
          .copy(center)
          .add(offset.normalize().multiplyScalar(minimum));
        this.velocity.set(0, 0, 0);
        collided = true;
      }
    }
    return collided;
  }
  private enforceHull(before: THREE.Vector3): void {
    if (!this.inspection) return;
    const delta = this.position.clone().sub(before),
      length = delta.length();
    if (length < 0.0001) return;
    // Swept sphere approximation: center plus six skin rays. Bay mouths remain physically open.
    const radius = hullRadius(FLEET[0]);
    const origin = before.clone().sub(this.inspection.position);
    const rotation = new THREE.Quaternion().setFromEuler(
      new THREE.Euler(this.inspection.pitch, this.inspection.yaw, 0, 'YXZ'),
    );
    this.ship.position.set(0, 0, 0);
    this.ship.quaternion.copy(rotation);
    this.ship.updateMatrixWorld(true);
    this.inspectionRay.far = length + radius;
    let safe = length;
    for (const offset of [
      [0, 0, 0],
      [radius, 0, 0],
      [-radius, 0, 0],
      [0, radius, 0],
      [0, -radius, 0],
      [0, 0, radius],
      [0, 0, -radius],
    ]) {
      this.inspectionRay.set(
        origin.clone().add(new THREE.Vector3(...offset)),
        delta.clone().normalize(),
      );
      const hit = this.inspectionRay
        .intersectObject(this.ship, true)
        .find((h) => !h.object.userData['nonSolid']);
      if (hit) safe = Math.min(safe, Math.max(0, hit.distance - radius));
    }
    if (safe < length) {
      this.position.copy(before).addScaledVector(delta, safe / length);
      this.velocity.set(0, 0, 0);
    }
  }
  private flySurface(dt: number): void {
    const s = this.surface;
    if (!s) return;
    if (s.descent < 4) {
      s.descent = Math.min(4, s.descent + dt);
      const t = s.descent / 4,
        ease = t * t * (3 - 2 * t);
      s.altitude = THREE.MathUtils.lerp(s.entryAltitude, 350, ease);
      return;
    }
    const axis = (p: string, n: string) =>
      Number(this.keys.has(p)) - Number(this.keys.has(n));
    s.forward.applyAxisAngle(
      s.normal,
      -axis('ArrowRight', 'ArrowLeft') * dt * this.preferences.sensitivity,
    );
    s.altitude = THREE.MathUtils.clamp(
      s.altitude +
        (axis('KeyE', 'KeyQ') + axis('ArrowUp', 'ArrowDown')) * dt * 80,
      80,
      6000,
    );
    const boost = this.keys.has('ShiftLeft') || this.keys.has('ShiftRight');
    const speed = this.keys.has('Space')
      ? 0
      : axis('KeyW', 'KeyS') * (boost ? 360 : 120);
    this.throttle = THREE.MathUtils.lerp(
      this.throttle,
      speed,
      1 - Math.exp(-dt * 3),
    );
    const right = s.forward.clone().cross(s.normal).normalize();
    const move = s.forward
      .clone()
      .multiplyScalar(this.throttle)
      .addScaledVector(right, axis('KeyD', 'KeyA') * 100);
    if (move.length() > 0.01) {
      const turn = s.normal.clone().cross(move).normalize();
      const angle = (move.length() * dt) / (s.radius + s.altitude);
      s.normal.applyAxisAngle(turn, angle).normalize();
      s.forward.applyAxisAngle(turn, angle).normalize();
    }
  }
  private drawSurface(dt: number): void {
    const s = this.surface;
    if (!s) return;
    this.space.visible = false;
    this.sky.visible = true;
    this.stars.visible = false;
    const atmosphere = this.sky.material as THREE.ShaderMaterial;
    atmosphere.uniforms['surface'].value = 1;
    atmosphere.uniforms['upDirection'].value.copy(s.normal);
    this.scene.background = new THREE.Color(0x658891);
    // Terrain and sky handle distance haze themselves. Scene fog would compile a new ship shader on first descent.
    this.scene.fog = null;
    const height = terrainHeight(
      s.normal.x,
      s.normal.y,
      s.normal.z,
      s.planet.seed,
    );
    const observer = s.normal
      .clone()
      .multiplyScalar(s.radius + height + s.altitude);
    this.updateTerrain(s.planet, observer, observer.clone().negate(), true);
    const right = s.forward.clone().cross(s.normal).normalize();
    const rotation = new THREE.Quaternion().setFromRotationMatrix(
      new THREE.Matrix4().makeBasis(
        right,
        s.normal,
        s.forward.clone().negate(),
      ),
    );
    this.ship.visible = false;
    this.shuttle.visible = true;
    this.shuttle.position.set(0, 0, 0);
    this.shuttle.quaternion.copy(rotation);
    const zoom = this.camera.aspect < 1 ? 1.5 : 1;
    this.targetCamera.set(0, 65 * zoom, 220 * zoom);
    this.targetLook.set(0, -30, -260);
    const orbiting = this.orbitCamera(rotation, dt);
    // Keep an underside orbit above the local terrain, including hills beside the shuttle.
    const cameraWorld = observer.clone().add(this.targetCamera);
    const cameraNormal = cameraWorld.clone().normalize();
    const floor =
      s.radius +
      terrainHeight(
        cameraNormal.x,
        cameraNormal.y,
        cameraNormal.z,
        s.planet.seed,
      ) +
      25;
    if (cameraWorld.length() < floor)
      this.targetCamera.copy(cameraNormal.multiplyScalar(floor).sub(observer));
    this.camera.position.lerp(
      this.targetCamera,
      orbiting || this.preferences.reducedMotion ? 1 : 1 - Math.exp(-dt * 5),
    );
    this.camera.up.copy(s.normal);
    this.camera.lookAt(this.targetLook);
    this.sky.position.copy(this.camera.position);
  }
  private drawSpace(dt: number): void {
    this.ship.visible = true;
    this.shuttle.visible = false;
    this.space.visible = true;
    this.sky.visible = true;
    this.stars.visible = true;
    this.scene.background = null;
    this.scene.fog = null;
    (this.sky.material as THREE.ShaderMaterial).uniforms['surface'].value = 0;
    this.ship.scale.setScalar(1);
    this.camera.up.set(0, 1, 0);
    const flying =
      this.view === 'play' || this.view === 'atlas' || this.view === 'settings';
    let orbiting = false;
    const origin = flying ? this.position : new THREE.Vector3();
    this.stream(origin);
    // Geometry stays near zero. Large travel coordinates never reach GPU float transforms.
    let nearest: WorldNode | undefined,
      nearestRange = Infinity;
    for (const node of this.worlds.values()) {
      node.object.position.fromArray(node.planet.position).sub(origin);
      const range = node.object.position.length() - node.planet.radius;
      updatePlanet(
        node.object,
        this.preferences.quality,
        range + node.planet.radius,
        node.planet.radius,
      );
      node.object.getObjectByName('globe')!.visible = true;
      if (range < nearestRange) {
        nearest = node;
        nearestRange = range;
      }
    }
    if (nearest && flying && nearestRange < nearest.planet.radius * 0.7) {
      const observer = origin
        .clone()
        .sub(new THREE.Vector3(...nearest.planet.position));
      this.updateTerrain(
        nearest.planet,
        observer,
        observer.clone().negate(),
        false,
      );
      nearest.object.getObjectByName('globe')!.visible =
        !this.terrain?.ready || !this.terrainPrepared;
    } else this.clearTerrain();
    for (const vessel of this.traffic) {
      vessel.object.visible = flying && !this.vessel.capital;
      vessel.object.position.copy(vessel.position).sub(origin);
    }
    if (this.view === 'home') {
      this.ship.position.set(125, -52, -50);
      this.ship.rotation.set(0.16, 0.65, -0.14);
      this.ship.position.y += this.preferences.reducedMotion
        ? 0
        : Math.sin(this.elapsed * 0.35) * 3;
      this.targetCamera.set(30, 60, 310);
      this.targetLook.set(80, 15, -550);
      if (this.camera.aspect < 0.85) {
        this.targetCamera.set(105, 70, 390);
        this.targetLook.set(180, 0, -550);
        this.ship.position.set(120, -80, -80);
      }
      const framing = this.vessel.length / 68;
      this.ship.position.multiplyScalar(framing);
      this.targetCamera.multiplyScalar(framing);
      this.targetLook.multiplyScalar(framing);
    } else if (this.view === 'ships') {
      this.ship.position.set(35, 0, -10);
      const turn = this.preferences.reducedMotion
        ? 0.55
        : 0.55 + this.elapsed * 0.085;
      this.ship.rotation.set(0.12, turn, -0.08);
      this.targetCamera.set(70, 75, 170);
      this.targetLook.set(18, 0, 0);
      if (this.camera.aspect < 1) {
        this.targetCamera.set(55, 110, 310);
        this.targetLook.set(35, -3, -10);
      }
      const framing = this.vessel.length / 68;
      this.ship.position.multiplyScalar(framing);
      this.targetCamera.multiplyScalar(framing);
      this.targetLook.multiplyScalar(framing);
      // A true-scale shuttle rides beside the gallery as a visual yardstick.
      if (this.vessel.capital) {
        if (this.camera.aspect >= 1)
          this.targetCamera
            .sub(this.targetLook)
            .multiplyScalar(0.72)
            .add(this.targetLook);
        this.shuttle.visible = true;
        this.shuttle.position
          .set(...HANGARS[1].approach)
          .applyQuaternion(this.ship.quaternion)
          .add(this.ship.position);
        this.shuttle.quaternion.copy(this.ship.quaternion);
      }
    } else {
      this.ship.position.set(0, 0, 0);
      this.orientation.setFromEuler(
        new THREE.Euler(this.pitch, this.yaw, 0, 'YXZ'),
      );
      this.ship.quaternion.copy(this.orientation);
      this.camera.up.set(0, 1, 0).applyQuaternion(this.orientation);
      const pilot = this.inspection ? FLEET[0] : this.vessel;
      const scale = (pilot.length / 68) * (this.camera.aspect < 1 ? 1.65 : 1);
      this.targetCamera.set(
        0,
        (pilot.capital ? 36 : 48) * scale,
        (pilot.capital ? 95 : 220) * scale,
      );
      this.targetLook.set(0, 0, (pilot.capital ? -30 : -450) * scale);
      orbiting = this.orbitCamera(this.orientation, dt);
      if (this.inspection) {
        this.ship.position.copy(this.inspection.position).sub(this.position);
        this.ship.rotation.set(
          this.inspection.pitch,
          this.inspection.yaw,
          0,
          'YXZ',
        );
        this.shuttle.visible = true;
        this.shuttle.position.set(0, 0, 0);
        this.shuttle.quaternion.copy(this.orientation);
      }
    }
    if (flying) {
      // Prevent the chase camera from passing beneath terrain when the ship turns away from a planet.
      for (const node of this.worlds.values()) {
        const contact = sphereContact(
          [0, 0, 0],
          this.targetCamera.toArray(),
          node.object.position.toArray(),
          node.planet.radius + 2500,
        );
        if (contact !== null && contact > 0)
          this.targetCamera.multiplyScalar(contact * 0.98);
      }
    }
    this.camera.position.lerp(
      this.targetCamera,
      orbiting || this.preferences.reducedMotion ? 1 : 1 - Math.exp(-dt * 4),
    );
    this.camera.lookAt(this.targetLook);
    this.sky.position.copy(this.camera.position);
    this.stars.position.copy(this.camera.position);
  }
  private drawInteriorView(): void {
    const deck = this.interior!;
    deck.updateCamera(this.camera.aspect, this.aboard === 'helm', this.orbit);
    this.ship.updateMatrixWorld(true);
    const bridge = this.ship
      .getObjectByName('bridge-window')!
      .getWorldPosition(new THREE.Vector3());
    const hangar = this.ship
      .getObjectByName('starboard-hangar')!
      .getWorldPosition(new THREE.Vector3());
    this.camera.position
      .copy(bridge)
      .lerp(hangar, THREE.MathUtils.smoothstep(deck.position.z, 12, 38))
      .add(deck.position.clone().applyQuaternion(this.orientation));
    this.camera.quaternion
      .copy(this.orientation)
      .multiply(deck.camera.quaternion);
    this.ship.visible = this.shuttle.visible = false;
    this.sky.position.copy(this.camera.position);
    this.stars.position.copy(this.camera.position);
  }
  private tick(time: number): void {
    if (this.disposed) return;
    this.frame = requestAnimationFrame((t) => this.tick(t));
    const frameMs = time - this.previous;
    const dt = this.previous
      ? Math.min(0.05, (time - this.previous) / 1000)
      : 0.016;
    this.previous = time;
    if (this.lost || document.hidden) return;
    if (frameMs > 0 && frameMs < 2000) {
      this.frameTimes.push(frameMs);
      if (this.frameTimes.length > 900) this.frameTimes.shift();
    }
    const workStarted = performance.now();
    const profile = QUALITY_BUDGETS[this.preferences.quality];
    this.stars.geometry.setDrawRange(0, profile.stars);
    this.scene.environmentIntensity = profile.detail * 0.8;
    (this.sky.material as THREE.ShaderMaterial).uniforms['detail'].value =
      profile.detail;
    this.elapsed += dt;
    if (this.view === 'play' && !this.paused) {
      if (this.aboard === 'walk')
        this.interior?.walk(this.keys, dt, this.preferences.sensitivity);
      else if (this.aboard === 'tactical')
        this.interior?.walk(
          new Set([...this.keys].filter((k) => k.startsWith('Arrow'))),
          dt,
          this.preferences.sensitivity,
        );
      if (
        this.combat?.status !== 'defeat' &&
        (this.capitalCommand() || !this.aboard)
      )
        this.surface ? this.flySurface(dt) : this.fly(dt);
      else if (this.combat?.status === 'defeat') {
        this.velocity.set(0, 0, 0);
        this.throttle = 0;
      }
      this.stepCombat(dt);
    }
    if (this.surface && this.view !== 'home' && this.view !== 'ships')
      this.drawSurface(dt);
    else this.drawSpace(dt);
    const inside = !!this.aboard && this.view === 'play' && !!this.interior;
    const fov = inside ? 68 : 48;
    if (this.camera.fov !== fov) {
      this.camera.fov = fov;
      this.camera.updateProjectionMatrix();
    }
    if (inside) this.drawInteriorView();
    this.combatArt?.update(
      this.combat,
      this.position,
      this.orientation,
      this.view === 'play' && !this.surface && !this.inspection,
      this.preferences.quality === 'low'
        ? 0
        : this.preferences.quality === 'high'
        ? 2
        : 1,
    );
    this.renderer.info.reset();
    this.renderer.render(this.scene, this.camera);
    if (inside) {
      this.renderer.autoClear = false;
      this.renderer.clearDepth();
      this.renderer.render(this.interior!.scene, this.interior!.camera);
      this.renderer.autoClear = true;
    }
    this.frameWorkMs = performance.now() - workStarted;
    this.peakFrameWorkMs = Math.max(this.peakFrameWorkMs, this.frameWorkMs);
    if (time - this.lastEmit > 160) {
      this.lastEmit = time;
      this.emit();
    }
    if (this.view === 'play' && time - this.lastSave > 3000) {
      this.lastSave = time;
      this.save();
    }
  }
  diagnostics(): object {
    const frames = [...this.frameTimes].sort((a, b) => a - b);
    return {
      view: this.view,
      state: this.state(),
      loadedSectors: this.worlds.size,
      queuedWorlds: this.worldQueue.length,
      terrain: this.terrain?.diagnostics() ?? null,
      qualityBudget: QUALITY_BUDGETS[this.preferences.quality],
      frameTiming: {
        samples: frames.length,
        medianMs: frames[Math.floor(frames.length * 0.5)] ?? 0,
        p95Ms: frames[Math.floor(frames.length * 0.95)] ?? 0,
        maxMs: frames.at(-1) ?? 0,
        lastCpuWorkMs: this.frameWorkMs,
        peakCpuWorkMs: this.peakFrameWorkMs,
      },
      textures: this.renderer.info.memory.textures,
      depth: this.renderer.capabilities.reversedDepthBuffer
        ? 'reversed'
        : 'logarithmic',
      geometries: this.renderer.info.memory.geometries,
      calls: this.renderer.info.render.calls,
      triangles: this.renderer.info.render.triangles,
      canvasCount: this.host.querySelectorAll('canvas').length,
      renderedFrames: this.renderer.info.render.frame,
      pixelRatio: this.renderer.getPixelRatio(),
      contextLost: this.lost,
      ship: this.vessel.id,
      camera: this.camera.position.toArray(),
      cameraDirection: this.camera
        .getWorldDirection(new THREE.Vector3())
        .toArray(),
      cameraLook: this.targetLook.toArray(),
      cameraOrbit: { ...this.orbit, held: this.drag?.mode === 'orbit' },
      deck: this.interior
        ? {
            position: this.interior.position.toArray(),
            yaw: this.interior.yaw,
            pitch: this.interior.pitch,
            allocated: true,
          }
        : null,
      practice: this.combat
        ? {
            target: [...this.combat.target],
            elapsed: this.combat.elapsed,
            projectiles: this.combat.projectiles
              .filter((p) => p.active)
              .map((p) => ({
                ...p,
                position: [...p.position],
                previous: [...p.previous],
                velocity: [...p.velocity],
              })),
            impacts: this.combat.impacts
              .filter((p) => p.active)
              .map((p) => ({
                ...p,
                position: [...p.position],
                direction: [...p.direction],
              })),
            hits: this.combat.hits,
            incomingHits: this.combat.incomingHits,
            blocked: this.combat.blocked,
            penetrated: this.combat.penetrated,
            expired: this.combat.expired,
            obstructed: this.combat.obstructed,
            saturated: this.combat.saturated,
            effectPool: MAX_IMPACTS,
            projectilePool: MAX_PROJECTILES,
          }
        : null,
      surfaceHeading: this.surface?.forward.toArray() ?? null,
      yaw: this.yaw,
      pitch: this.pitch,
      vessel: this.vessel,
      hullRadius: hullRadius(this.inspection ? FLEET[0] : this.vessel),
      carrier: this.inspection
        ? {
            position: this.inspection.position.toArray(),
            yaw: this.inspection.yaw,
            pitch: this.inspection.pitch,
          }
        : null,
      inspectionLocalPosition: this.inspection
        ? this.position
            .clone()
            .sub(this.inspection.position)
            .applyQuaternion(
              new THREE.Quaternion()
                .setFromEuler(
                  new THREE.Euler(
                    this.inspection.pitch,
                    this.inspection.yaw,
                    0,
                    'YXZ',
                  ),
                )
                .invert(),
            )
            .toArray()
        : null,
      hangars: HANGARS,
      solidBounds: this.ship.userData['solidBounds'] ?? null,
    };
  }
  dispose(): void {
    this.save();
    this.disposed = true;
    this.releaseLook();
    cancelAnimationFrame(this.frame);
    this.cleanup.forEach((fn) => fn());
    this.resizeObserver.disconnect();
    this.clearTerrain();
    this.interior?.dispose();
    disposeTree(this.scene);
    this.reflections.dispose();
    this.renderer.dispose();
    this.renderer.domElement.remove();
  }
  private updateTerrain(
    planet: Planet,
    observer: THREE.Vector3,
    offset: THREE.Vector3,
    surface: boolean,
  ): void {
    let created = false;
    if (this.terrain?.planet.id !== planet.id) {
      this.clearTerrain();
      this.terrain = new PlanetTerrain(planet, this.preferences.quality);
      this.scene.add(this.terrain.group);
      this.terrainPrepared = false;
      created = true;
    }
    this.terrain!.group.position.copy(offset);
    this.terrain!.update(observer, this.elapsed, surface);
    if (created) {
      const field = this.terrain;
      void this.renderer
        .compileAsync(field!.group, this.camera, this.scene)
        .then(() => {
          if (!this.disposed && this.terrain === field)
            this.terrainPrepared = true;
        });
    }
    this.terrain!.group.visible = this.terrainPrepared;
  }
  private clearTerrain(): void {
    if (!this.terrain) return;
    this.scene.remove(this.terrain.group);
    this.terrain.dispose();
    this.terrain = null;
  }
}
