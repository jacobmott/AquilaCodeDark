// Fixed-step local practice simulation. Rendering never decides hits or damage.
export type CombatPoint = [number, number, number];
export type CombatRotation = [number, number, number, number];
export type Ammunition = 'standard' | 'piercing';
export type WeaponMode = 'automatic' | 'hold';
export type DamageType = 'kinetic' | 'disruption';
export interface Attack {
  type: DamageType;
  damage: number;
  penetration: number;
}
export interface CombatHealth {
  shield: number;
  hull: number;
  armor: number;
  resistance: number;
  immune: readonly DamageType[];
}
export interface HitResult {
  kind: 'shield' | 'blocked' | 'penetrated';
  shieldDamage: number;
  hullDamage: number;
  blocked: boolean;
  shieldBroken: boolean;
}
export interface CombatFrame {
  position: CombatPoint;
  rotation: CombatRotation;
  velocity: CombatPoint;
  muzzles: [CombatPoint, CombatPoint];
  clear: boolean;
  obstacles: readonly { position: CombatPoint; radius: number }[];
}
export interface Projectile {
  active: boolean;
  id: number;
  position: CombatPoint;
  previous: CombatPoint;
  velocity: CombatPoint;
  victim: 'drone' | 'ship';
  ammo: Ammunition;
  age: number;
}
export interface CombatImpact extends HitResult {
  active: boolean;
  position: CombatPoint;
  direction: CombatPoint;
  victim: 'drone' | 'ship';
  age: number;
}
export const PRACTICE_RANGE = 60000;
export const SHIELD_SIZE: CombatPoint = [2800, 1250, 7550];
export const HULL_SIZE: CombatPoint = [2400, 640, 6800];
export const DRONE_RADIUS = 1100;
export const TURRET_PIVOTS: [CombatPoint, CombatPoint] = [
  [-1350, 940, -4400],
  [1350, 940, -4400],
];
export const BARREL_LENGTH = 1120;
export const MAX_PROJECTILES = 160;
export const MAX_IMPACTS = 32;
export const PROJECTILE_LIFETIME = 12;
export const FIXED_STEP = 1 / 120;
export const AMMUNITION: Record<Ammunition, Attack> = {
  standard: { type: 'kinetic', damage: 6, penetration: 12 },
  piercing: { type: 'kinetic', damage: 8, penetration: 60 },
};
const ENEMY_ATTACK: Attack = { type: 'kinetic', damage: 10, penetration: 25 };
const distance = (a: CombatPoint, b: CombatPoint): number =>
  Math.hypot(a[0] - b[0], a[1] - b[1], a[2] - b[2]);

// A failed armor check never leaks minimum damage. Shield overflow resolves against armor separately.
export function combatDamage(body: CombatHealth, attack: Attack): HitResult {
  const result: HitResult = {
    kind: 'blocked',
    shieldDamage: 0,
    hullDamage: 0,
    blocked: false,
    shieldBroken: false,
  };
  if (
    !Number.isFinite(attack.damage) ||
    attack.damage <= 0 ||
    !Number.isFinite(attack.penetration)
  )
    return result;
  const before = body.shield;
  result.shieldDamage = Math.min(body.shield, attack.damage);
  body.shield -= result.shieldDamage;
  result.shieldBroken = before > 0 && body.shield === 0;
  const remainder = attack.damage - result.shieldDamage;
  if (remainder > 0) {
    result.blocked =
      body.immune.includes(attack.type) || attack.penetration < body.armor;
    if (!result.blocked) {
      result.hullDamage = Math.min(
        body.hull,
        remainder * (1 - body.resistance),
      );
      body.hull -= result.hullDamage;
    }
  }
  result.kind =
    result.shieldDamage > 0
      ? 'shield'
      : result.blocked
      ? 'blocked'
      : 'penetrated';
  return result;
}

// Segment against a sphere, returning first contact fraction. Used for shots and world obstruction.
export function segmentSphere(
  a: CombatPoint,
  b: CombatPoint,
  center: CombatPoint,
  radius: number,
): number | null {
  const x = a[0] - center[0],
    y = a[1] - center[1],
    z = a[2] - center[2];
  const dx = b[0] - a[0],
    dy = b[1] - a[1],
    dz = b[2] - a[2];
  const c = x * x + y * y + z * z - radius * radius;
  if (c <= 0) return 0;
  const aa = dx * dx + dy * dy + dz * dz;
  if (aa < 1e-12) return null;
  const bb = x * dx + y * dy + z * dz,
    discriminant = bb * bb - aa * c;
  if (discriminant < 0) return null;
  const t = (-bb - Math.sqrt(discriminant)) / aa;
  return t >= 0 && t <= 1 ? t : null;
}
function local(
  point: CombatPoint,
  origin: CombatPoint,
  q: CombatRotation,
  radii: CombatPoint,
): CombatPoint {
  const x = point[0] - origin[0],
    y = point[1] - origin[1],
    z = point[2] - origin[2];
  // Inverse quaternion rotation, then normalize to a unit sphere.
  const qx = -q[0],
    qy = -q[1],
    qz = -q[2],
    qw = q[3];
  const tx = 2 * (qy * z - qz * y),
    ty = 2 * (qz * x - qx * z),
    tz = 2 * (qx * y - qy * x);
  return [
    (x + qw * tx + qy * tz - qz * ty) / radii[0],
    (y + qw * ty + qz * tx - qx * tz) / radii[1],
    (z + qw * tz + qx * ty - qy * tx) / radii[2],
  ];
}
export function shipContact(
  a: CombatPoint,
  b: CombatPoint,
  before: CombatPoint,
  after: CombatPoint,
  rotation: CombatRotation,
  radii: CombatPoint,
): number | null {
  return segmentSphere(
    local(a, before, rotation, radii),
    local(b, after, rotation, radii),
    [0, 0, 0],
    1,
  );
}
function lead(
  from: CombatPoint,
  target: CombatPoint,
  velocity: CombatPoint,
  speed: number,
): CombatPoint {
  const r = target.map((n, i) => n - from[i]) as CombatPoint;
  const a = velocity.reduce((n, v) => n + v * v, 0) - speed * speed;
  const b = 2 * r.reduce((n, v, i) => n + v * velocity[i], 0),
    c = r.reduce((n, v) => n + v * v, 0);
  const d = b * b - 4 * a * c;
  let t = 0;
  if (Math.abs(a) < 1e-8) t = b < 0 ? -c / b : 0;
  else if (d >= 0) {
    const candidates = [
      (-b - Math.sqrt(d)) / (2 * a),
      (-b + Math.sqrt(d)) / (2 * a),
    ].filter((n) => n > 0);
    t = candidates.length ? Math.min(...candidates) : 0;
  }
  t = Math.min(t, PROJECTILE_LIFETIME);
  return target.map((n, i) => n + velocity[i] * t) as CombatPoint;
}

export class PracticeBattle {
  readonly player: CombatHealth = {
    shield: 100,
    hull: 100,
    armor: 8,
    resistance: 0,
    immune: [],
  };
  readonly drone: CombatHealth = {
    shield: 100,
    hull: 100,
    armor: 40,
    resistance: 0.25,
    immune: [],
  };
  readonly projectiles: Projectile[] = Array.from(
    { length: MAX_PROJECTILES },
    () => ({
      active: false,
      id: 0,
      position: [0, 0, 0],
      previous: [0, 0, 0],
      velocity: [0, 0, 0],
      victim: 'drone',
      ammo: 'standard',
      age: 0,
    }),
  );
  readonly impacts: CombatImpact[] = Array.from(
    { length: MAX_IMPACTS },
    () => ({
      active: false,
      position: [0, 0, 0],
      direction: [0, 0, 1],
      victim: 'drone',
      age: 0,
      kind: 'shield',
      shieldDamage: 0,
      hullDamage: 0,
      blocked: false,
      shieldBroken: false,
    }),
  );
  readonly muzzleAge = [10, 10, 10];
  readonly target: CombatPoint;
  status: 'ready' | 'engaged' | 'victory' | 'defeat' = 'ready';
  mode: WeaponMode = 'automatic';
  ammunition: Ammunition = 'standard';
  locked = false;
  elapsed = 0;
  fired = 0;
  received = 0;
  hits = 0;
  incomingHits = 0;
  blocked = 0;
  penetrated = 0;
  expired = 0;
  obstructed = 0;
  saturated = 0;
  armorKnown = false;
  lastImpact = 'Awaiting first impact.';
  private clocks = [0.6, 0.76, 2.8];
  private bursts = [0, 0, 0];
  private serial = 0;
  private impactCursor = 0;
  private accumulator = 0;
  private previousPosition: CombatPoint | null = null;

  constructor(target: CombatPoint) {
    this.target = [...target];
  }
  get cooldown(): number {
    return Math.max(0, Math.min(this.clocks[0], this.clocks[1]));
  }
  get activeProjectiles(): number {
    return this.projectiles.filter((p) => p.active).length;
  }
  range(frame: CombatFrame): number {
    return distance(frame.position, this.target);
  }
  reason(frame: CombatFrame): string {
    if (this.status === 'victory') return 'Target disabled. Practice complete.';
    if (this.status === 'defeat')
      return 'Delta Tree disabled. Reset to try again.';
    if (this.mode === 'hold')
      return 'Holding new shots. Rounds already in flight continue.';
    if (this.range(frame) > PRACTICE_RANGE) return 'Target beyond 60 km range.';
    if (!frame.clear) return 'A planet blocks the firing line.';
    if (
      this.armorKnown &&
      this.ammunition === 'standard' &&
      this.drone.shield === 0
    )
      return 'No penetration. Fit piercing rounds to defeat this armor.';
    return this.ammunition === 'standard'
      ? 'Twin autocannons · standard kinetic rounds.'
      : 'Twin autocannons · armor-piercing rounds.';
  }
  setMode(mode: WeaponMode): void {
    if (this.terminal()) return;
    if (mode === 'automatic' || mode === 'hold') this.mode = mode;
  }
  setAmmunition(ammo: Ammunition): void {
    if (this.terminal()) return;
    if (ammo === 'standard' || ammo === 'piercing') this.ammunition = ammo;
  }
  private terminal(): boolean {
    return this.status === 'victory' || this.status === 'defeat';
  }

  step(dt: number, frame: CombatFrame): void {
    if (!Number.isFinite(dt) || dt <= 0) return;
    dt = Math.min(dt, 0.1);
    const from = this.previousPosition ?? frame.position;
    const oldRemainder = this.accumulator;
    this.accumulator += dt;
    let consumed = FIXED_STEP - oldRemainder;
    let before: CombatPoint = [...from];
    while (this.accumulator + 1e-10 >= FIXED_STEP) {
      const t = Math.min(1, consumed / dt);
      const position = from.map(
        (n, i) => n + (frame.position[i] - n) * t,
      ) as CombatPoint;
      this.tick(frame, before, position);
      before = position;
      consumed += FIXED_STEP;
      this.accumulator -= FIXED_STEP;
    }
    this.previousPosition = [...frame.position];
  }
  private tick(
    frame: CombatFrame,
    before: CombatPoint,
    position: CombatPoint,
  ): void {
    this.elapsed += FIXED_STEP;
    for (let i = 0; i < 3; i++) this.muzzleAge[i] += FIXED_STEP;
    for (const effect of this.impacts) {
      if (effect.active) {
        effect.age += FIXED_STEP;
        if (effect.age > 0.85) effect.active = false;
      }
    }
    if (this.terminal()) return;
    for (const p of this.projectiles) {
      if (!p.active) continue;
      p.age += FIXED_STEP;
      for (let i = 0; i < 3; i++) {
        p.previous[i] = p.position[i];
        p.position[i] += p.velocity[i] * FIXED_STEP;
      }
      const contact =
        p.victim === 'drone'
          ? segmentSphere(
              p.previous,
              p.position,
              this.target,
              DRONE_RADIUS * (this.drone.shield > 0 ? 1.12 : 1),
            )
          : shipContact(
              p.previous,
              p.position,
              before,
              position,
              frame.rotation,
              this.player.shield > 0 ? SHIELD_SIZE : HULL_SIZE,
            );
      let worldContact = 2;
      for (const world of frame.obstacles) {
        const t = segmentSphere(
          p.previous,
          p.position,
          world.position,
          world.radius,
        );
        if (t !== null) worldContact = Math.min(worldContact, t);
      }
      if (worldContact <= 1 && (contact === null || worldContact < contact)) {
        p.active = false;
        this.obstructed++;
        continue;
      }
      if (contact !== null) {
        for (let i = 0; i < 3; i++)
          p.position[i] =
            p.previous[i] + (p.position[i] - p.previous[i]) * contact;
        this.impact(p);
        p.active = false;
        if (this.terminal()) {
          for (const other of this.projectiles) other.active = false;
          return;
        }
      } else if (p.age >= PROJECTILE_LIFETIME) {
        p.active = false;
        this.expired++;
      }
    }
    this.locked = frame.clear && this.range(frame) <= PRACTICE_RANGE;
    for (let i = 0; i < 3; i++) {
      if (i < 2 || this.status === 'engaged') this.clocks[i] -= FIXED_STEP;
      if (this.clocks[i] > 1e-9 || !this.locked) continue;
      if (i < 2) {
        if (
          this.mode === 'hold' ||
          (this.armorKnown &&
            this.drone.shield === 0 &&
            this.ammunition === 'standard')
        )
          continue;
        const aim: CombatPoint = [...this.target];
        const n = this.fired + i;
        aim[0] += Math.sin(n * 2.399) * DRONE_RADIUS * 0.35;
        aim[1] += Math.cos(n * 2.399) * DRONE_RADIUS * 0.35;
        if (!this.launch(frame.muzzles[i], aim, 'drone', this.ammunition, 8500))
          continue;
        this.fired++;
        this.status = 'engaged';
      } else {
        if (this.status !== 'engaged') continue;
        const aim = lead(this.target, position, frame.velocity, 7000);
        const delta = aim.map((n, j) => n - this.target[j]) as CombatPoint;
        const length = Math.max(1, Math.hypot(...delta));
        const muzzle = this.target.map(
          (n, j) => n + (delta[j] / length) * 1250,
        ) as CombatPoint;
        if (!this.launch(muzzle, aim, 'ship', 'standard', 7000)) continue;
        this.received++;
      }
      this.muzzleAge[i] = 0;
      this.bursts[i]++;
      const count = i < 2 ? 4 : 2;
      this.clocks[i] =
        this.bursts[i] % count === 0
          ? i < 2
            ? 1.65
            : 2.8
          : i < 2
          ? 0.14
          : 0.3;
    }
  }
  private launch(
    from: CombatPoint,
    to: CombatPoint,
    victim: Projectile['victim'],
    ammo: Ammunition,
    speed: number,
  ): boolean {
    const p = this.projectiles.find((n) => !n.active);
    if (!p) {
      this.saturated++;
      return false;
    }
    const length = Math.max(1, distance(from, to));
    p.active = true;
    p.id = ++this.serial;
    p.age = 0;
    p.victim = victim;
    p.ammo = ammo;
    for (let i = 0; i < 3; i++) {
      p.position[i] = p.previous[i] = from[i];
      p.velocity[i] = ((to[i] - from[i]) / length) * speed;
    }
    return true;
  }
  private impact(p: Projectile): void {
    const body = p.victim === 'drone' ? this.drone : this.player;
    const result = combatDamage(
      body,
      p.victim === 'drone' ? AMMUNITION[p.ammo] : ENEMY_ATTACK,
    );
    const effect = this.impacts[this.impactCursor++ % MAX_IMPACTS];
    Object.assign(effect, result);
    effect.active = true;
    effect.age = 0;
    effect.victim = p.victim;
    for (let i = 0; i < 3; i++) {
      effect.position[i] = p.position[i];
      effect.direction[i] = p.velocity[i];
    }
    if (p.victim === 'drone') {
      this.hits++;
      if (result.blocked) {
        this.blocked++;
        this.armorKnown = true;
      }
      if (result.hullDamage > 0) this.penetrated++;
      this.lastImpact = result.shieldBroken
        ? result.blocked
          ? 'Shield broken · armor blocked.'
          : 'Shield broken.'
        : result.kind === 'shield'
        ? 'Shield impact.'
        : result.blocked
        ? 'NO PENETRATION · armor blocked the round.'
        : 'PENETRATED · hull damage confirmed.';
    } else this.incomingHits++;
    if (body.hull <= 0) {
      this.status = p.victim === 'drone' ? 'victory' : 'defeat';
      this.locked = false;
    }
  }
}
