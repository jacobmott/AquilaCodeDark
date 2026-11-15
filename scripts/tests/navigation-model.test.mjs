import test from "node:test";
import assert from "node:assert/strict";
import {
  combatDestination,
  turnToward,
  COMBAT_STANDOFF,
  WITHDRAWAL_RANGE,
} from "../../src/app/universe/navigation-model.ts";
import {
  flightPath,
  sphereContact,
  orbitAltitude,
  hullRadius,
  FLAGSHIP,
} from "../../src/app/universe/universe-model.ts";

const distance = (a, b) => Math.hypot(...a.map((n, i) => n - b[i]));
test("combat orders approach from far away and retreat when too close, across all three axes", () => {
  const target = [14000, -28000, 5000];
  for (const scale of [0.1, 3]) {
    const position = target.map(
      (n, i) => n + [10000, 20000, -30000][i] * scale,
    );
    const destination = combatDestination(position, target, false);
    assert.ok(Math.abs(distance(destination, target) - COMBAT_STANDOFF) < 1e-6);
    assert.equal(
      Math.sign(destination[1] - target[1]),
      Math.sign(position[1] - target[1]),
    );
    assert.equal(
      Math.sign(destination[2] - target[2]),
      Math.sign(position[2] - target[2]),
    );
  }
});
test("range tolerance prevents repeated courses; withdrawal never returns toward the threat", () => {
  assert.equal(combatDestination([35000, 0, 0], [0, 0, 0], false), null);
  assert.equal(combatDestination([35500, 0, 0], [0, 0, 0], false), null);
  assert.equal(combatDestination([100000, 0, 0], [0, 0, 0], true), null);
  assert.equal(
    distance(combatDestination([20000, 0, 0], [0, 0, 0], true), [0, 0, 0]),
    WITHDRAWAL_RANGE,
  );
});
test("coincident targets stay finite and world offsets preserve order geometry", () => {
  assert.deepEqual(combatDestination([0, 0, 0], [0, 0, 0], false), [
    0,
    0,
    COMBAT_STANDOFF,
  ]);
  const offset = [1e10, -1e10, 1e10];
  const target = [5000, 10000, -23000],
    position = [0, 500, -20];
  const local = combatDestination(position, target, true);
  const shifted = combatDestination(
    position.map((n, i) => n + offset[i]),
    target.map((n, i) => n + offset[i]),
    true,
  );
  assert.ok(
    distance(
      local,
      shifted.map((n, i) => n - offset[i]),
    ) < 0.001,
  );
});
test("helm turns use the short arc and bound frame gaps without an instant heading snap", () => {
  const yaw = Math.PI - 0.01,
    desired = -Math.PI + 0.01;
  const direction = [-Math.sin(desired), 0, -Math.cos(desired)];
  const [next] = turnToward(yaw, 0, direction, 0.05);
  assert.ok(
    Math.abs(Math.atan2(Math.sin(next - desired), Math.cos(next - desired))) <
      1e-8,
  );
  const ordinary = turnToward(0, 0, [1, 1, 0], 0.05);
  assert.deepEqual(turnToward(0, 0, [1, 1, 0], 50), ordinary);
  assert.deepEqual(turnToward(0.7, 0.2, [0, 0, 0], 0.05), [0.7, 0.2]);
  assert.deepEqual(turnToward(0.7, 0.2, [1, 0, 0], NaN), [0.7, 0.2]);
});
test("a withdrawal course goes around an intervening planet with the capital hull clearance", () => {
  const planet = {
    id: "test",
    name: "Test",
    position: [0, 0, 0],
    radius: 12000,
    seed: 0,
    hue: 0,
    kind: "Rock",
    rings: false,
  };
  const start = [0, 0, 30000],
    target = [0, 0, 60000];
  const end = combatDestination(start, target, true);
  const clearance = hullRadius(FLAGSHIP);
  const path = flightPath(start, end, [planet], clearance);
  assert.ok(path.length > 2);
  assert.deepEqual(path.at(-1), end);
  for (let i = 1; i < path.length; i++)
    assert.equal(
      sphereContact(
        path[i - 1],
        path[i],
        planet.position,
        planet.radius + orbitAltitude(planet, clearance),
      ),
      null,
    );
});
