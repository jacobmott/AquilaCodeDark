import { EventBus } from '../EventBus';
import { Scene } from 'phaser';
import * as spine from '@esotericsoftware/spine-phaser';
import RAPIER from '@dimforge/rapier2d-compat';

import { AquilaInputService } from '../services/AquilaInputService';
import { AquilaPlayer } from '../types/AquilaPlayer';
import { AquilaEnemy } from '../types/AquilaEnemy';
import { AquilaEnemyProjectile } from '../types/AquilaEnemyProjectile';
import { AquilaSoundService } from '../services/AquilaSoundService';

export class GameScene extends Scene {
  debugGraphics!: Phaser.GameObjects.Graphics;

  rapierWorld!: RAPIER.World;
  eventQueue!: RAPIER.EventQueue;
  debugRapierGraphicsEnabled: boolean = false;

  aquilaInputService!: AquilaInputService;
  aquilaPlayer!: AquilaPlayer;

  aquilaEnemies: AquilaEnemy[] = [];
  meteorBodies: RAPIER.RigidBody[] = [];

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
  gDown: boolean = false;
  escDown: boolean = false;
  paused: boolean = false;
  canSpawnEnemy: boolean = true;
  playerDead: boolean = false;
  focusedEnemy: AquilaEnemy | null = null;
  private cam2!: Phaser.Cameras.Scene2D.Camera;
  private cam3!: Phaser.Cameras.Scene2D.Camera;
  private cam3TrackedProjectile: any = null;
  private controlsBtn!: Phaser.GameObjects.Container;
  private controlsPanel!: Phaser.GameObjects.Container;

  ready: boolean = false;

  constructor() {
    super('Game2');
  }

  async create() {
    this.ready = false;
    this.playerDead = false;
    this.focusedEnemy = null;
    this.aquilaEnemies = [];
    this.meteorBodies = [];
    this.cam3TrackedProjectile = null;

    // Remove extra cameras from previous run
    if (this.cam2) { this.cameras.remove(this.cam2, true); }
    if (this.cam3) { this.cameras.remove(this.cam3, true); }

    // Free previous Rapier world
    if (this.rapierWorld) { this.rapierWorld.free(); }

    this.aquilaInputService = new AquilaInputService(this);
    await this.setupRapierPhysics();

    // Disable right-click context menu on game canvas
    this.game.canvas.oncontextmenu = (e) => { e.preventDefault(); };

    this.setupWorld();

    this.setupCameras();

    this.setupControlsMenu();

    this.setupInput();

    this.ready = true;
    EventBus.emit('current-scene-ready', this);
    this.events.emit('scene-awake');
  }

  setupWorld() {
    this.createSpaceBackground();

    this.aquilaPlayer = new AquilaPlayer(
      this.add.spine(1545, 200, 'Ship0-360-data', 'Ship0-360-atlas'),
      this.aquilaInputService,
      this.rapierWorld,
    );

    this.spawnMeteors();

    // Spawn additional enemies randomly around the player spawn
    // Ships are 1024x1024, so min radius must be well beyond that
    const playerSpawnX = 1545;
    const playerSpawnY = 200;
    const enemyCount = 7;
    const minRadius = 5000;
    const maxRadius = 10000;
    const minSpacing = 1200; // minimum distance between any two enemies
    const spawnedPositions: { x: number; y: number }[] = [];

    for (let i = 0; i < enemyCount; i++) {
      let ex = 0, ey = 0;
      let valid = false;
      for (let attempt = 0; attempt < 50; attempt++) {
        const angle = Math.random() * Math.PI * 2;
        const radius = minRadius + Math.random() * (maxRadius - minRadius);
        ex = playerSpawnX + Math.cos(angle) * radius;
        ey = playerSpawnY + Math.sin(angle) * radius;
        valid = spawnedPositions.every(
          (p) => Math.hypot(p.x - ex, p.y - ey) >= minSpacing,
        );
        if (valid) break;
      }
      spawnedPositions.push({ x: ex, y: ey });
      const enemy = new AquilaEnemy(
        this.add.spine(ex, ey, 'Ship0-360-data', 'Ship0-360-atlas'),
        this.aquilaInputService,
        this.rapierWorld,
      );
      this.aquilaEnemies.push(enemy);
    }
  }

  createSpaceBackground() {
    // Dark space bg covering a large area
    const bgSize = 30000;
    const bg = this.add.rectangle(0, 0, bgSize * 2, bgSize * 2, 0x050510);
    bg.setOrigin(0.5);
    bg.setDepth(-10);

    // Scatter stars
    const starCount = 600;
    for (let i = 0; i < starCount; i++) {
      const sx = (Math.random() - 0.5) * bgSize * 2;
      const sy = (Math.random() - 0.5) * bgSize * 2;
      const size = Math.random() * 3 + 1;
      const alpha = Math.random() * 0.6 + 0.4;
      const tint = Phaser.Utils.Array.GetRandom([0xffffff, 0xccccff, 0xffffcc, 0xaaaaff]);
      const star = this.add.circle(sx, sy, size, tint, alpha);
      star.setDepth(-5);
    }
  }

  spawnMeteors() {
    const playerSpawnX = 1545;
    const playerSpawnY = 200;
    const meteorCount = 15;
    const minRadius = 2000;
    const maxRadius = 12000;
    const minSpacing = 1500;
    const positions: { x: number; y: number }[] = [];

    // Create meteor texture once
    if (!this.textures.exists('meteor_tex')) {
      const gfx = this.make.graphics({ x: 0, y: 0 }, false);
      // Irregular rocky shape
      gfx.fillStyle(0x665544, 1);
      gfx.fillCircle(64, 64, 60);
      gfx.fillStyle(0x554433, 1);
      gfx.fillCircle(50, 45, 40);
      gfx.fillStyle(0x776655, 1);
      gfx.fillCircle(80, 75, 35);
      gfx.fillStyle(0x443322, 1);
      gfx.fillCircle(40, 70, 25);
      gfx.fillStyle(0x887766, 1);
      gfx.fillCircle(75, 40, 20);
      // Craters
      gfx.fillStyle(0x332211, 1);
      gfx.fillCircle(55, 55, 12);
      gfx.fillCircle(75, 65, 8);
      gfx.fillCircle(45, 75, 6);
      gfx.generateTexture('meteor_tex', 128, 128);
      gfx.destroy();
    }

    for (let i = 0; i < meteorCount; i++) {
      let mx = 0, my = 0;
      let valid = false;
      for (let attempt = 0; attempt < 50; attempt++) {
        const angle = Math.random() * Math.PI * 2;
        const radius = minRadius + Math.random() * (maxRadius - minRadius);
        mx = playerSpawnX + Math.cos(angle) * radius;
        my = playerSpawnY + Math.sin(angle) * radius;
        valid = positions.every(
          (p) => Math.hypot(p.x - mx, p.y - my) >= minSpacing,
        );
        if (valid) break;
      }
      positions.push({ x: mx, y: my });

      const scale = 2 + Math.random() * 4;
      const colliderRadius = 64 * scale;
      const meteor = this.add.image(mx, my, 'meteor_tex');
      meteor.setScale(scale);
      meteor.setRotation(Math.random() * Math.PI * 2);
      meteor.setDepth(50);

      // Fixed physics body
      const bodyDesc = RAPIER.RigidBodyDesc.fixed().setTranslation(mx, my);
      const body = this.rapierWorld.createRigidBody(bodyDesc);
      const colliderDesc = RAPIER.ColliderDesc.ball(colliderRadius);
      const collider = this.rapierWorld.createCollider(colliderDesc, body);
      collider.setRestitution(1);
      // Collide with everything
      collider.setCollisionGroups(0xFFFF_FFFF);
      this.meteorBodies.push(body);
    }
  }

  setupInput() {
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
      if (key.key === 't') {
        this.tDown = key.isDown;
      }
      if (key.key === 'g') {
        this.gDown = key.isDown;
      }
      if (key.key === 'esc') {
        this.escDown = key.isDown;
      }
    });

    this.input.on('pointerdown', (pointer: Phaser.Input.Pointer) => {
      // Left-click on cam2 viewport → pan main camera to that world position
      if (pointer.button === 0 && !this.playerDead && !this.paused) {
        const cam2x = this.cam2.x;
        const cam2y = this.cam2.y;
        const cam2w = this.cam2.width;
        const cam2h = this.cam2.height;
        if (pointer.x >= cam2x && pointer.x < cam2x + cam2w &&
            pointer.y >= cam2y && pointer.y < cam2y + cam2h) {
          const worldPoint = this.cam2.getWorldPoint(pointer.x - cam2x, pointer.y - cam2y);
          this.cameras.main.stopFollow();
          this.cameras.main.centerOn(worldPoint.x, worldPoint.y);
          return;
        }
      }

      if (pointer.button !== 2 || this.playerDead || this.paused) return;

      if (this.focusedEnemy) {
        // Right click while focused on enemy → cam2 back to player
        this.focusedEnemy = null;
        this.cam2.setZoom(0.1);
        this.cam2.startFollow(this.aquilaPlayer.spineObject, true, 1, 1);
        return;
      }

      // Check if right click is on an alive enemy
      const worldPoint = this.cameras.main.getWorldPoint(pointer.x, pointer.y);
      for (const enemy of this.aquilaEnemies) {
        if (!enemy.alive) continue;
        const pos = enemy.enemyBody.translation();
        const dx = worldPoint.x - pos.x;
        const dy = worldPoint.y - pos.y;
        if (Math.sqrt(dx * dx + dy * dy) <= enemy.colliderRadius) {
          this.focusedEnemy = enemy;
          this.cam2.setZoom(0.175);
          this.cam2.startFollow(enemy.spineObject, true, 1, 1);
          return;
        }
      }
    });
  }

  async setupRapierPhysics() {
    // Initialization: Initialize Rapier with await RAPIER.init().
    // Then, create a new Rapier world with gravity set to 9.81:
    await RAPIER.init();

    this.rapierWorld = new RAPIER.World(new RAPIER.Vector2(0.0, 0));
    this.eventQueue = new RAPIER.EventQueue(false);
    this.setupDebugRapierGraphics();
  }

  findEnemyByColliderHandle(handle: number): AquilaEnemy | undefined {
    // First try direct handle match
    const direct = this.aquilaEnemies.find(
      (e) => e.alive && e.colliderHandle === handle,
    );
    if (direct) return direct;

    // Fallback: look up the collider's parent body and match against enemy bodies
    const collider = this.rapierWorld.getCollider(handle);
    if (!collider) return undefined;
    const parentBody = collider.parent();
    if (!parentBody) return undefined;
    const bodyHandle = parentBody.handle;
    return this.aquilaEnemies.find(
      (e) => e.alive && e.enemyBody && e.enemyBody.handle === bodyHandle,
    );
  }

  stepRapierPhysics() {
    // Check if the Rapier world is initialized.
    if (this.rapierWorld !== undefined) {
      // Step the physics simulation.

      const eventQueue = new RAPIER.EventQueue(true);
      this.rapierWorld.step(eventQueue);

      // Collect collision results first, then process after drain completes
      const playerProjectilesToExplode: import('../types/AquilaProjectile').AquilaProjectile[] = [];
      const enemiesToExplode: AquilaEnemy[] = [];
      const enemyProjectilesToExplode: AquilaEnemyProjectile[] = [];
      let playerHit = false;

      eventQueue.drainCollisionEvents((handle1, handle2, started) => {
        if (!started) return;

        // Check if either collider is a player projectile
        const projectile =
          this.aquilaPlayer.getProjectileByColliderHandle(handle1) ||
          this.aquilaPlayer.getProjectileByColliderHandle(handle2);
        if (projectile) {
          const otherHandle = projectile.colliderHandle === handle1 ? handle2 : handle1;
          const enemy = this.findEnemyByColliderHandle(otherHandle);
          if (enemy && enemy.alive && !enemiesToExplode.includes(enemy)) {
            enemiesToExplode.push(enemy);
          }
          if (!playerProjectilesToExplode.includes(projectile)) {
            playerProjectilesToExplode.push(projectile);
          }
          return;
        }

        // Check if either collider is an enemy projectile hitting the player
        let enemyProjectile: AquilaEnemyProjectile | undefined;
        for (const enemy of this.aquilaEnemies) {
          enemyProjectile =
            enemy.getProjectileByColliderHandle(handle1) ||
            enemy.getProjectileByColliderHandle(handle2);
          if (enemyProjectile) break;
        }
        if (enemyProjectile) {
          const otherHandle = enemyProjectile.colliderHandle === handle1 ? handle2 : handle1;
          const isPlayer = this.aquilaPlayer.isPlayerColliderHandle(otherHandle);
          if (isPlayer) {
            playerHit = true;
          }
          if (!enemyProjectilesToExplode.includes(enemyProjectile)) {
            enemyProjectilesToExplode.push(enemyProjectile);
          }
        }
      });

      // Process explosions after drain is complete (safe to modify physics world now)
      enemiesToExplode.forEach((e) => e.explode());
      playerProjectilesToExplode.forEach((p) => p.explode());
      enemyProjectilesToExplode.forEach((p) => p.explode());
      if (playerHit) this.playerDeath();

      eventQueue.drainContactForceEvents((event) => {
        const handle1 = event.collider1(); // Handle of the first collider involved in the event.
        const handle2 = event.collider2(); // Handle of the second collider involved in the event.
        /* Handle the contact force event. */
      });
    }
  }

  syncGameObjectsWithRapierPhysics() {
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
    this.cameras.main.setZoom(0.3);

    this.cam2 = this.cameras.add(0, 0, 500, 500, false, 'cam2');
    this.cam2.setZoom(0.1);
    this.cam2.startFollow(this.aquilaPlayer.spineObject, true, 1, 1);

    this.cam3 = this.cameras.add(0, 500, 500, 500, false, 'cam3');
    this.cam3.setZoom(0.075);
    this.cam3.setBackgroundColor(0x111111);

    // Draw camera border outlines using DOM overlay
    const canvas = this.game.canvas;
    const parent = canvas.parentElement!;
    const createBorder = (x: number, y: number, w: number, h: number) => {
      const div = document.createElement('div');
      div.style.position = 'absolute';
      div.style.left = `${x}px`;
      div.style.top = `${y}px`;
      div.style.width = `${w}px`;
      div.style.height = `${h}px`;
      div.style.border = '2px solid rgba(68, 136, 255, 0.6)';
      div.style.pointerEvents = 'none';
      div.style.boxSizing = 'border-box';
      div.className = 'cam-border';
      parent.style.position = 'relative';
      parent.appendChild(div);
      return div;
    };
    // Remove old borders if restarting
    parent.querySelectorAll('.cam-border').forEach(el => el.remove());
    createBorder(this.cam2.x, this.cam2.y, this.cam2.width, this.cam2.height);
    createBorder(this.cam3.x, this.cam3.y, this.cam3.width, this.cam3.height);
  }

  updateCam3Projectile() {
    const projectiles = this.aquilaPlayer.projectiles;

    // If tracked projectile died, clear it
    if (this.cam3TrackedProjectile && !this.cam3TrackedProjectile.alive) {
      this.cam3TrackedProjectile = null;
      this.cam3.stopFollow();
    }

    // If not tracking anything, find the oldest alive projectile
    if (!this.cam3TrackedProjectile) {
      const oldest = projectiles.find((p: any) => p.alive);
      if (oldest) {
        this.cam3TrackedProjectile = oldest;
        this.cam3.startFollow(oldest.graphics, true, 1, 1);
      }
    }
  }

  lostFocus() {
    this.input.keyboard!.manager.enabled = true;
  }

  gainFocus() {
    this.input.keyboard!.manager.enabled = false;
  }

  setupControlsMenu() {
    const z = 1 / this.cameras.main.zoom;

    const controlsText =
      'CAMERA\n' +
      '  Zoom In: Mousewheel Up\n' +
      '  Zoom Out: Mousewheel Down\n' +
      '  Pan: Hold Middle Mouse + Drag\n' +
      '  Pan via Minimap: Left Click on Minimap\n' +
      '\n' +
      'DEBUG\n' +
      '  Toggle Physics Debug: T\n' +
      '\n' +
      'PLAYER\n' +
      '  Rotate Left: A\n' +
      '  Rotate Right: D\n' +
      '  Forward: W\n' +
      '  Backwards: S\n' +
      '  Increase Speed: E\n' +
      '  Decrease Speed: R\n' +
      '  Increase Rotation Speed: C\n' +
      '  Decrease Rotation Speed: V\n' +
      '  Shape Cast: F\n' +
      '  Shoot: Left Click\n' +
      '  Pause: Escape\n' +
      '  Spawn Enemy: G\n' +
      '  Focus Minimap on Enemy: Right Click\n' +
      '    (Right Click again to reset)';

    // Dropdown panel (hidden initially)
    const panelPadding = 24 * z;
    const panelText = this.add.text(panelPadding, panelPadding, controlsText, {
      fontFamily: 'monospace',
      fontSize: `${Math.round(28 * z)}px`,
      color: '#e0e0e0',
      lineSpacing: 6 * z,
    });
    const panelW = panelText.width + panelPadding * 2;
    const panelH = panelText.height + panelPadding * 2;
    const panelBg = this.add.rectangle(0, 0, panelW, panelH, 0x111122, 0.92)
      .setOrigin(0, 0)
      .setStrokeStyle(2 * z, 0x4488ff);

    const panel = this.add.container(0, 0, [panelBg, panelText]);
    panel.setSize(panelW, panelH);
    panel.setDepth(1000);
    panel.setVisible(false);

    this.cam2.ignore(panel);
    this.cam3.ignore(panel);

    // Button
    const btnW = 200 * z;
    const btnH = 56 * z;

    const btnBg = this.add.rectangle(0, 0, btnW, btnH, 0x2244aa, 0.9)
      .setOrigin(0, 0)
      .setStrokeStyle(2 * z, 0x4488ff);
    const btnLabel = this.add.text(btnW / 2, btnH / 2, 'Controls', {
      fontFamily: 'monospace',
      fontSize: `${Math.round(28 * z)}px`,
      color: '#ffffff',
    }).setOrigin(0.5);

    const btn = this.add.container(0, 0, [btnBg, btnLabel]);
    btn.setDepth(1001);
    btn.setSize(btnW, btnH);
    btn.setInteractive({ useHandCursor: true });

    this.cam2.ignore(btn);
    this.cam3.ignore(btn);

    this.controlsBtn = btn;
    this.controlsPanel = panel;

    let controlsOpen = false;
    btn.on('pointerdown', () => {
      controlsOpen = !controlsOpen;
      panel.setVisible(controlsOpen);
      btnLabel.setText(controlsOpen ? 'Controls \u25b2' : 'Controls');
    });
  }

  /** Reposition controls UI to always sit at screen top-right in world coordinates */
  private updateControlsPosition() {
    const cam = this.cameras.main;
    const z = 1 / cam.zoom;
    const margin = 10 * z;
    const btnX = cam.worldView.right - this.controlsBtn.width - margin;
    const btnY = cam.worldView.y + margin;
    this.controlsBtn.setPosition(btnX, btnY);
    this.controlsPanel.setPosition(btnX + this.controlsBtn.width - this.controlsPanel.width, btnY + this.controlsBtn.height + 4 * z);
  }

  setupDebugRapierGraphics() {
    this.debugGraphics = this.add.graphics();
    this.debugGraphics.setDepth(2000);
  }

  private pauseText?: Phaser.GameObjects.Text;
  private canTogglePause: boolean = true;

  override update(time: number, delta: number) {
    if (!this.ready) {
      return;
    }

    // Always process input so pause/unpause works
    this.aquilaInputService.update();
    this.checkPause();
    this.updateControlsPosition();

    if (this.playerDead || this.paused) {
      return;
    }

    // If focused enemy died, hold cam2 at last position (don't snap to player)
    if (this.focusedEnemy && !this.focusedEnemy.alive) {
      const pos = this.focusedEnemy.lastPosition;
      this.cam2.stopFollow();
      this.cam2.centerOn(pos.x, pos.y);
      this.focusedEnemy = null;
    }

    // cam3: follow oldest alive projectile
    this.updateCam3Projectile();

    this.checkInput();
    this.aquilaPlayer.updateManually(time, delta);
    this.updateEnemyAI(delta);
    this.stepRapierPhysics();
    this.debugRapierPhysics();
    this.syncGameObjectsWithRapierPhysics();
  }

  checkPause() {
    if (this.escDown && this.canTogglePause) {
      this.canTogglePause = false;
      this.paused = !this.paused;
      if (this.paused) {
        const cam = this.cameras.main;
        const z = 1 / cam.zoom;
        const cx = cam.worldView.x + cam.worldView.width / 2;
        const cy = cam.worldView.y + cam.worldView.height / 2;
        this.pauseText = this.add.text(cx, cy, 'PAUSED', {
          fontFamily: 'Arial',
          fontSize: `${Math.round(72 * z)}px`,
          color: '#ffffff',
          stroke: '#000000',
          strokeThickness: 6 * z,
          align: 'center',
        }).setOrigin(0.5).setDepth(500);
        this.cam2.ignore(this.pauseText);
        this.cam3.ignore(this.pauseText);
      } else if (this.pauseText) {
        this.pauseText.destroy();
        this.pauseText = undefined;
      }
      setTimeout(() => {
        this.canTogglePause = true;
      }, 300);
    }
  }

  updateEnemyAI(delta: number) {
    const playerPos = this.aquilaPlayer.playerRigidBody.translation();
    for (const enemy of this.aquilaEnemies) {
      enemy.updateAI(playerPos.x, playerPos.y, delta);
    }
  }

  checkInput() {
    if (this.tDown) {
      this.debugRapierGraphicsEnabled = !this.debugRapierGraphicsEnabled;
      if (!this.debugRapierGraphicsEnabled) {
        this.debugGraphics?.clear();
      }
    }
    if (this.gDown && this.canSpawnEnemy) {
      this.canSpawnEnemy = false;
      this.spawnEnemyNearPlayer();
      setTimeout(() => {
        this.canSpawnEnemy = true;
      }, 300);
    }
  }

  spawnEnemyNearPlayer() {
    const pointer = this.input.activePointer;
    const worldPoint = this.cameras.main.getWorldPoint(pointer.x, pointer.y);
    const ex = worldPoint.x;
    const ey = worldPoint.y;
    const enemy = new AquilaEnemy(
      this.add.spine(ex, ey, 'Ship0-360-data', 'Ship0-360-atlas'),
      this.aquilaInputService,
      this.rapierWorld,
    );
    this.aquilaEnemies.push(enemy);
  }

  playerDeath() {
    if (this.playerDead) return;
    this.playerDead = true;

    // Explosion at player position
    const pos = this.aquilaPlayer.playerRigidBody.translation();
    const playerSpine = this.aquilaPlayer.spineObject;

    AquilaSoundService.playShipExplodeSound();

    // Clean up all enemy projectiles and player projectiles
    for (const enemy of this.aquilaEnemies) {
      for (const p of enemy.projectiles) {
        p.destroy();
      }
      enemy.projectiles = [];
    }
    for (const p of this.aquilaPlayer.projectiles) {
      p.destroy();
    }
    this.aquilaPlayer.projectiles = [];

    // Stop all enemy movement
    for (const enemy of this.aquilaEnemies) {
      if (enemy.alive && enemy.enemyBody) {
        enemy.enemyBody.setLinvel({ x: 0, y: 0 }, true);
      }
    }

    // --- Shake the ship ---
    const origX = playerSpine.x;
    const origY = playerSpine.y;
    this.tweens.add({
      targets: playerSpine,
      x: { from: origX - 20, to: origX + 20 },
      y: { from: origY - 12, to: origY + 12 },
      duration: 50,
      yoyo: true,
      repeat: 8,
      onComplete: () => {
        playerSpine.setPosition(origX, origY);
      },
    });

    // --- Initial explosion burst (after shake) ---
    this.time.delayedCall(450, () => {
      // Inner bright flash
      const flashEmitter = this.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 80, max: 400 },
        scale: { start: 16, end: 0 },
        alpha: { start: 1, end: 0 },
        lifespan: { min: 200, max: 400 },
        quantity: 20,
        tint: [0xffffff, 0xffffaa, 0xffff44],
        emitting: false,
        maxParticles: 20,
      });
      flashEmitter.setDepth(302);
      flashEmitter.explode();

      // Main fireball burst
      const burstEmitter = this.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 300, max: 1200 },
        scale: { start: 10, end: 2 },
        alpha: { start: 1, end: 0 },
        lifespan: { min: 500, max: 1100 },
        quantity: 50,
        tint: [0xff4400, 0xff6600, 0xff8800, 0xffaa00],
        emitting: false,
        maxParticles: 50,
      });
      burstEmitter.setDepth(301);
      burstEmitter.explode();

      this.time.delayedCall(1500, () => {
        flashEmitter.destroy();
        burstEmitter.destroy();
      });

      // --- Burning wreckage ---
      playerSpine.skeleton.color.r = 0.33;
      playerSpine.skeleton.color.g = 0.13;
      playerSpine.skeleton.color.b = 0;

      const wreckFireEmitter = this.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 30, max: 150 },
        scale: { start: 6, end: 0 },
        alpha: { start: 0.8, end: 0 },
        lifespan: { min: 400, max: 800 },
        frequency: 50,
        tint: [0xff4400, 0xff6600, 0xff2200, 0xffaa00],
        emitZone: { type: 'random', source: new Phaser.Geom.Circle(0, 0, 250) } as any,
      });
      wreckFireEmitter.setDepth(303);

      const wreckSmokeEmitter = this.add.particles(pos.x, pos.y, 'explosion_particle', {
        speed: { min: 10, max: 60 },
        scale: { start: 5, end: 10 },
        alpha: { start: 0.3, end: 0 },
        lifespan: { min: 800, max: 1500 },
        frequency: 100,
        tint: [0x333333, 0x444444, 0x222222],
        emitZone: { type: 'random', source: new Phaser.Geom.Circle(0, 0, 200) } as any,
      });
      wreckSmokeEmitter.setDepth(300);

      // Fade out wreckage after 2s, remove at 5s
      this.tweens.add({
        targets: playerSpine,
        alpha: 0,
        duration: 3000,
        delay: 2000,
        onComplete: () => {
          playerSpine.destroy();
          wreckFireEmitter.destroy();
          wreckSmokeEmitter.destroy();
        },
      });

      this.time.delayedCall(2000, () => {
        wreckFireEmitter.stop();
        wreckSmokeEmitter.stop();
      });
    });

    // Show Game Over text with animation
    const cam = this.cameras.main;
    const z = 1 / cam.zoom;
    const centerX = cam.worldView.x + cam.worldView.width / 2;
    const centerY = cam.worldView.y + cam.worldView.height / 2 - 80 * z;
    const gameOverText = this.add.text(centerX, centerY, 'GAME OVER', {
      fontFamily: 'Arial Black',
      fontSize: `${Math.round(120 * z)}px`,
      color: '#ff2222',
      stroke: '#000000',
      strokeThickness: 10 * z,
      align: 'center',
      shadow: {
        offsetX: 4 * z,
        offsetY: 4 * z,
        color: '#000000',
        blur: 10 * z,
        fill: true,
      },
    }).setOrigin(0.5).setDepth(400);
    this.cam2.ignore(gameOverText);
    this.cam3.ignore(gameOverText);

    // Start small and scale up with a bounce
    gameOverText.setScale(0);
    gameOverText.setAlpha(0);
    this.tweens.add({
      targets: gameOverText,
      scaleX: 1,
      scaleY: 1,
      alpha: 1,
      duration: 600,
      ease: 'Back.easeOut',
    });

    // Pulsing glow effect
    this.tweens.add({
      targets: gameOverText,
      scaleX: 1.08,
      scaleY: 1.08,
      duration: 800,
      yoyo: true,
      repeat: -1,
      ease: 'Sine.easeInOut',
      delay: 600,
    });

    // Color shift tween via tint
    this.tweens.addCounter({
      from: 0,
      to: 360,
      duration: 2000,
      repeat: -1,
      onUpdate: (tween) => {
        const value = tween.getValue();
        const r = Math.floor(180 + 75 * Math.sin(value * Math.PI / 180));
        const g = Math.floor(40 + 40 * Math.sin((value + 120) * Math.PI / 180));
        const b = Math.floor(40 + 40 * Math.sin((value + 240) * Math.PI / 180));
        gameOverText.setTint(Phaser.Display.Color.GetColor(r, g, b));
      },
    });

    // Restart the scene after wreckage fades
    this.time.delayedCall(5000, () => {
      // Stop engine sounds before restart
      this.aquilaPlayer.stopEngineSound();
      for (const enemy of this.aquilaEnemies) {
        enemy.stopEngineSound();
      }
      this.scene.restart();
    });
  }

  debugRapierPhysics() {
    // Clear the previous debug graphics
    if (!this.debugRapierGraphicsEnabled) {
      return;
    }
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
    this.scene.start('GameOver');
  }
}
