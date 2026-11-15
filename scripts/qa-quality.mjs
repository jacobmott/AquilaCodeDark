// Run against the local development preview using isolated browser tooling.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";

const browser = await chromium.launch({ channel: "chrome", headless: true });
const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const errors = [],
  checks = [];
await fs.mkdir("reports/game-direction/media", { recursive: true });
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log(`PASS ${name}`);
};
const state = (page) => page.evaluate(() => window.aquilaDiagnostics());
const storage = (page) =>
  page.evaluate(() =>
    JSON.parse(localStorage.getItem("aquila.preferences.v1")),
  );
const choose = (page, name) =>
  page
    .getByRole("radiogroup", { name: "Visual quality" })
    .getByText(name, { exact: true })
    .click();

try {
  const context = await browser.newContext({
    viewport: { width: 1280, height: 800 },
    deviceScaleFactor: 2.4,
  });
  const page = await context.newPage();
  page.on("pageerror", (e) => errors.push(String(e)));
  page.on("console", (m) => {
    if (m.type() === "error") errors.push(m.text());
  });
  await page.goto(`${base}/settings`);
  await page
    .getByRole("radio", { name: "Balanced", exact: true })
    .waitFor({ state: "attached" });
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 2,
  );
  assert.equal(await page.locator("select").count(), 0);
  const starting = await state(page);
  await page.evaluate(() => {
    window.qaCanvas = document.querySelector("canvas");
    window.qaBufferChanges = [];
    new MutationObserver((records) =>
      window.qaBufferChanges.push(...records.map((r) => r.attributeName)),
    ).observe(window.qaCanvas, {
      attributes: true,
      attributeFilter: ["width", "height"],
    });
  });
  const selections = [];
  for (const [label, value, ratio] of [
    ["Light", "low", 1],
    ["Balanced", "balanced", 1.4],
    ["High", "high", 2],
    ["Balanced", "balanced", 1.4],
  ]) {
    await page.evaluate(() => (window.qaBufferChanges = []));
    const before = await state(page),
      started = Date.now();
    await choose(page, label);
    await page.waitForFunction(
      (r) => window.aquilaDiagnostics().pixelRatio === r,
      ratio,
    );
    await page.waitForFunction(
      (frame) => window.aquilaDiagnostics().renderedFrames > frame + 2,
      before.renderedFrames,
    );
    assert.equal((await storage(page)).quality, value);
    assert.equal(
      await page.getByRole("radio", { name: label, exact: true }).isChecked(),
      true,
    );
    const observed = await page.evaluate(() => ({
      changes: window.qaBufferChanges,
      width: window.qaCanvas.width,
      height: window.qaCanvas.height,
      sameCanvas: window.qaCanvas === document.querySelector("canvas"),
    }));
    assert.deepEqual(observed.changes.sort(), ["height", "width"]);
    assert.equal(observed.width, Math.floor(1280 * ratio));
    assert.equal(observed.height, Math.floor(800 * ratio));
    assert.equal(observed.sameCanvas, true);
    selections.push({
      value,
      elapsedMs: Date.now() - started,
      width: observed.width,
      height: observed.height,
    });
  }
  check(
    "Pointer selection changes all three quality modes and rendering continues at high display density",
    selections,
  );

  await page.evaluate(() => (window.qaBufferChanges = []));
  await choose(page, "Balanced");
  await page.getByLabel("Reduced motion").check();
  await page.getByLabel("Steering sensitivity").focus();
  await page.keyboard.press("End");
  await page.waitForTimeout(300);
  assert.deepEqual(await page.evaluate(() => window.qaBufferChanges), []);
  check(
    "Repeated selection, motion and sensitivity changes do not reset the graphics buffer",
  );

  await page.getByRole("radio", { name: "Balanced", exact: true }).focus();
  await page.keyboard.press("ArrowRight");
  assert.equal((await storage(page)).quality, "high");
  await page.keyboard.press("ArrowLeft");
  assert.equal((await storage(page)).quality, "balanced");
  assert.deepEqual((await state(page)).state.position, starting.state.position);
  assert.equal((await state(page)).yaw, starting.yaw);
  assert.equal((await state(page)).pitch, starting.pitch);
  await page.locator(".settings-view").evaluate(async (element) => {
    await Promise.all(
      element.getAnimations().map((animation) => animation.finished),
    );
  });
  await page.screenshot({
    path: `reports/game-direction/media/${prefix}quality-desktop.png`,
  });
  await page.reload();
  await page.waitForFunction(() => !!window.aquilaDiagnostics());
  assert.equal(
    await page
      .getByRole("radio", { name: "Balanced", exact: true })
      .isChecked(),
    true,
  );
  await page.getByRole("link", { name: "Fleet", exact: true }).click();
  await page.locator(".ship-option").first().waitFor({ state: "visible" });
  check(
    "Arrow keys select quality without steering; saved choice survives reload and navigation remains responsive",
  );

  const mobileContext = await browser.newContext({
    viewport: { width: 390, height: 844 },
    deviceScaleFactor: 1,
    hasTouch: true,
  });
  const phone = await mobileContext.newPage();
  phone.on("pageerror", (e) => errors.push(String(e)));
  await phone.goto(`${base}/settings`);
  const group = phone.getByRole("radiogroup", { name: "Visual quality" });
  await group.waitFor();
  await phone.evaluate(() => {
    window.qaBufferChanges = [];
    new MutationObserver((records) =>
      window.qaBufferChanges.push(...records.map((r) => r.attributeName)),
    ).observe(document.querySelector("canvas"), {
      attributes: true,
      attributeFilter: ["width", "height"],
    });
  });
  for (const [label, value] of [
    ["Light", "low"],
    ["High", "high"],
    ["Balanced", "balanced"],
  ]) {
    await group.getByText(label, { exact: true }).tap();
    assert.equal((await storage(phone)).quality, value);
    assert.equal(
      await phone.getByRole("radio", { name: label, exact: true }).isChecked(),
      true,
    );
  }
  assert.deepEqual(await phone.evaluate(() => window.qaBufferChanges), []);
  assert.equal(
    await phone.evaluate(
      () => document.documentElement.scrollWidth > innerWidth,
    ),
    false,
  );
  await phone.locator(".settings-view").evaluate(async (element) => {
    await Promise.all(
      element.getAnimations().map((animation) => animation.finished),
    );
  });
  await phone.screenshot({
    path: `reports/game-direction/media/${prefix}quality-mobile.png`,
  });
  check(
    "Touch selection works at phone size; equal physical resolutions cause no redundant buffer reset",
  );
  assert.deepEqual(errors, []);
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    checks,
    errors,
    capturePrefix: prefix,
  };
  await fs.writeFile(
    ".runtime/quality-browser-qa.json",
    JSON.stringify(receipt, null, 2),
  );
  console.log(JSON.stringify({ passed: checks.length, errors }));
} finally {
  await browser.close();
}
