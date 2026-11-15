// Native controls in isolated Chrome. Diagnostics only observe; no simulation setters.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";
const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-projectile-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const browser = await chromium.launch({ channel: "chrome", headless: true });
const page = await browser.newPage({ viewport: { width: 1440, height: 900 } });
page.setDefaultTimeout(45000);
const checks = [],
  errors = [];
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
    path: "reports/game-direction/media/" + prefix + name + ".png",
  });
const click = (name) => page.getByRole("button", { name }).click();
const hold = async (key, ms) => {
  await page.keyboard.down(key);
  await page.waitForTimeout(ms);
  await page.keyboard.up(key);
};
const move = async (key, axis, target) => {
  const before = (await state()).deck.position[axis];
  await page.keyboard.down(key);
  try {
    await page.waitForFunction(
      ({ axis, target, greater }) =>
        greater
          ? window.aquilaDiagnostics().deck.position[axis] >= target
          : window.aquilaDiagnostics().deck.position[axis] <= target,
      { axis, target, greater: target > before },
    );
  } finally {
    await page.keyboard.up(key);
  }
};
const frozen = (s) => ({
  elapsed: s.practice.elapsed,
  projectiles: s.practice.projectiles,
  shield: s.state.combat.shield,
  hull: s.state.combat.hull,
});
try {
  await page.goto(base + "/play");
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.loadedSectors === 27,
  );
  await click("Enter bridge");
  await move("w", 2, -2.3);
  await move("d", 0, 6.3);
  await page.keyboard.press("f");
  assert.equal((await state()).state.interior.mode, "tactical");
  await click("Start practice");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.fired >= 2,
  );
  const first = await state();
  assert.equal(first.state.combat.targetShield, 100);
  assert.equal(first.practice.hits, 0);
  assert.equal(first.state.combat.mode, "automatic");
  assert.equal(first.state.combat.ammunition, "standard");
  assert.ok(first.practice.projectiles.length >= 2);
  assert.equal(
    await page
      .getByRole("button", { name: /Lock Sentinel|Fire pulse|Release lock/ })
      .count(),
    0,
  );
  await page.waitForFunction(
    () => window.aquilaDiagnostics().practice.elapsed > 1.9,
  );
  await page.keyboard.press("Escape");
  await capture("bridge-stream");
  const moving = await state();
  assert.ok(moving.practice.projectiles.length >= 6);
  assert.equal(moving.state.combat.targetShield, 100);
  check(
    "Two automatic turrets launch visible separated rounds before any impact damage",
  );
  await click("Resume flight");
  await click("Hold fire");
  const held = await state();
  await hold("x", 300);
  await page.keyboard.press("t");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().practice.incomingHits > 0,
  );
  const incoming = await state();
  assert.equal(incoming.state.combat.fired, held.state.combat.fired);
  assert.ok(incoming.state.combat.targetShield < 100);
  assert.ok(incoming.state.combat.shield < 100);
  assert.equal(incoming.state.combat.hull, 100);
  check(
    "Hold fire preserves flying rounds; both sides take damage only after arrival; old aim/trigger shortcuts remain absent",
  );

  await click("Automatic");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().practice.blocked > 0,
  );
  const blocked = await state();
  assert.equal(blocked.state.combat.targetShield, 0);
  assert.equal(blocked.state.combat.targetHull, 100);
  assert.match(blocked.state.combat.reason, /No penetration/);
  await page.waitForTimeout(2200);
  const stillBlocked = await state();
  assert.equal(stillBlocked.state.combat.fired, blocked.state.combat.fired);
  assert.equal(stillBlocked.state.combat.targetHull, 100);
  await page.keyboard.press("Escape");
  await capture("blocked");
  check(
    "Standard rounds break the shield but cannot chip reinforced armor; fire control reports and stops ineffective new shots",
  );

  const paused = await state();
  await page.waitForTimeout(350);
  assert.deepEqual(frozen(await state()), frozen(paused));
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  const routed = await state();
  for (const quality of ["Light", "High", "Balanced"])
    await page
      .getByRole("radiogroup", { name: "Visual quality" })
      .getByText(quality, { exact: true })
      .click();
  assert.deepEqual(frozen(await state()), frozen(routed));
  await click("Return to your ship");
  await click("Piercing rounds");
  const piercing = await state();
  assert.equal(piercing.state.combat.ammunition, "piercing");
  await page.keyboard.press("f");
  assert.equal((await state()).state.interior.mode, "walk");
  assert.equal(
    await page
      .getByRole("button", { name: "Standard rounds", exact: true })
      .isDisabled(),
    true,
  );
  await page.waitForFunction(
    (n) => window.aquilaDiagnostics().state.combat.fired > n,
    piercing.state.combat.fired,
  );
  await page.keyboard.press("f");
  await page.keyboard.press("c");
  const outside = await state();
  assert.equal(outside.state.interior, null);
  assert.equal(outside.state.combat.ammunition, "piercing");
  assert.deepEqual(outside.practice.target, piercing.practice.target);
  await page.waitForFunction(
    () => window.aquilaDiagnostics().practice.penetrated > 0,
  );
  await page.keyboard.press("Escape");
  await capture("penetration");
  const penetrated = await state();
  assert.ok(penetrated.state.combat.targetHull < 100);
  assert.match(penetrated.state.combat.lastImpact, /PENETRATED/);
  check(
    "Ammunition modes carry between bridge, walking and exterior; piercing rounds penetrate; pause and quality changes preserve projectile state",
  );
  await click("Resume flight");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.status === "victory",
  );
  const won = await state();
  assert.equal(won.state.combat.targetHull, 0);
  assert.equal(won.practice.projectiles.length, 0);
  await page.waitForTimeout(1000);
  assert.equal(
    (await state()).state.combat.received,
    won.state.combat.received,
  );
  check(
    "Piercing ammunition completes the automatic encounter; victory stops both guns and clears active rounds",
  );

  const position = (await state()).state.position;
  await click("Reset practice");
  const reset = await state();
  assert.deepEqual(reset.state.position, position);
  assert.equal(reset.state.combat.ammunition, "standard");
  assert.equal(
    reset.state.combat.shield +
      reset.state.combat.hull +
      reset.state.combat.targetShield +
      reset.state.combat.targetHull,
    400,
  );
  assert.equal(reset.practice.blocked, 0);
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.fired >= 1,
  );
  await click("Hold fire");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.shield === 0,
  );
  const shieldFailed = await state();
  assert.ok(shieldFailed.state.combat.hull <= 100);
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.status === "defeat",
    null,
    { timeout: 60000 },
  );
  const lost = await state();
  await hold("w", 300);
  assert.deepEqual((await state()).state.position, lost.state.position);
  assert.equal(lost.practice.projectiles.length, 0);
  check(
    "Incoming kinetics collapse the single shield and damage hull; defeat stops flight; reset restores health and standard ammunition",
  );

  await click("Reset practice");
  await page.waitForTimeout(200);
  const allocated = await state();
  for (let i = 0; i < 5; i++) {
    await click("End practice");
    await click("Start practice");
    await page.waitForTimeout(100);
  }
  const reused = await state();
  assert.equal(reused.geometries, allocated.geometries);
  assert.equal(reused.textures, allocated.textures);
  assert.equal(reused.practice.effectPool, 32);
  assert.equal(reused.practice.projectilePool, 160);
  assert.equal(reused.practice.saturated, 0);
  await page.setViewportSize({ width: 390, height: 844 });
  await click("Piercing rounds");
  await click("Hold fire");
  const bounds = await page.locator(".combat-panel").boundingBox();
  assert.ok(
    bounds.x >= 0 &&
      bounds.x + bounds.width <= 390 &&
      bounds.y >= 0 &&
      bounds.y + bounds.height <= 844,
  );
  await capture("mobile");
  check(
    "Repeated practice reuses geometry and fixed pools; narrow controls fit and change ammunition and weapon mode",
    {
      geometries: reused.geometries,
      textures: reused.textures,
      pool: 160,
      impactPool: 32,
    },
  );

  await page.setViewportSize({ width: 1440, height: 900 });
  await click("Reset practice");
  await click("Withdraw");
  await page.waitForFunction(
    () =>
      window.aquilaDiagnostics().state.combat.range > 75000 &&
      !window.aquilaDiagnostics().state.course,
  );
  const distant = await state();
  assert.equal(distant.state.combat.locked, false);
  await page.waitForTimeout(700);
  assert.equal((await state()).state.combat.fired, distant.state.combat.fired);
  assert.equal(
    (await state()).state.combat.received,
    distant.state.combat.received,
  );
  await click("Keep combat range");
  await page.waitForFunction(
    () =>
      window.aquilaDiagnostics().state.combat.range < 50000 &&
      !window.aquilaDiagnostics().state.course,
  );
  await click("Hold position");
  await page.waitForFunction(
    (n) => window.aquilaDiagnostics().state.combat.fired > n,
    distant.state.combat.fired,
  );
  assert.equal((await state()).state.combat.locked, true);
  check(
    "Range still limits new launches; return reacquires and resumes automatic fire",
  );

  await click("Reset practice");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.combat.activeProjectiles > 3,
  );
  await page.evaluate(() => {
    const gl = document.querySelector("canvas").getContext("webgl2");
    window.qaCombatLoss = gl.getExtension("WEBGL_lose_context");
    window.qaCombatLoss.loseContext();
  });
  await page.waitForFunction(() => window.aquilaDiagnostics().contextLost);
  const contextLost = await state();
  await page.waitForTimeout(300);
  await page.evaluate(() => window.qaCombatLoss.restoreContext());
  await page.waitForFunction(() => !window.aquilaDiagnostics().contextLost);
  assert.equal((await state()).state.paused, true);
  assert.deepEqual(frozen(await state()), frozen(contextLost));
  await click("Resume flight");
  await page.reload();
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 10,
  );
  assert.equal((await state()).state.combat, null);
  assert.equal((await state()).ship, "resolute");
  await click("Start practice");
  await page.getByRole("link", { name: "Atlas", exact: true }).click();
  await page.locator(".destination").first().click();
  assert.equal((await state()).state.combat, null);
  check(
    "Graphics recovery preserves paused rounds; reload and travel clear temporary practice",
  );
  assert.deepEqual(errors, []);
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    prefix,
    checks,
    errors,
    first,
    won,
    shieldFailed,
    allocated: reused,
    final: await state(),
  };
  await fs.writeFile(
    ".runtime/combat-browser-qa.json",
    JSON.stringify(receipt, null, 2),
  );
  console.log(JSON.stringify({ passed: checks.length, errors }));
} catch (error) {
  await page.screenshot({ path: ".runtime/combat-failure.png" });
  const failure = {
    error: String(error),
    checks,
    errors,
    state: await state(),
  };
  await fs.writeFile(
    ".runtime/combat-failure.json",
    JSON.stringify(failure, null, 2),
  );
  console.log(
    JSON.stringify({ error: String(error), passed: checks.length, errors }),
  );
  throw error;
} finally {
  await browser.close();
}
