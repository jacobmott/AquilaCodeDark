// Install isolated tooling: npm install --prefix .runtime/browser-qa --ignore-scripts playwright-core@1.58.2
// Start the local development server, then: node scripts/qa-universe.mjs
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";

const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const out = "reports/game-direction/media";
// Version captures so later runs never replace pictures in published rounds.
const capturePrefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-";
assert.match(capturePrefix, /^[a-zA-Z0-9-]+$/);
await fs.mkdir(out, { recursive: true });
const browser = await chromium.launch({ channel: "chrome", headless: true });
const context = await browser.newContext({
  viewport: { width: 1440, height: 900 },
  deviceScaleFactor: 1,
});
const page = await context.newPage();
const errors = [],
  checks = [];
page.on("pageerror", (e) => errors.push(String(e)));
page.on("console", (m) => {
  if (m.type() === "error") errors.push(m.text());
});
const state = () => page.evaluate(() => window.aquilaDiagnostics());
const capture = async (name) => {
  await page.waitForTimeout(450);
  await page.screenshot({ path: `${out}/${capturePrefix}${name}.png` });
};
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log(`PASS ${name}`);
};
const hold = async (key, ms) => {
  await page.keyboard.down(key);
  await page.waitForTimeout(ms);
  await page.keyboard.up(key);
};

try {
  await page.goto(`${base}/home`);
  await page.waitForFunction(() => !!window.aquilaDiagnostics);
  await page.waitForTimeout(1800);
  assert.equal((await state()).canvasCount, 1);
  await page.evaluate(
    () => (window.qaCanvas = document.querySelector("canvas")),
  );
  await capture("home-desktop");
  await page.getByRole("link", { name: "Fleet", exact: true }).click();
  await page.getByRole("button", { name: /02 Osprey/ }).click();
  assert.equal((await state()).ship, "osprey");
  await page.getByRole("button", { name: /03 Aegis/ }).click();
  assert.equal((await state()).ship, "aegis");
  await page.getByRole("button", { name: /01 Peregrine/ }).click();
  await page.waitForTimeout(1800);
  assert.ok(
    await page.evaluate(
      () => window.qaCanvas === document.querySelector("canvas"),
    ),
  );
  await capture("fleet-desktop");
  check("One persistent canvas; the three existing shuttle selections work");
  await page.getByRole("button", { name: "Fly the Peregrine" }).click();
  await page.waitForTimeout(1500);
  const start = (await state()).state.position;
  await hold("w", 800);
  await page.waitForTimeout(250);
  const forward = (await state()).state.position;
  assert.ok(forward[2] < start[2] - 20);
  await hold("e", 650);
  const above = (await state()).state.position;
  assert.ok(above[1] > forward[1] + 20);
  await hold("q", 1000);
  const below = (await state()).state.position;
  assert.ok(below[1] < above[1] - 20);
  await hold("ArrowRight", 500);
  assert.ok(Math.abs((await state()).yaw) > 0.15);
  await hold("ArrowUp", 400);
  assert.ok((await state()).pitch > 0.1);
  await hold("a", 400);
  const beforeDrag = await state();
  await page.mouse.move(770, 320);
  await page.mouse.down();
  await page.mouse.move(850, 350, { steps: 8 });
  await page.mouse.up();
  assert.notEqual((await state()).yaw, beforeDrag.yaw);
  const beforeRight = await state();
  await page.mouse.move(770, 320);
  await page.mouse.down({ button: "right" });
  await page.mouse.move(830, 340, { steps: 6 });
  await page.mouse.up({ button: "right" });
  const afterRight = await state();
  assert.equal(afterRight.yaw, beforeRight.yaw);
  assert.equal(afterRight.pitch, beforeRight.pitch);
  assert.notDeepEqual(afterRight.camera, beforeRight.camera);
  await page.mouse.move(700, 260, { steps: 6 });
  assert.equal((await state()).yaw, afterRight.yaw);
  const menus = await page.evaluate(() => {
    const cancelled = (element) => {
      const event = new MouseEvent("contextmenu", {
        bubbles: true,
        cancelable: true,
        button: 2,
      });
      element.dispatchEvent(event);
      return event.defaultPrevented;
    };
    return {
      canvas: cancelled(document.querySelector("canvas")),
      ui: cancelled(document.querySelector(".navigation a")),
    };
  });
  assert.deepEqual(menus, { canvas: true, ui: false });
  check(
    "Right-drag moves the camera without steering; browser menus remain available on UI",
  );
  await page.keyboard.press("Space");
  await page.waitForTimeout(600);
  check(
    "Keyboard thrust, climb, descent, turn, pitch, strafe, and mouse steering",
  );
  await capture("flight-desktop");
  await page.keyboard.press("Escape");
  assert.equal((await state()).state.paused, true);
  const paused = (await state()).state.position;
  await hold("w", 300);
  assert.deepEqual((await state()).state.position, paused);
  const pausedYaw = (await state()).yaw;
  await page.mouse.move(800, 250);
  await page.mouse.down({ button: "right" });
  await page.mouse.move(880, 280, { steps: 5 });
  await page.mouse.up({ button: "right" });
  assert.equal((await state()).yaw, pausedYaw);
  await capture("pause-desktop");
  await page.getByRole("button", { name: "Resume flight" }).click();
  await page
    .getByRole("button", { name: "Flight controls", exact: true })
    .click();
  assert.equal((await state()).state.paused, true);
  await capture("help-desktop");
  await page.keyboard.press("Escape");
  await page.locator(".help-panel").waitFor({ state: "hidden" });
  assert.equal(await page.locator(".help-panel").count(), 0);
  assert.equal((await state()).state.paused, false);
  check("Pause and controls overlay prevent flight; Escape closes controls");
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  assert.equal(
    await page.evaluate(() => {
      const event = new MouseEvent("contextmenu", {
        bubbles: true,
        cancelable: true,
        button: 2,
      });
      document.querySelector("canvas").dispatchEvent(event);
      return event.defaultPrevented;
    }),
    false,
  );
  const settingsPosition = (await state()).state.position;
  await page
    .getByRole("radiogroup", { name: "Visual quality" })
    .getByText("Light", { exact: true })
    .click();
  await page.getByLabel("Reduced motion").check();
  await page.getByLabel("Steering sensitivity").focus();
  await hold("w", 300);
  assert.deepEqual((await state()).state.position, settingsPosition);
  assert.equal(
    await page.evaluate(
      () => JSON.parse(localStorage.getItem("aquila.preferences.v1")).quality,
    ),
    "low",
  );
  await capture("settings-desktop");
  check("Settings persist; form input does not fly the ship");
  await page.getByRole("button", { name: "Return to Vesper" }).click();
  await page.getByRole("button", { name: "Approach orbit" }).click();
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.canDescend,
    {},
    { timeout: 20000 },
  );
  const orbit = (await state()).state.position;
  await page.getByRole("button", { name: "Descend to planet" }).click();
  assert.equal((await state()).state.surface, true);
  await page.waitForTimeout(2000);
  await capture("surface-desktop");
  const altitude = (await state()).state.altitude;
  await hold("e", 600);
  assert.ok((await state()).state.altitude > altitude + 10);
  const beforeSurfaceDrag = (await state()).state.altitude;
  await page.mouse.move(770, 320);
  await page.mouse.down({ button: "left" });
  await page.mouse.move(820, 350, { steps: 5 });
  await page.mouse.up({ button: "left" });
  assert.ok((await state()).state.altitude < beforeSurfaceDrag - 10);
  await hold("w", 1000);
  assert.ok((await state()).state.speed > 30);
  await page.getByRole("button", { name: "Return to orbit" }).click();
  assert.equal((await state()).state.surface, false);
  assert.deepEqual((await state()).state.position, orbit);
  check(
    "Orbit assist, shuttle excursion, surface controls, and carrier return",
  );
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  await page.getByRole("button", { name: "Return to Vesper" }).click();
  await page.keyboard.down("Shift");
  await page.keyboard.down("d");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.sector !== "0:0:0",
    {},
    { timeout: 15000 },
  );
  await page.keyboard.up("d");
  await page.keyboard.up("Shift");
  assert.notEqual((await state()).state.sector, "0:0:0");
  await page.waitForFunction(
    () => window.aquilaDiagnostics().loadedSectors === 27,
  );
  check("Manual boosted flight crosses a sector boundary without an edge");
  await page.getByRole("link", { name: "Atlas", exact: true }).click();
  const departureSector = (await state()).state.sector;
  await capture("atlas-desktop");
  const destinations = (await state()).state.planets;
  const destinationIndex = destinations.findIndex(
    (p) => p.id !== departureSector,
  );
  assert.ok(destinationIndex >= 0);
  await page.locator(".destination").nth(destinationIndex).click();
  await page.waitForFunction(
    () =>
      !window.aquilaDiagnostics().state.course &&
      window.aquilaDiagnostics().state.canDescend,
    {},
    { timeout: 20000 },
  );
  const traveled = await state();
  assert.equal(traveled.state.nearest.id, destinations[destinationIndex].id);
  assert.notEqual(traveled.state.sector, departureSector);
  await page.waitForFunction(
    () => window.aquilaDiagnostics().loadedSectors === 27,
  );
  assert.ok(traveled.geometries < 400);
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  const save = await page.evaluate(() =>
    JSON.parse(localStorage.getItem("aquila.voyage.v1")),
  );
  await page.reload();
  await page.waitForFunction(() => !!window.aquilaDiagnostics);
  assert.deepEqual((await state()).state.position, save.position);
  assert.equal(
    await page.getByRole("radio", { name: "Light", exact: true }).isChecked(),
    true,
  );
  check(
    "Travel crosses sectors, loaded worlds stay bounded, reload restores voyage and preferences",
    { sector: traveled.state.sector, loaded: traveled.loadedSectors },
  );

  await page.getByRole("link", { name: "Home", exact: true }).click();
  await page.setViewportSize({ width: 390, height: 844 });
  await page.waitForTimeout(1500);
  await capture("home-mobile");
  for (const route of ["Fleet", "Atlas", "Settings"]) {
    await page.getByRole("link", { name: route, exact: true }).click();
    await page.waitForTimeout(600);
    assert.equal(
      await page.evaluate(
        () => document.documentElement.scrollWidth > innerWidth,
      ),
      false,
    );
    await capture(`${route.toLowerCase()}-mobile`);
  }
  await page.getByRole("button", { name: "Return to Vesper" }).click();
  await page.waitForTimeout(900);
  const mobileStart = (await state()).state.position;
  const thrust = page.getByRole("button", { name: "Forward thrust" });
  const box = await thrust.boundingBox();
  assert.ok(box);
  await page.mouse.move(box.x + box.width / 2, box.y + box.height / 2);
  await page.mouse.down();
  await page.waitForTimeout(700);
  await page.mouse.up();
  assert.ok((await state()).state.position[2] < mobileStart[2] - 20);
  // Wait for the observed slowdown, not a fixed number of wall-clock frames.
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.speed < 30,
    {},
    { timeout: 6000 },
  );
  await capture("flight-mobile");
  check("Phone layouts fit; pointer flight control releases without sticking");
  await page.evaluate(() => {
    const gl = document.querySelector("canvas").getContext("webgl2");
    window.qaLoss = gl.getExtension("WEBGL_lose_context");
    window.qaLoss.loseContext();
  });
  await page.waitForFunction(() => window.aquilaDiagnostics().contextLost);
  await page.locator("[role=alert]").waitFor({ state: "visible" });
  assert.equal(await page.locator("[role=alert]").count(), 1);
  await page.evaluate(() => window.qaLoss.restoreContext());
  await page.waitForFunction(() => !window.aquilaDiagnostics().contextLost);
  await page.locator("[role=alert]").waitFor({ state: "hidden" });
  assert.equal(await page.locator("[role=alert]").count(), 0);
  assert.equal((await state()).state.paused, true);
  check(
    "Graphics context loss shows recovery notice and restores safely while paused",
  );
  assert.deepEqual(errors, []);
  const webgl = await page.evaluate(() => {
    const gl = document.querySelector("canvas").getContext("webgl2"),
      ext = gl.getExtension("WEBGL_debug_renderer_info");
    return ext ? gl.getParameter(ext.UNMASKED_RENDERER_WEBGL) : "unavailable";
  });
  const fallback = await context.newPage();
  await fallback.addInitScript(() => {
    const original = HTMLCanvasElement.prototype.getContext;
    HTMLCanvasElement.prototype.getContext = function (type, ...args) {
      return type.startsWith("webgl")
        ? null
        : original.call(this, type, ...args);
    };
  });
  await fallback.goto(`${base}/home`);
  await fallback.getByRole("alert").waitFor();
  assert.match(await fallback.getByRole("alert").innerText(), /WebGL 2/);
  assert.equal(
    await fallback.getByRole("button", { name: /your voyage/ }).isDisabled(),
    true,
  );
  await fallback.getByRole("link", { name: "Fleet", exact: true }).click();
  await fallback.locator(".ship-option").first().waitFor({ state: "visible" });
  assert.equal(await fallback.locator(".ship-option").count(), 4);
  await fallback.close();
  check("Unavailable graphics shows a readable fallback and disables flight");
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    webgl,
    capturePrefix,
    checks,
    errors,
    final: await state(),
  };
  await fs.writeFile(
    ".runtime/universe-browser-qa.json",
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
} finally {
  await browser.close();
}
