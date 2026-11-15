// Native orders and input in isolated Chrome. Diagnostics only observe.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";
const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-navigation-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const browser = await chromium.launch({ channel: "chrome", headless: true });
const page = await browser.newPage({ viewport: { width: 1440, height: 900 } });
page.setDefaultTimeout(30000);
const checks = [],
  errors = [];
page.on("pageerror", (e) => errors.push(String(e)));
page.on("console", (m) => {
  if (m.type() === "error") errors.push(m.text());
});
const state = () => page.evaluate(() => window.aquilaDiagnostics());
const click = (name) => page.getByRole("button", { name }).click();
const difference = (a, b) => Math.hypot(...a.map((n, i) => n - b[i]));
const capture = (name) =>
  page.screenshot({
    path: "reports/game-direction/media/" + prefix + name + ".png",
  });
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log("PASS", name);
};
const hold = async (key, ms) => {
  await page.keyboard.down(key);
  await page.waitForTimeout(ms);
  await page.keyboard.up(key);
};
const arrive = () =>
  page.waitForFunction(() => !window.aquilaDiagnostics().state.course);
try {
  await page.goto(base + "/play");
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.loadedSectors === 27,
  );
  const initial = await state();
  assert.equal(initial.state.navigation.order, "hold");
  assert.equal(
    await page
      .getByRole("button", { name: "Forward thrust", exact: true })
      .count(),
    0,
  );
  assert.equal(await page.locator(".flight-hints").count(), 0);
  for (const key of [
    "w",
    "s",
    "a",
    "d",
    "q",
    "e",
    "ArrowUp",
    "ArrowRight",
    "Shift",
    "Space",
  ])
    await hold(key, 70);
  assert.deepEqual((await state()).state.position, initial.state.position);
  assert.equal((await state()).yaw, initial.yaw);
  assert.equal((await state()).pitch, initial.pitch);
  for (const button of ["left", "right"]) {
    await page.mouse.move(600, 410);
    await page.mouse.down({ button });
    await page.mouse.move(760, 460, { steps: 8 });
    const orbited = await state();
    assert.equal(orbited.yaw, initial.yaw);
    assert.ok(Math.abs(orbited.cameraOrbit.yaw) > 0.2);
    await page.mouse.up({ button });
    await page.waitForFunction(
      () => window.aquilaDiagnostics().cameraOrbit.yaw === 0,
    );
  }
  await click("Flight controls");
  assert.equal(await page.locator(".help-panel .controls-list").count(), 0);
  assert.ok(
    !(await page.locator(".help-panel").innerText()).includes("T locks"),
  );
  await click("Close flight controls");
  check(
    "Battleship hides manual flight controls; keys cannot move it; either drag looks and releases behind the ship",
  );

  await click("Approach orbit");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.speed > 100,
  );
  await hold("w", 200);
  assert.ok((await state()).state.course);
  await capture("approach");
  await click("Enter bridge");
  const walking = await state();
  await page.keyboard.down("w");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().deck.position[2] < -2.3,
  );
  await page.keyboard.up("w");
  const walked = await state();
  assert.ok(difference(walked.state.position, walking.state.position) > 500);
  assert.equal(walked.state.navigation.canOrder, false);
  await page.keyboard.press("f");
  assert.equal((await state()).state.interior.mode, "helm");
  assert.equal(
    await page
      .getByRole("button", { name: "Forward thrust", exact: true })
      .count(),
    0,
  );
  await page.keyboard.press("Escape");
  const paused = await state();
  assert.ok(paused.state.course);
  await page.waitForTimeout(300);
  assert.deepEqual((await state()).state.position, paused.state.position);
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  await page.waitForTimeout(300);
  assert.deepEqual((await state()).state.position, paused.state.position);
  assert.equal((await state()).state.course, paused.state.course);
  await click("Return to your ship");
  await page.waitForFunction(
    (p) =>
      Math.hypot(
        ...window.aquilaDiagnostics().state.position.map((n, i) => n - p[i]),
      ) > 100,
    paused.state.position,
  );
  await click("Hold position");
  const held = await state();
  await hold("w", 300);
  assert.deepEqual((await state()).state.position, held.state.position);
  assert.equal(held.state.course, null);
  await capture("command-console");
  check(
    "Automatic course continues while walking; helm orders replace piloting; pause/settings retain orders and Hold position cancels them",
  );

  await click("Approach orbit");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.speed > 100,
  );
  await page.evaluate(() => {
    const gl = document.querySelector("canvas").getContext("webgl2");
    window.qaNavigationLoss = gl.getExtension("WEBGL_lose_context");
    window.qaNavigationLoss.loseContext();
  });
  await page.waitForFunction(() => window.aquilaDiagnostics().contextLost);
  const lost = await state();
  await page.waitForTimeout(300);
  await page.evaluate(() => window.qaNavigationLoss.restoreContext());
  await page.waitForFunction(() => !window.aquilaDiagnostics().contextLost);
  assert.equal((await state()).state.paused, true);
  assert.equal((await state()).state.course, lost.state.course);
  assert.deepEqual((await state()).state.position, lost.state.position);
  await click("Resume flight");
  await arrive();
  const orbit = await state();
  assert.match(orbit.state.navigation.status, /Station keeping/);
  assert.ok(
    orbit.state.distance >
      Math.max(3500, orbit.state.nearest.radius * 0.065) + orbit.hullRadius,
  );
  await page.keyboard.press("c");
  await page.waitForFunction(() => window.aquilaDiagnostics().state.canDescend);
  await click("Launch survey shuttle");
  await page.waitForFunction(
    () =>
      window.aquilaDiagnostics().state.surface &&
      !window.aquilaDiagnostics().state.descending,
  );
  assert.equal((await state()).state.navigation, null);
  await hold("w", 500);
  assert.ok((await state()).state.speed > 10);
  await click("Return to battleship");
  assert.equal((await state()).state.navigation.order, "hold");
  check(
    "Graphics recovery preserves paused navigation; arrival respects planetary hull clearance; survey shuttle retains manual movement",
  );

  await click("Inspect with shuttle");
  const inspecting = await state();
  assert.equal(inspecting.state.navigation, null);
  await hold("s", 700);
  assert.ok(
    difference((await state()).state.position, inspecting.state.position) > 10,
  );
  await hold("ArrowLeft", 250);
  assert.notEqual((await state()).yaw, inspecting.yaw);
  await click("Resume command");
  assert.deepEqual((await state()).state.position, inspecting.carrier.position);
  check(
    "Exterior inspection keeps shuttle thrust and steering; returning restores a stationary battleship",
  );

  // Return to open space using an existing native reset, with no diagnostic mutation.
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  await click("Return to Vesper");
  await click("Start practice");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.fired >= 2,
  );
  await click("Hold fire");
  const target = (await state()).practice.target;
  await click("Keep combat range");
  await click("Piercing rounds");
  assert.equal((await state()).state.combat.ammunition, "piercing");
  await arrive();
  assert.ok(Math.abs((await state()).state.combat.range - 35000) < 100);
  assert.deepEqual((await state()).practice.target, target);
  await click("Withdraw");
  await click("Return aboard");
  const departing = await state();
  assert.equal(departing.state.interior.mode, "walk");
  await arrive();
  const withdrawn = await state();
  assert.ok(withdrawn.state.combat.range >= 84000);
  assert.equal(withdrawn.state.combat.locked, false);
  assert.ok(
    difference(withdrawn.state.position, departing.state.position) > 1000,
  );
  await page.waitForTimeout(400);
  assert.equal(
    (await state()).state.combat.received,
    withdrawn.state.combat.received,
  );
  await page.keyboard.press("c");
  await click("Keep combat range");
  await click("Automatic");
  await arrive();
  await click("Hold position");
  assert.ok((await state()).state.combat.locked);
  await capture("combat-orders");
  check(
    "Keep range and withdrawal move the ship without piloting, work while walking and preserve independent weapon modes",
  );

  await page.setViewportSize({ width: 390, height: 844 });
  await click("Hold fire");
  await click("Withdraw");
  const panel = await page.locator(".combat-panel").boundingBox();
  assert.ok(
    panel.x >= 0 &&
      panel.x + panel.width <= 390 &&
      panel.y >= 0 &&
      panel.y + panel.height <= 844,
  );
  await page
    .getByRole("button", { name: "Withdraw", exact: true })
    .scrollIntoViewIfNeeded();
  await capture("mobile-orders");
  await click("Hold position");
  await click("End practice");
  assert.equal((await state()).state.navigation.order, "hold");
  await click("Approach orbit");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.speed > 100,
  );
  await page.reload();
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 10,
  );
  assert.equal((await state()).state.navigation.order, "hold");
  assert.equal((await state()).state.course, null);
  assert.equal((await state()).canvasCount, 1);
  check(
    "Narrow controls fit and operate; ending practice clears combat orders; reload restores position with no stale navigation",
  );
  assert.deepEqual(errors, []);
  await fs.writeFile(
    ".runtime/navigation-browser-qa.json",
    JSON.stringify(
      {
        time: new Date().toISOString(),
        base,
        prefix,
        browser: await browser.version(),
        checks,
        errors,
        initial,
        orbit,
        withdrawn,
        final: await state(),
      },
      null,
      2,
    ),
  );
  console.log(JSON.stringify({ passed: checks.length, errors }));
} catch (error) {
  await page.screenshot({ path: ".runtime/navigation-failure.png" });
  await fs.writeFile(
    ".runtime/navigation-failure.json",
    JSON.stringify(
      { error: String(error), checks, errors, state: await state() },
      null,
      2,
    ),
  );
  throw error;
} finally {
  await browser.close();
}
