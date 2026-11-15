import test from "node:test";
import assert from "node:assert/strict";
import {
  PracticeBattle,
  combatDamage,
  segmentSphere,
  shipContact,
  AMMUNITION,
  MAX_PROJECTILES,
  MAX_IMPACTS,
  HULL_SIZE,
} from "../../src/app/universe/combat-model.ts";
const frame = {
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  velocity: [0, 0, 0],
  muzzles: [
    [-1350, 940, -5520],
    [1350, 940, -5520],
  ],
  clear: true,
  obstacles: [],
};
const step = (battle, seconds, state = frame, dt = 1 / 120) => {
  for (let i = 0; i < Math.round(seconds / dt); i++) battle.step(dt, state);
};
const health = () => ({
  shield: 10,
  hull: 100,
  armor: 40,
  resistance: 0.25,
  immune: [],
});
test("shield overflow resolves armor separately; complete protection has no chip damage", () => {
  const body = health();
  const hit = combatDamage(body, { ...AMMUNITION.standard, damage: 26 });
  assert.equal(hit.shieldDamage, 10);
  assert.equal(hit.blocked, true);
  assert.equal(hit.shieldBroken, true);
  assert.equal(body.hull, 100);
  for (let i = 0; i < 1000; i++) combatDamage(body, AMMUNITION.standard);
  assert.equal(body.hull, 100);
  assert.equal(combatDamage(body, AMMUNITION.piercing).hullDamage, 6);
  assert.equal(body.hull, 94);
  for (const damage of [NaN, Infinity, -1, 0])
    combatDamage(body, { ...AMMUNITION.piercing, damage });
  assert.equal(body.hull, 94);
  body.immune = ["disruption"];
  assert.equal(
    combatDamage(body, { type: "disruption", damage: 100, penetration: 100 })
      .blocked,
    true,
  );
  assert.equal(body.hull, 94);
});
test("swept contacts catch crossing rounds, moving hulls and rotated ellipsoids", () => {
  assert.equal(segmentSphere([0, 0, -100], [0, 0, 100], [0, 0, 0], 10), 0.45);
  assert.equal(segmentSphere([11, 0, -100], [11, 0, 100], [0, 0, 0], 10), null);
  assert.equal(segmentSphere([0, 0, 0], [0, 0, 0], [0, 0, 0], 10), 0);
  const t = shipContact(
    [9000, 0, 0],
    [-9000, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0, 1],
    HULL_SIZE,
  );
  assert.ok(Math.abs(t - (9000 - 2400) / 18000) < 1e-10);
  const rotated = shipContact(
    [9000, 0, 0],
    [-9000, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, Math.SQRT1_2, 0, Math.SQRT1_2],
    HULL_SIZE,
  );
  assert.ok(Math.abs(rotated - (9000 - 6800) / 18000) < 1e-10);
  assert.notEqual(
    shipContact(
      [0, 0, 0],
      [0, 0, 0],
      [-10000, 0, 0],
      [10000, 0, 0],
      [0, 0, 0, 1],
      HULL_SIZE,
    ),
    null,
  );
});
test("two turrets launch separated rounds without instant damage; impacts follow travel", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  step(b, 0.9);
  assert.equal(b.locked, true);
  assert.ok(b.fired >= 3);
  assert.equal(b.hits, 0);
  assert.equal(b.drone.shield, 100);
  assert.ok(b.projectiles.some((p) => p.active && p.position[0] < 0));
  assert.ok(b.projectiles.some((p) => p.active && p.position[0] > 0));
  const p = b.projectiles.find((p) => p.active),
    z = p.position[2],
    fired = b.fired;
  b.setMode("hold");
  step(b, 0.3);
  assert.ok(p.position[2] < z - 2000);
  assert.equal(b.drone.shield, 100);
  step(b, 3);
  assert.equal(b.fired, fired);
  assert.ok(b.hits > 0);
  assert.ok(b.drone.shield < 100);
});
test("hold fire delays engagement and does not cancel rounds or enemy fire", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  b.setMode("hold");
  step(b, 10);
  assert.equal(b.status, "ready");
  assert.equal(b.fired + b.received, 0);
  b.setMode("automatic");
  step(b, 0.1);
  const fired = b.fired;
  assert.ok(fired > 0);
  b.setMode("hold");
  step(b, 8);
  assert.equal(b.fired, fired);
  assert.ok(b.received > 0);
  assert.ok(b.player.shield < 100);
  b.setMode("invalid");
  assert.equal(b.mode, "hold");
});
test("range and line of sight gate new launches, not projectiles already in flight", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  step(b, 3, { ...frame, clear: false });
  assert.equal(b.fired, 0);
  step(b, 3, { ...frame, position: [0, 0, 100000] });
  assert.equal(b.fired, 0);
  step(b, 0.3);
  assert.equal(b.locked, true);
  assert.ok(b.fired > 0);
  const fired = b.fired;
  step(b, 4, { ...frame, clear: false });
  assert.equal(b.fired, fired);
  assert.ok(b.hits > 0);
  assert.equal(b.received, 0);
  step(b, 0.1);
  assert.ok(b.fired > fired);
  assert.ok(b.received > 0);
});
test("a planet crossing a launched trajectory blocks impact damage", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  step(b, 0.9);
  b.setMode("hold");
  step(b, 5, {
    ...frame,
    clear: false,
    obstacles: [{ position: [0, 0, -12000], radius: 2500 }],
  });
  assert.ok(b.obstructed > 0);
  assert.equal(b.hits, 0);
  assert.equal(b.drone.shield, 100);
});
test("changing ammunition affects later launches, never rounds already flying", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  step(b, 0.9);
  const ids = b.projectiles.filter((p) => p.active).map((p) => p.id);
  b.setAmmunition("piercing");
  step(b, 0.4);
  for (const id of ids)
    assert.equal(b.projectiles.find((p) => p.id === id).ammo, "standard");
  assert.ok(b.projectiles.some((p) => p.active && p.ammo === "piercing"));
  b.setAmmunition("invalid");
  assert.equal(b.ammunition, "piercing");
});
test("standard rounds drain a shield, stop after confirmed protection, then piercing wins", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  step(b, 12);
  assert.equal(b.drone.shield, 0);
  assert.equal(b.drone.hull, 100);
  assert.ok(b.blocked > 0);
  assert.equal(b.armorKnown, true);
  assert.match(b.reason(frame), /No penetration/);
  const fired = b.fired;
  step(b, 3);
  assert.equal(b.fired, fired);
  assert.equal(b.drone.hull, 100);
  b.setAmmunition("piercing");
  step(b, 15);
  assert.equal(b.status, "victory");
  assert.ok(b.penetrated > 0);
  assert.equal(b.drone.hull, 0);
});
test("an incoming round keeps its launch trajectory and can miss a maneuvering ship", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  step(b, 3.8);
  b.setMode("hold");
  assert.ok(b.received > 0);
  assert.equal(b.incomingHits, 0);
  for (let i = 1; i <= 1800; i++) {
    const x = (i * 10000) / 120;
    b.step(1 / 120, {
      ...frame,
      position: [x, 0, 0],
      velocity: [10000, 0, 0],
      clear: false,
    });
  }
  assert.equal(b.incomingHits, 0);
  assert.equal(b.player.shield, 100);
  assert.ok(b.expired > 0);
});
test("fixed simulation cadence produces equal outcomes at 30 and 144 presentation frames per second", () => {
  const states = [];
  for (const dt of [1 / 30, 1 / 144]) {
    const b = new PracticeBattle([0, 0, -26000]);
    step(b, 12, frame, dt);
    states.push({
      fired: b.fired,
      received: b.received,
      hits: b.hits,
      blocked: b.blocked,
      player: b.player,
      drone: b.drone,
    });
  }
  assert.deepEqual(states[0], states[1]);
});
test("large coordinate offsets preserve combat and shield/armor results", () => {
  const a = new PracticeBattle([0, 0, -26000]);
  step(a, 12);
  const offset = [1e9, -1e9, 1e9];
  const plus = (p) => p.map((n, i) => n + offset[i]);
  const b = new PracticeBattle(plus([0, 0, -26000]));
  step(b, 12, {
    ...frame,
    position: plus(frame.position),
    muzzles: frame.muzzles.map(plus),
  });
  assert.deepEqual(b.player, a.player);
  assert.deepEqual(b.drone, a.drone);
  assert.equal(b.blocked, a.blocked);
  assert.equal(b.hits, a.hits);
});
test("victory and defeat stop both guns, clear active rounds and keep pools bounded", () => {
  for (const win of [true, false]) {
    const b = new PracticeBattle([0, 0, -26000]);
    if (win) b.setAmmunition("piercing");
    step(b, 0.9);
    if (!win) b.setMode("hold");
    step(b, 45);
    assert.equal(b.status, win ? "victory" : "defeat");
    assert.equal(b.activeProjectiles, 0);
    const counts = [b.fired, b.received],
      pool = b.projectiles[0],
      effects = b.impacts[0];
    step(b, 100);
    assert.deepEqual([b.fired, b.received], counts);
    assert.equal(b.projectiles.length, MAX_PROJECTILES);
    assert.equal(b.impacts.length, MAX_IMPACTS);
    assert.equal(b.projectiles[0], pool);
    assert.equal(b.impacts[0], effects);
    assert.equal(
      b.impacts.some((p) => p.active),
      false,
    );
    assert.equal(b.saturated, 0);
    const reset = new PracticeBattle([0, 0, -26000]);
    assert.equal(reset.ammunition, "standard");
    assert.equal(reset.mode, "automatic");
    assert.equal(reset.hits, 0);
  }
});
test("invalid and long frame gaps cannot create a catch-up burst", () => {
  const b = new PracticeBattle([0, 0, -26000]);
  for (const dt of [NaN, Infinity, -1, 0, 10000]) b.step(dt, frame);
  assert.equal(b.fired, 0);
  assert.ok(b.elapsed < 0.11);
});
