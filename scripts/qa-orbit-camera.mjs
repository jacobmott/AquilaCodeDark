// Isolated native Chrome input checks. Development diagnostics are read-only.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";

const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-orbit-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const browser = await chromium.launch({ channel: "chrome", headless: true });
const context = await browser.newContext({
  viewport: { width: 1440, height: 900 },
});
const page = await context.newPage(),
  errors = [],
  checks = [];
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
const settled = () =>
  page.waitForFunction(() => {
    const o = window.aquilaDiagnostics().cameraOrbit;
    return !o.held && o.yaw === 0 && o.pitch === 0 && o.focus === 0;
  });
const startOrbit = async () => {
  await page.mouse.move(1100, 400);
  await page.mouse.down({ button: "right" });
  await page.mouse.move(790, 400, { steps: 12 });
  await page.waitForTimeout(700);
};
try {
  await page.goto(`${base}/play`);
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 20,
  );
  await page.waitForTimeout(2000);
  const initial = await state();
  const dimensions = initial.solidBounds.max.map(
    (v, i) => v - initial.solidBounds.min[i],
  );
  for (const [i, expected] of [4800, 1280, 13600].entries())
    assert.ok(Math.abs(dimensions[i] - expected) < 0.01);
  assert.equal(initial.hangars[1].approach[1], -180);
  assert.ok(Math.abs(initial.hangars[1].height - 480) < 0.001);
  await capture("chase");
  await startOrbit();
  const side = await state();
  assert.equal(side.cameraOrbit.held, true);
  assert.equal(side.yaw, initial.yaw);
  assert.equal(side.pitch, initial.pitch);
  assert.deepEqual(side.state.position, initial.state.position);
  assert.ok(side.camera[0] > 15000);
  await capture("side");
  await page.mouse.move(472, 400, { steps: 12 });
  await page.waitForTimeout(100);
  assert.ok((await state()).camera[2] < -18000);
  await page.mouse.move(472, 800, { steps: 12 });
  assert.ok((await state()).camera[1] > 18000);
  await page.mouse.move(472, 100, { steps: 12 });
  assert.ok((await state()).camera[1] < -15000);
  // Observe the full return, ensuring no straight chord crosses the hull.
  await page.evaluate(() => {
    window.qaCameraRadii = [];
    const end = performance.now() + 1600;
    function sample() {
      window.qaCameraRadii.push(
        Math.hypot(...window.aquilaDiagnostics().camera),
      );
      if (performance.now() < end) requestAnimationFrame(sample);
    }
    requestAnimationFrame(sample);
  });
  await page.mouse.up({ button: "right" });
  await settled();
  await page.waitForTimeout(500);
  const returned = await state(),
    radii = await page.evaluate(() => window.qaCameraRadii);
  assert.ok(Math.max(...radii) - Math.min(...radii) < 1);
  assert.ok(
    Math.hypot(...returned.camera.map((v, i) => v - initial.camera[i])) < 15,
  );
  assert.equal(returned.yaw, initial.yaw);
  assert.equal(returned.pitch, initial.pitch);
  check(
    "Full horizontal orbit and above/below views leave the ship still; release returns around the hull",
    {
      dimensions,
      minimumCameraRadius: Math.min(...radii),
      maximumCameraRadius: Math.max(...radii),
    },
  );

  await page.mouse.move(800, 400);
  await page.mouse.down({ button: "left" });
  await page.mouse.move(900, 420, { steps: 8 });
  const left = await state();
  assert.equal(left.yaw, initial.yaw);
  assert.equal(left.pitch, initial.pitch);
  assert.equal(left.cameraOrbit.held, true);
  await page.mouse.down({ button: "right" });
  await page.mouse.move(600, 420, { steps: 10 });
  const both = await state();
  assert.equal(both.yaw, left.yaw);
  assert.equal(both.pitch, left.pitch);
  assert.equal(both.cameraOrbit.held, true);
  await page.mouse.up({ button: "right" });
  assert.equal((await state()).cameraOrbit.held, true);
  await page.mouse.move(700, 420, { steps: 6 });
  assert.equal((await state()).yaw, left.yaw);
  await page.mouse.up({ button: "left" });
  await settled();
  await startOrbit();
  const rightFirst = await state();
  await page.mouse.down({ button: "left" });
  await page.mouse.move(700, 400, { steps: 5 });
  await page.mouse.up({ button: "left" });
  await page.mouse.move(600, 400, { steps: 5 });
  assert.equal((await state()).cameraOrbit.held, true);
  assert.equal((await state()).yaw, rightFirst.yaw);
  assert.equal((await state()).pitch, rightFirst.pitch);
  await page.mouse.up({ button: "right" });
  await settled();
  check(
    "Both battleship drags control the camera; mixed-button releases never steer the hull",
  );

  await startOrbit();
  await page.keyboard.press("Escape");
  assert.equal((await state()).state.paused, true);
  assert.deepEqual((await state()).cameraOrbit, {
    yaw: 0,
    pitch: 0,
    focus: 0,
    held: false,
  });
  await page.mouse.up({ button: "right" });
  await page.getByRole("button", { name: "Resume flight" }).click();
  await startOrbit();
  await page.evaluate(() => window.dispatchEvent(new Event("blur")));
  assert.equal((await state()).cameraOrbit.held, false);
  assert.equal((await state()).state.paused, true);
  await page.mouse.up({ button: "right" });
  await page.getByRole("button", { name: "Resume flight" }).click();
  await startOrbit();
  await page.evaluate(() => {
    const canvas = document.querySelector("canvas");
    canvas.dispatchEvent(new PointerEvent("pointercancel", { pointerId: 1 }));
  });
  await page.mouse.up({ button: "right" });
  await settled();
  check("Pause, focus loss and cancelled pointer input clear camera control");

  await page.getByRole("button", { name: "Inspect with shuttle" }).click();
  await page.waitForTimeout(2000);
  const inspection = await state();
  await startOrbit();
  const looking = await state();
  assert.equal(looking.yaw, inspection.yaw);
  assert.equal(looking.pitch, inspection.pitch);
  assert.deepEqual(looking.state.position, inspection.state.position);
  assert.deepEqual(looking.carrier, inspection.carrier);
  assert.notDeepEqual(looking.camera, inspection.camera);
  await page.mouse.up({ button: "right" });
  await settled();
  await page.getByRole("button", { name: "Resume command" }).click();
  check(
    "Inspection shuttle can look around without moving the shuttle or parked battleship",
  );

  await page.getByRole("button", { name: "Approach orbit" }).click();
  await startOrbit();
  assert.ok((await state()).state.course);
  await page.mouse.move(450, 350, { steps: 10 });
  assert.ok((await state()).state.course);
  await page.mouse.up({ button: "right" });
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.canDescend,
    {},
    { timeout: 20000 },
  );
  await page.getByRole("button", { name: "Launch survey shuttle" }).click();
  await page.waitForFunction(
    () => !window.aquilaDiagnostics().state.descending,
  );
  await page.waitForTimeout(500);
  const surface = await state();
  await startOrbit();
  const surfaceLook = await state();
  assert.equal(surfaceLook.state.altitude, surface.state.altitude);
  assert.deepEqual(surfaceLook.surfaceHeading, surface.surfaceHeading);
  assert.notDeepEqual(surfaceLook.camera, surface.camera);
  await capture("surface");
  await page.mouse.up({ button: "right" });
  await settled();
  check(
    "Camera orbit preserves arrival assist and surface heading and altitude",
  );

  await page.getByRole("link", { name: "Settings", exact: true }).click();
  await page.getByLabel("Reduced motion").check();
  await page.getByRole("link", { name: /Enter universe/ }).click();
  await startOrbit();
  await page.mouse.up({ button: "right" });
  await page.waitForTimeout(50);
  assert.deepEqual((await state()).cameraOrbit, {
    yaw: 0,
    pitch: 0,
    focus: 0,
    held: false,
  });
  check("Reduced motion returns directly to chase view");
  assert.deepEqual(errors, []);
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    prefix,
    checks,
    errors,
  };
  await fs.writeFile(
    ".runtime/orbit-camera-qa.json",
    JSON.stringify(receipt, null, 2),
  );
  console.log(JSON.stringify({ passed: checks.length, errors }));
} finally {
  await browser.close();
}
