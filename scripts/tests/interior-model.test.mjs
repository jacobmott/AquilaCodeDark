import test from "node:test";
import assert from "node:assert/strict";
import {
  BOARDING,
  HELM,
  TACTICAL,
  deckInteraction,
  deckRoom,
  deckWalkable,
  moveOnDeck,
  walkingLookDelta,
} from "../../src/app/universe/interior-model.ts";

test("walking look rejects isolated vertical and horizontal spikes without losing the other axis", () => {
  assert.deepEqual(walkingLookDelta(4, 700, 1), [0.01, 0]);
  assert.deepEqual(walkingLookDelta(-700, -4, 1), [0, -0.01]);
  assert.deepEqual(walkingLookDelta(12000, -12000, 1.8), [0, 0]);
  assert.deepEqual(walkingLookDelta(NaN, Infinity, 1), [0, 0]);
});

test("normal walking look keeps sensitivity and continuous full turns without limiting the total angle", () => {
  for (const sensitivity of [0.4, 1, 1.8]) {
    assert.deepEqual(walkingLookDelta(12, -8, sensitivity), [
      0.03 * sensitivity,
      -0.02 * sensitivity,
    ]);
    let yaw = 0;
    for (let n = 0; n < 160; n++)
      yaw += walkingLookDelta(48, 0, sensitivity)[0];
    assert.ok(yaw > Math.PI * 2);
  }
});

test("bridge, passage and boarding station form one continuous walkable trip", () => {
  let point = [...HELM];
  const rooms = new Set();
  while (point[1] < BOARDING[1]) {
    point = moveOnDeck(point, [0, 0.1]);
    assert.ok(deckWalkable(...point));
    rooms.add(deckRoom(point[1]));
  }
  assert.deepEqual([...rooms], ["Bridge", "Passage", "Hangar"]);
  assert.equal(deckInteraction(point), "shuttle");
  point = moveOnDeck(point, [0, HELM[1] - point[1]]);
  assert.ok(Math.abs(point[1] - HELM[1]) < 0.001);
  assert.equal(deckInteraction(point), "helm");
  assert.equal(deckInteraction([0, 25]), null);
});

test("tactical station is reachable from the helm without crossing console geometry", () => {
  const point = moveOnDeck(HELM, [TACTICAL[0] - HELM[0], 0]);
  assert.ok(Math.hypot(point[0] - TACTICAL[0], point[1] - TACTICAL[1]) < 0.01);
  assert.equal(deckInteraction(point), "tactical");
  assert.ok(moveOnDeck(point, [0, -50])[1] > -3.7);
});

test("long frames cannot tunnel through consoles, shuttle or outside walls", () => {
  assert.ok(moveOnDeck(HELM, [0, -200])[1] > -3.7);
  assert.ok(moveOnDeck(BOARDING, [0, 200])[1] < 69);
  assert.ok(moveOnDeck([0, 25], [200, 0])[0] <= 2.6);
  assert.ok(moveOnDeck([40, 100], [200, 0])[0] <= 47.6);
  assert.ok(moveOnDeck([-40, 100], [-200, 0])[0] >= -47.6);
});

test("walkers slide along passage walls and can pass around the parked shuttle", () => {
  const slide = moveOnDeck([2.5, 20], [10, 10]);
  assert.ok(slide[0] <= 2.6);
  assert.ok(Math.abs(slide[1] - 30) < 0.01);
  let point = moveOnDeck(BOARDING, [40, 0]);
  point = moveOnDeck(point, [0, 75]);
  assert.ok(Math.abs(point[1] - 140) < 0.01);
  point = moveOnDeck(point, [-80, 0]);
  assert.ok(Math.abs(point[0] + 40) < 0.01);
  assert.ok(deckWalkable(...point));
});
