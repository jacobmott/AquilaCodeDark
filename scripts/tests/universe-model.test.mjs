import test from "node:test";
import assert from "node:assert/strict";
import {
  sectorAt,
  sectorKey,
  sectorPlanet,
  nearbySectors,
  readFlightSave,
  readPreferences,
  HOME_WORLD,
  SECTOR_SIZE,
  sphereContact,
  orbitAltitude,
  flightPath,
  FLAGSHIP,
  FLEET,
  hullRadius,
} from "../../src/app/universe/universe-model.ts";
import {
  planTiles,
  roots,
  tileKey,
  children,
  cubeDirection,
} from "../../src/app/universe/planet-tiles.ts";
import { terrainHeight } from "../../src/app/universe/planet-fields.ts";

test("sectors recreate worlds across a long trip, including negative coordinates", () => {
  const origin = sectorPlanet([-83, 21, -194]);
  for (let n = 0; n < 200; n++) sectorPlanet([n * 87, n * -35, 500]);
  assert.deepEqual(sectorPlanet([-83, 21, -194]), origin);
  assert.notDeepEqual(sectorPlanet([-83, 21, -193]), origin);
  assert.deepEqual(sectorPlanet([0, 0, 0]), HOME_WORLD);
});
test("streaming stays bounded and continuous at sector boundaries", () => {
  for (const position of [
    [0, 0, 0],
    [SECTOR_SIZE / 2, -SECTOR_SIZE / 2 - 1, SECTOR_SIZE / 2 - 1],
    [1e9, -1e9, 1e9],
  ]) {
    const keys = nearbySectors(position).map(sectorKey);
    assert.equal(keys.length, 27);
    assert.equal(new Set(keys).size, 27);
    assert.ok(keys.includes(sectorKey(sectorAt(position))));
  }
  const before = new Set(
    nearbySectors([SECTOR_SIZE / 2 - 1, 0, 0]).map(sectorKey),
  );
  assert.equal(
    nearbySectors([SECTOR_SIZE / 2 + 1, 0, 0]).filter((s) =>
      before.has(sectorKey(s)),
    ).length,
    18,
  );
  assert.deepEqual(
    sectorAt([-SECTOR_SIZE / 2 - 1, SECTOR_SIZE / 2, SECTOR_SIZE / 2 - 1]),
    [-1, 1, 0],
  );
});
test("save loader rejects corruption and accepts valid positions", () => {
  const good = {
    version: 2,
    position: [8500, -190, 212],
    rotation: [0.3, 0.1],
    vessel: "peregrine",
  };
  assert.deepEqual(readFlightSave(JSON.stringify(good)), good);
  for (const raw of [
    null,
    "",
    "{",
    "{}",
    JSON.stringify({ ...good, version: 3 }),
    JSON.stringify({ ...good, position: [null, 0, 0] }),
    JSON.stringify({ ...good, position: [1e15, 0, 0] }),
    JSON.stringify({ ...good, rotation: [100, 0] }),
    JSON.stringify({ ...good, vessel: "missing" }),
  ]) {
    assert.equal(readFlightSave(raw), null);
  }
});
test("legacy saves retain their sector, rotation and ship in the enlarged universe", () => {
  const old = {
    version: 1,
    position: [8500, -190, 212],
    rotation: [0.3, 0.1],
    vessel: "aegis",
  };
  const result = readFlightSave(JSON.stringify(old));
  assert.equal(result.version, 2);
  assert.deepEqual(sectorAt(result.position), [1, 0, 0]);
  assert.deepEqual(result.rotation, old.rotation);
  assert.equal(result.vessel, old.vessel);
});
test("swept collision catches a planet even when both cruise endpoints are outside", () => {
  assert.equal(
    sphereContact([-100000, 0, 0], [100000, 0, 0], [0, 0, 0], 60000),
    0.2,
  );
  assert.equal(
    sphereContact([60000, 0, 0], [70000, 0, 0], [0, 0, 0], 60000),
    null,
  );
  assert.equal(sphereContact([0, 0, 0], [70000, 0, 0], [0, 0, 0], 60000), 0);
  assert.equal(
    sphereContact([-100000, 70000, 0], [100000, 70000, 0], [0, 0, 0], 60000),
    null,
  );
});
test("terrain plans retain complete coverage and respect resident budgets across faces", () => {
  for (const limit of [54, 90, 138])
    for (const observer of [
      [0, 0, 60350],
      [60350, 0, 0],
      [35000, 35000, 35000],
      [-60200, 500, 0],
    ]) {
      const plan = planTiles(60000, observer, limit),
        keys = new Set(plan.map(tileKey));
      assert.ok(plan.length <= 6 + Math.floor((limit - 6) / 3) * 4);
      assert.equal(keys.size, plan.length);
      assert.ok(roots().every((t) => keys.has(tileKey(t))));
      const leaves = plan.filter((t) => !keys.has(tileKey(children(t)[0])));
      for (const t of plan) {
        const count = children(t).filter((c) => keys.has(tileKey(c))).length;
        assert.ok(count === 0 || count === 4);
      }
      for (let face = 0; face < 6; face++)
        for (const [u, v] of [
          [0.01, 0.73],
          [0.5, 0.5],
          [0.99, 0.001],
        ]) {
          assert.equal(
            leaves.filter(
              (t) =>
                t.face === face &&
                Math.floor(u * 2 ** t.level) === t.x &&
                Math.floor(v * 2 ** t.level) === t.y,
            ).length,
            1,
          );
        }
    }
});
test("planet fields agree across face borders and stay below the carrier boundary", () => {
  for (let i = 0; i <= 50; i++) {
    const a = cubeDirection(0, -1, i / 25 - 1),
      b = cubeDirection(4, 1, i / 25 - 1);
    assert.deepEqual(a, b);
    assert.equal(terrainHeight(...a, 42), terrainHeight(...b, 42));
    for (let face = 0; face < 6; face++) {
      const p = cubeDirection(face, i / 25 - 1, Math.sin(i));
      assert.ok(terrainHeight(...p, 42) < orbitAltitude(HOME_WORLD));
    }
  }
});
test("arrival paths clear planet boundaries for opposite-side and off-axis departures", () => {
  const planet = { ...HOME_WORLD, position: [0, 0, 0] };
  const radius = planet.radius + orbitAltitude(planet);
  for (const [start, end] of [
    [
      [radius + 0.1, 0, 0],
      [-500000, 0, 0],
    ],
    [
      [0, radius + 0.1, 0],
      [10000, -500000, 20000],
    ],
    [
      [500000, 30000, 5000],
      [-500000, 15000, 0],
    ],
  ]) {
    const path = flightPath(start, end, [planet]);
    assert.deepEqual(path[0], start);
    assert.deepEqual(path.at(-1), end);
    assert.ok(path.length > 2 && path.length < 15);
    for (let i = 1; i < path.length; i++) {
      assert.equal(
        sphereContact(path[i - 1], path[i], planet.position, radius),
        null,
      );
    }
  }
});
test("preferences respect reduced motion and clamp malformed values", () => {
  assert.deepEqual(readPreferences(null, true), {
    quality: "balanced",
    reducedMotion: true,
    sensitivity: 1,
  });
  assert.deepEqual(readPreferences('{"quality":"ultra","sensitivity":99}'), {
    quality: "balanced",
    reducedMotion: false,
    sensitivity: 1.8,
  });
});

test("capital hull clearance protects every hull corner through arbitrary turns", () => {
  const ship = FLAGSHIP,
    shuttle = FLEET[0];
  assert.deepEqual(
    [
      ship.length / shuttle.length,
      ship.width / shuttle.width,
      ship.height / shuttle.height,
    ],
    [200, 75, 80],
  );
  const radius = hullRadius(ship),
    margin = orbitAltitude(HOME_WORLD, radius);
  assert.ok(
    margin - Math.hypot(ship.length, ship.width, ship.height) / 2 >
      orbitAltitude(HOME_WORLD),
  );
  assert.ok(radius > ship.length / 2);
  assert.equal(
    readFlightSave(
      JSON.stringify({
        version: 2,
        position: [0, 0, 0],
        rotation: [0, 0],
        vessel: ship.id,
      }),
    ).vessel,
    ship.id,
  );
});

test("capital arrival arcs clear the enlarged hull envelope, including opposite-side departures", () => {
  const planet = { ...HOME_WORLD, position: [0, 0, 0] },
    clearance = hullRadius(FLAGSHIP);
  const radius = planet.radius + orbitAltitude(planet, clearance);
  const path = flightPath(
    [radius + 1, 0, 0],
    [-500000, 2000, 1000],
    [planet],
    clearance,
  );
  assert.ok(path.length > 2);
  for (let i = 1; i < path.length; i++)
    assert.equal(
      sphereContact(path[i - 1], path[i], planet.position, radius),
      null,
    );
});
