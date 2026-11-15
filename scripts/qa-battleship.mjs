// Isolated local Chrome checks for the exterior milestone; no accounts or provider calls.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";
const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-battleship-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const browser = await chromium.launch({ channel: "chrome", headless: true });
const context = await browser.newContext({
  viewport: { width: 1440, height: 900 },
});
const page = await context.newPage(),
  errors = [],
  checks = [],
  measurements = [];
page.on("pageerror", (e) => errors.push(String(e)));
page.on("console", (m) => {
  if (m.type() === "error") errors.push(m.text());
});
const state = () => page.evaluate(() => window.aquilaDiagnostics());
const capture = async (name) => {
  await page.waitForTimeout(700);
  await page.screenshot({
    path: `reports/game-direction/media/${prefix}${name}.png`,
  });
};
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log("PASS", name);
};
const hold = async (key, ms) => {
  await page.keyboard.down(key);
  await page.waitForTimeout(ms);
  await page.keyboard.up(key);
};
const sample = async (ms) =>
  page.evaluate(
    (ms) =>
      new Promise((resolve) => {
        const times = [];
        let first = 0,
          last = 0;
        function tick(t) {
          if (!first) first = t;
          if (last) times.push(t - last);
          last = t;
          if (t - first < ms) return requestAnimationFrame(tick);
          times.sort((a, b) => a - b);
          resolve({
            frames: times.length,
            p95Ms: times[Math.floor(times.length * 0.95)],
            maxMs: times.at(-1),
          });
        }
        requestAnimationFrame(tick);
      }),
    ms,
  );
try {
  await page.goto(`${base}/home`);
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 10,
  );
  assert.equal((await state()).ship, "resolute");
  await capture("home");
  await page.getByRole("button", { name: "Meet Resolute" }).click();
  await page.waitForTimeout(1800);
  assert.equal(await page.locator(".ship-option").count(), 4);
  const spec = await state(),
    b = spec.solidBounds;
  const dimensions = b.max.map((v, i) => v - b.min[i]);
  assert.ok(Math.abs(dimensions[0] - 4800) < 5);
  assert.ok(Math.abs(dimensions[1] - 1280) < 5);
  assert.ok(Math.abs(dimensions[2] - 13600) < 5);
  await capture("fleet");
  check(
    "Original exterior matches declared relative scale; four fleet choices remain available",
    { dimensions, spec: spec.vessel },
  );
  await page.getByRole("button", { name: "Command the Resolute" }).click();
  await page.waitForTimeout(1200);
  const start = (await state()).state.position;
  await hold("w", 1600);
  assert.deepEqual((await state()).state.position, start);
  await hold("ArrowRight", 600);
  assert.equal((await state()).yaw, 0);
  await hold(" ", 500);
  await capture("flight");
  measurements.push({ name: "capital-flight", ...(await sample(5000)) });
  check(
    "Battleship holds position despite manual flight input without replacing the renderer",
  );
  await page.getByRole("button", { name: "Inspect with shuttle" }).click();
  const inspecting = await state(),
    carrier = inspecting.carrier;
  assert.equal(inspecting.state.inspecting, true);
  assert.ok(Math.abs(inspecting.inspectionLocalPosition[0] - 3400) < 1);
  await capture("hangar-approach");
  await hold("w", 5500);
  let inside = await state();
  assert.ok(
    inside.inspectionLocalPosition[0] < 2350,
    JSON.stringify(inside.inspectionLocalPosition),
  );
  assert.ok(inside.inspectionLocalPosition[0] > 950);
  await capture("hangar-gallery");
  await page.keyboard.down("Shift");
  await hold("w", 3000);
  await page.keyboard.up("Shift");
  inside = await state();
  assert.ok(
    inside.inspectionLocalPosition[0] > 880,
    "Shuttle must not cross the inner hangar wall",
  );
  assert.ok(inside.inspectionLocalPosition[0] < 1150);
  assert.deepEqual(inside.carrier, carrier);
  check(
    "Shuttle enters the open gallery; swept hull checks stop boosted flight through its inner wall",
    { local: inside.inspectionLocalPosition },
  );
  // User navigation keeps the excursion and clears held flight input.
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  const suspended = (await state()).state.position;
  await hold("w", 300);
  assert.deepEqual((await state()).state.position, suspended);
  await page.getByRole("link", { name: "Enter universe" }).click();
  await page.getByRole("button", { name: "Resume command" }).click();
  const resumed = await state();
  assert.deepEqual(resumed.state.position, carrier.position);
  assert.equal(resumed.yaw, carrier.yaw);
  assert.equal(resumed.state.inspecting, false);
  await page.getByRole("button", { name: "Inspect with shuttle" }).click();
  await page.keyboard.press("Escape");
  const saved = await page.evaluate(() =>
    JSON.parse(localStorage.getItem("aquila.voyage.v1")),
  );
  assert.deepEqual(saved.position, carrier.position);
  await page.reload();
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 5,
  );
  assert.equal((await state()).state.inspecting, false);
  assert.deepEqual((await state()).state.position, carrier.position);
  check(
    "Inspection, pause, route changes, command return and reload preserve the battleship position",
  );
  await page.getByRole("button", { name: "Approach orbit" }).click();
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.canDescend,
    null,
    { timeout: 30000 },
  );
  await capture("planet");
  const orbit = await state();
  await page.keyboard.down("Shift");
  await hold("w", 2500);
  await page.keyboard.up("Shift");
  const boundary = await state();
  assert.ok(boundary.state.distance >= 3900 + boundary.hullRadius - 1);
  assert.deepEqual(boundary.state.position, orbit.state.position);
  check(
    "Automatic arrival leaves full capital clearance; manual boost cannot move the battleship",
    { distance: boundary.state.distance, envelope: boundary.hullRadius },
  );
  await hold("ArrowRight", 3000);
  assert.ok(
    (await state()).state.distance >= 3900 + (await state()).hullRadius - 1,
  );
  const planetPosition = (await state()).state.position;
  await page.getByRole("button", { name: "Launch survey shuttle" }).click();
  await page.waitForFunction(
    () =>
      window.aquilaDiagnostics().state.surface &&
      !window.aquilaDiagnostics().state.descending,
    null,
    { timeout: 20000 },
  );
  assert.ok(
    (await state()).state.altitude >= 349 &&
      (await state()).state.altitude <= 351,
  );
  await page.getByRole("button", { name: "Return to battleship" }).click();
  assert.deepEqual((await state()).state.position, planetPosition);
  check(
    "Planet approach, ignored manual turning, surface survey and return retain capital state",
  );
  // Exercise quality and ship replacement; allocations should return to a stable range.
  await page.getByRole("link", { name: "Fleet", exact: true }).click();
  const allocations = [];
  for (let i = 0; i < 3; i++) {
    await page.getByRole("button", { name: /01 Peregrine/ }).click();
    await page.getByRole("button", { name: /04 Resolute/ }).click();
    await page.waitForTimeout(300);
    allocations.push((await state()).geometries);
  }
  assert.ok(Math.max(...allocations) - Math.min(...allocations) <= 2);
  check(
    "Repeated shuttle and capital selection releases replaced hull geometry",
    { allocations },
  );
  await page.setViewportSize({ width: 390, height: 844 });
  await capture("phone-fleet");
  assert.ok(
    await page.evaluate(
      () => document.documentElement.scrollWidth <= window.innerWidth,
    ),
  );
  await page.getByRole("button", { name: "Command the Resolute" }).click();
  await page.getByRole("button", { name: "Inspect with shuttle" }).click();
  await capture("phone-inspection");
  assert.ok(
    await page.getByRole("button", { name: "Resume command" }).isVisible(),
  );
  await page.getByRole("button", { name: "Resume command" }).click();
  measurements.push({
    name: "phone-viewport-capital",
    ...(await sample(4000)),
  });
  check("Narrow viewport keeps fleet selection and inspection controls usable");
  assert.deepEqual(errors, []);
  await fs.writeFile(
    ".runtime/battleship-qa.json",
    JSON.stringify(
      {
        browser: browser.version(),
        checks,
        measurements,
        errors,
        final: await state(),
      },
      null,
      2,
    ),
  );
  console.log(JSON.stringify({ checks: checks.length, measurements, errors }));
} catch (error) {
  await fs.writeFile(
    ".runtime/battleship-qa-failure.json",
    JSON.stringify(
      {
        error: String(error),
        checks,
        errors,
        state: await state().catch(() => null),
      },
      null,
      2,
    ),
  );
  await page
    .screenshot({ path: ".runtime/battleship-failure.png" })
    .catch(() => {});
  throw error;
} finally {
  await browser.close();
}
