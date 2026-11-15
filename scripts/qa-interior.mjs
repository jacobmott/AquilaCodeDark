// Real input in isolated installed Chrome. Diagnostics are read-only observations.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";

const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-interior-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const browser = await chromium.launch({ channel: "chrome", headless: true });
const context = await browser.newContext({
  viewport: { width: 1440, height: 900 },
});
const page = await context.newPage(),
  errors = [],
  checks = [];
page.setDefaultTimeout(12000);
page.on("pageerror", (e) => errors.push(String(e)));
page.on("console", (m) => {
  if (m.type() === "error") errors.push(m.text());
});
const state = () => page.evaluate(() => window.aquilaDiagnostics());
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log("PASS", name);
};
const capture = (name) =>
  page.screenshot({
    path: `reports/game-direction/media/${prefix}${name}.png`,
  });
const hold = async (key, ms) => {
  await page.keyboard.down(key);
  await page.waitForTimeout(ms);
  await page.keyboard.up(key);
};
const walkTo = async (key, z, greater) => {
  await page.keyboard.down("Shift");
  await page.keyboard.down(key);
  try {
    await page.waitForFunction(
      ({ z, greater }) => {
        const current = window.aquilaDiagnostics().deck.position[2];
        return greater ? current >= z : current <= z;
      },
      { z, greater },
    );
  } finally {
    await page.keyboard.up(key);
    await page.keyboard.up("Shift");
  }
};
const difference = (a, b) => Math.hypot(...a.map((v, i) => v - b[i]));

try {
  await page.goto(`${base}/play`);
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 20,
  );
  await page.waitForTimeout(1600);
  await page.evaluate(() => {
    window.qaCanvas = document.querySelector("canvas");
  });
  const original = await state();
  await page.getByRole("button", { name: "Enter bridge" }).click();
  await page.waitForTimeout(900);
  const entered = await state();
  assert.equal(entered.state.interior.mode, "walk");
  assert.equal(entered.state.interior.room, "Bridge");
  assert.equal(entered.canvasCount, 1);
  assert.deepEqual(entered.state.position, original.state.position);
  assert.ok(difference(entered.cameraDirection, [0, 0, -1]) < 0.00001);
  await capture("bridge");
  check(
    "Enter a human-scale bridge with one canvas and a live forward world view",
    {
      worldCamera: entered.camera,
      deckPosition: entered.deck.position,
    },
  );

  await page.mouse.click(750, 420);
  await page.waitForFunction(() => Boolean(document.pointerLockElement));
  const locked = await state();
  await page.mouse.move(830, 430, { steps: 8 });
  assert.notEqual((await state()).deck.yaw, locked.deck.yaw);
  assert.equal((await state()).yaw, locked.yaw);
  await page.mouse.click(830, 430);
  assert.equal(
    await page.evaluate(() => Boolean(document.pointerLockElement)),
    true,
  );
  await page.keyboard.press("Escape");
  await page.waitForFunction(
    () =>
      !document.pointerLockElement && window.aquilaDiagnostics().state.paused,
  );
  const stopped = await state();
  await hold("w", 350);
  assert.deepEqual((await state()).deck.position, stopped.deck.position);
  await page.getByRole("button", { name: "Resume flight" }).click();
  // Reset personal look through the existing helm interaction, using its nearby zone.
  await hold("w", 450);
  await page.getByRole("button", { name: /^Use command console/ }).click();
  check(
    "Click-to-look turns only the walker; Escape releases the cursor and pauses walking",
  );

  const helm = await state();
  assert.equal(helm.state.interior.mode, "helm");
  assert.equal(
    await page.evaluate(() => Boolean(document.pointerLockElement)),
    false,
  );
  await hold("w", 750);
  await hold("ArrowRight", 450);
  assert.deepEqual((await state()).state.position, helm.state.position);
  assert.equal((await state()).yaw, helm.yaw);
  await page.getByRole("button", { name: "Approach orbit" }).click();
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.speed > 1000,
  );
  await page
    .getByRole("button", { name: "Hold position", exact: true })
    .click();
  const flown = await state();
  assert.ok(difference(flown.state.position, helm.state.position) > 10);
  assert.notEqual(flown.yaw, helm.yaw);
  assert.ok(difference(flown.cameraDirection, helm.cameraDirection) > 0.02);
  await page.mouse.move(950, 400);
  await page.mouse.down({ button: "right" });
  await page.mouse.move(1100, 450, { steps: 8 });
  const looked = await state();
  assert.equal(looked.yaw, flown.yaw);
  assert.equal(looked.pitch, flown.pitch);
  assert.ok(difference(looked.cameraDirection, flown.cameraDirection) > 0.1);
  await page.mouse.up({ button: "right" });
  await page.waitForFunction(
    () => window.aquilaDiagnostics().cameraOrbit.yaw === 0,
  );
  await capture("helm");
  await page.keyboard.press("c");
  assert.equal((await state()).state.interior, null);
  const exterior = await state();
  await page.getByRole("button", { name: "Return aboard" }).click();
  assert.equal((await state()).state.interior.mode, "helm");
  assert.deepEqual((await state()).state.position, exterior.state.position);
  assert.equal((await state()).yaw, exterior.yaw);
  check(
    "Helm orders move the ship while manual input is ignored; look and exterior view preserve command state",
  );

  await page.getByRole("button", { name: "Leave command console" }).click();
  let parked = await state();
  await walkTo("s", 23, true);
  await hold("d", 1000);
  const wall = await state();
  assert.ok(wall.deck.position[0] <= 2.6 && wall.deck.position[0] > 2);
  await page.keyboard.down("a");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().deck.position[0] < 0.15,
  );
  await page.keyboard.up("a");
  await capture("passage");
  await walkTo("s", 64, true);
  assert.equal((await state()).state.interior.room, "Hangar");
  assert.deepEqual((await state()).state.position, parked.state.position);
  // Turn to face the shuttle without any simulation setter or teleport.
  await hold("ArrowLeft", 2090);
  await capture("hangar");
  await page.getByRole("link", { name: "Atlas", exact: true }).click();
  await page.locator(".destination").first().click();
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.speed > 1000,
  );
  assert.equal((await state()).state.interior.room, "Hangar");
  await page
    .getByRole("button", { name: "Board and launch Peregrine" })
    .click();
  const sortie = await state();
  assert.equal(sortie.state.sortie, true);
  assert.equal(sortie.state.interior, null);
  assert.equal(sortie.state.course, null);
  assert.ok(difference(sortie.carrier.position, parked.state.position) > 10);
  parked = {
    ...parked,
    state: { ...parked.state, position: sortie.carrier.position },
    yaw: sortie.carrier.yaw,
    pitch: sortie.carrier.pitch,
  };
  assert.deepEqual(sortie.carrier.position, parked.state.position);
  assert.equal(sortie.carrier.yaw, parked.yaw);
  await hold("w", 1500);
  assert.ok(
    difference((await state()).state.position, sortie.state.position) > 50,
  );
  await capture("sortie");
  await page.getByRole("button", { name: "Return to hangar" }).click();
  const returned = await state();
  assert.equal(returned.state.interior.room, "Hangar");
  assert.equal(returned.state.interior.mode, "walk");
  assert.equal(returned.state.sortie, false);
  assert.deepEqual(returned.state.position, parked.state.position);
  assert.equal(returned.yaw, parked.yaw);
  assert.equal(returned.pitch, parked.pitch);
  await capture("returned");
  await page.getByRole("button", { name: "Exterior view" }).click();
  await page.getByRole("button", { name: "Return aboard" }).click();
  assert.deepEqual((await state()).deck.position, returned.deck.position);
  check(
    "Walk through the passage; launching during an automatic course parks the capital, then guided return restores that launch position",
    {
      parkedPosition: parked.state.position,
      launchPosition: sortie.state.position,
      restoredPosition: returned.state.position,
    },
  );

  await walkTo("s", -1.5, false); // At return, the walker faces aft; backward travels toward the bridge.
  await page.getByRole("button", { name: /^Use command console/ }).click();
  await page.getByRole("button", { name: "Leave command console" }).click();
  const resident = await state();
  for (let i = 0; i < 4; i++) {
    await page.getByRole("button", { name: "Exterior view" }).click();
    await page.getByRole("button", { name: "Return aboard" }).click();
    await page.waitForTimeout(100);
  }
  const reused = await state();
  assert.equal(reused.geometries, resident.geometries);
  assert.equal(reused.textures, resident.textures);
  assert.equal(reused.canvasCount, 1);
  check(
    "Walk back to command; repeated interior visits reuse geometry, textures and the renderer",
    {
      geometries: reused.geometries,
      textures: reused.textures,
    },
  );

  const beforeRoute = await state();
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  for (const name of ["Light", "High", "Balanced"]) {
    await page
      .getByRole("radiogroup", { name: "Visual quality" })
      .getByText(name, { exact: true })
      .click();
    await page.waitForTimeout(150);
  }
  await page.getByRole("button", { name: "Return to your ship" }).click();
  assert.deepEqual((await state()).deck.position, beforeRoute.deck.position);
  assert.equal((await state()).state.interior.mode, "walk");
  assert.equal(
    await page.evaluate(
      () => window.qaCanvas === document.querySelector("canvas"),
    ),
    true,
  );
  await page.setViewportSize({ width: 390, height: 844 });
  await page.waitForTimeout(500);
  assert.equal(
    await page.evaluate(
      () => document.documentElement.scrollWidth <= innerWidth,
    ),
    true,
  );
  const walkButton = page.getByRole("button", { name: "Walk backward" });
  assert.equal(await walkButton.isVisible(), true);
  const box = await walkButton.boundingBox();
  const mobileBefore = await state();
  await page.mouse.move(box.x + box.width / 2, box.y + box.height / 2);
  await page.mouse.down();
  await page.waitForTimeout(250);
  await page.mouse.up();
  assert.ok((await state()).deck.position[2] > mobileBefore.deck.position[2]);
  await page.waitForTimeout(250);
  const mobileStopped = await state();
  await page.waitForTimeout(250);
  assert.deepEqual((await state()).deck.position, mobileStopped.deck.position);
  await capture("mobile");
  await page.getByRole("button", { name: /^Use command console/ }).click();
  assert.equal(
    await page
      .getByRole("button", { name: "Forward thrust", exact: true })
      .count(),
    0,
  );
  assert.equal(
    await page
      .getByRole("button", { name: "Hold position", exact: true })
      .isVisible(),
    true,
  );
  await capture("mobile-helm");
  check(
    "Settings and quality choices preserve the visit; narrow layouts expose walking and command orders without overflow",
  );

  await page.evaluate(() => {
    const gl = document.querySelector("canvas").getContext("webgl2");
    window.qaLoss = gl.getExtension("WEBGL_lose_context");
    window.qaLoss.loseContext();
  });
  await page.waitForFunction(() => window.aquilaDiagnostics().contextLost);
  await page.getByRole("alert").waitFor();
  await page.evaluate(() => window.qaLoss.restoreContext());
  await page.waitForFunction(() => !window.aquilaDiagnostics().contextLost);
  await page.getByRole("alert").waitFor({ state: "hidden" });
  assert.equal((await state()).state.paused, true);
  await page.getByRole("button", { name: "Resume flight" }).click();
  await page.waitForTimeout(500);
  const beforeReload = await state();
  await page.reload();
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 10,
  );
  const reloaded = await state();
  assert.equal(reloaded.ship, "resolute");
  assert.equal(reloaded.state.interior, null);
  assert.ok(
    difference(reloaded.state.position, beforeReload.state.position) < 1,
  );
  assert.equal(reloaded.canvasCount, 1);
  check(
    "Context restoration pauses safely; reload resumes the same battleship from exterior flight",
  );
  assert.deepEqual(errors, []);
  const webgl = await page.evaluate(() => {
    const gl = document.querySelector("canvas").getContext("webgl2"),
      ext = gl.getExtension("WEBGL_debug_renderer_info");
    return ext ? gl.getParameter(ext.UNMASKED_RENDERER_WEBGL) : "unavailable";
  });
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    webgl,
    prefix,
    checks,
    errors,
    final: reloaded,
  };
  await fs.writeFile(
    ".runtime/interior-browser-qa.json",
    JSON.stringify(receipt, null, 2),
  );
  console.log(
    JSON.stringify({
      passed: checks.length,
      browser: receipt.browser,
      webgl,
      errors,
    }),
  );
} catch (error) {
  await page.screenshot({ path: ".runtime/interior-failure.png" });
  const failure = {
    error: String(error),
    checks,
    errors,
    state: await state(),
  };
  await fs.writeFile(
    ".runtime/interior-failure.json",
    JSON.stringify(failure, null, 2),
  );
  console.log(
    JSON.stringify({
      error: failure.error,
      errors,
      deck: failure.state.deck,
      interior: failure.state.state.interior,
    }),
  );
  throw error;
} finally {
  await browser.close();
}
