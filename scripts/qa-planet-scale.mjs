// Local Chrome measurements. CPU throttling and phone viewports do NOT emulate a mobile GPU or thermal limits.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";

const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const prefix = process.env.AQUILA_QA_CAPTURE_PREFIX || "latest-";
assert.match(prefix, /^[a-zA-Z0-9-]+$/);
const browser = await chromium.launch({ channel: "chrome", headless: true });
const errors = [],
  checks = [],
  scenarios = [];
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log("PASS", name);
};
const state = (page) => page.evaluate(() => window.aquilaDiagnostics());
const capture = (page, name) =>
  page.screenshot({
    path: `reports/game-direction/media/${prefix}${name}.png`,
  });
const sample = async (page, name, duration) => {
  const result = await page.evaluate(
    (ms) =>
      new Promise((resolve) => {
        const intervals = [],
          resources = [],
          started = performance.now();
        let last = 0,
          sampleAt = 0;
        function frame(now) {
          if (last) intervals.push(now - last);
          last = now;
          if (now - sampleAt > 1000) {
            const s = window.aquilaDiagnostics();
            resources.push({
              at: now - started,
              sectors: s.loadedSectors,
              geometry: s.geometries,
              textures: s.textures,
              terrain: s.terrain,
              triangles: s.triangles,
              calls: s.calls,
            });
            sampleAt = now;
          }
          if (now - started < ms) return requestAnimationFrame(frame);
          intervals.sort((a, b) => a - b);
          resolve({
            durationMs: now - started,
            frames: intervals.length,
            medianMs: intervals[Math.floor(intervals.length * 0.5)],
            p95Ms: intervals[Math.floor(intervals.length * 0.95)],
            p99Ms: intervals[Math.floor(intervals.length * 0.99)],
            maxMs: intervals.at(-1),
            over50ms: intervals.filter((x) => x > 50).length,
            resources,
          });
        }
        requestAnimationFrame(frame);
      }),
    duration,
  );
  for (const r of result.resources) {
    assert.ok(r.sectors <= 27);
    if (r.terrain) {
      assert.ok(r.terrain.resident <= r.terrain.limit);
      assert.ok(r.terrain.geometryBytes < 2_000_000);
    }
  }
  console.log(
    "MEASURE",
    name,
    JSON.stringify({
      p95: result.p95Ms,
      max: result.maxMs,
      over50ms: result.over50ms,
    }),
  );
  return { name, ...result };
};
const approach = async (page) => {
  await page.getByRole("button", { name: "Approach orbit" }).click();
  await page.waitForFunction(
    () => window.aquilaDiagnostics().state.canDescend,
    {},
    { timeout: 30000 },
  );
};
const choose = async (page, name) => {
  await page.getByRole("link", { name: "Settings", exact: true }).click();
  await page
    .getByRole("radiogroup", { name: "Visual quality" })
    .getByText(name, { exact: true })
    .click();
};
try {
  for (const config of [
    {
      name: "desktop-balanced",
      viewport: { width: 1440, height: 900 },
      deviceScaleFactor: 1,
      cpuRate: 1,
      quality: "Balanced",
      targetP95Ms: 16.7,
    },
    {
      name: "phone-viewport-cpu4-light",
      viewport: { width: 390, height: 844 },
      deviceScaleFactor: 2,
      cpuRate: 4,
      quality: "Light",
      targetP95Ms: 33.4,
    },
  ]) {
    const context = await browser.newContext({
      viewport: config.viewport,
      deviceScaleFactor: config.deviceScaleFactor,
      hasTouch: config.cpuRate > 1,
    });
    const page = await context.newPage(),
      cdp = await context.newCDPSession(page);
    page.on("pageerror", (e) => errors.push(String(e)));
    page.on("console", (m) => {
      if (m.type() === "error") errors.push(m.text());
    });
    await cdp.send("Emulation.setCPUThrottlingRate", { rate: config.cpuRate });
    const startup = Date.now();
    await page.goto(`${base}/home`);
    await page.waitForFunction(
      () => window.aquilaDiagnostics()?.renderedFrames > 3,
      {},
      { timeout: 30000 },
    );
    const startupMs = Date.now() - startup;
    await choose(page, config.quality);
    await page.getByRole("button", { name: "Return to Vesper" }).click();
    const firstMeasurement = sample(page, "first-approach", 8000);
    await approach(page);
    const firstApproach = await firstMeasurement;
    const orbit = await state(page);
    assert.equal(orbit.state.nearest.radius, 60000);
    assert.ok(orbit.state.distance >= 3900 + orbit.hullRadius - 1);
    await page.keyboard.down("Shift");
    await page.keyboard.down("w");
    await page.waitForTimeout(2000);
    await page.keyboard.up("w");
    await page.keyboard.up("Shift");
    const boundary = await state(page);
    assert.ok(boundary.state.distance >= 3900 + boundary.hullRadius - 1);
    assert.deepEqual(boundary.state.position, orbit.state.position);
    check(
      `${config.name}: automatic arrival preserves capital clearance and ignores manual boost`,
      {
        distance: boundary.state.distance,
      },
    );
    const carrier = boundary.state.position;
    const descentMeasurement = sample(page, "descent", 6500);
    await page.getByRole("button", { name: "Launch survey shuttle" }).click();
    await page.waitForFunction(
      () => !window.aquilaDiagnostics().state.descending,
      {},
      { timeout: 20000 },
    );
    const descent = await descentMeasurement;
    await capture(page, `${config.name}-surface`);
    const before = await state(page);
    await page.keyboard.down("Shift");
    await page.keyboard.down("w");
    const surface = await sample(page, "surface-travel", 30000);
    await page.keyboard.up("w");
    await page.keyboard.up("Shift");
    const after = await state(page);
    assert.ok(after.terrain.generated > before.terrain.generated);
    assert.ok(after.terrain.evicted > before.terrain.evicted);
    assert.ok(after.state.altitude >= 80);
    check(
      `${config.name}: moving terrain remains bounded and releases old patches`,
      {
        generated: after.terrain.generated,
        evicted: after.terrain.evicted,
        resident: after.terrain.resident,
        limit: after.terrain.limit,
      },
    );
    const budgets = [];
    for (const quality of ["High", "Light", "Balanced", config.quality]) {
      await choose(page, quality);
      await page.waitForTimeout(1800);
      const d = await state(page),
        dimensions = await page
          .locator("canvas")
          .evaluate((c) => ({ width: c.width, height: c.height }));
      assert.ok(
        dimensions.width * dimensions.height <= d.qualityBudget.maxPixels,
      );
      assert.ok(d.terrain.resident <= d.terrain.limit);
      budgets.push({ quality, dimensions, terrain: d.terrain });
    }
    check(`${config.name}: quality limits both pixels and terrain`, budgets);
    await page.getByRole("link", { name: "Enter universe" }).click();
    await page.getByRole("button", { name: "Return to battleship" }).click();
    assert.deepEqual((await state(page)).state.position, carrier);
    await page.getByRole("link", { name: "Atlas", exact: true }).click();
    await page.locator(".destination").nth(1).click();
    await page.waitForFunction(
      () => window.aquilaDiagnostics().state.canDescend,
      {},
      { timeout: 30000 },
    );
    const second = await state(page);
    assert.notEqual(second.state.nearest.id, "0:0:0");
    await page.waitForFunction(
      () => window.aquilaDiagnostics().queuedWorlds === 0,
    );
    await choose(page, config.quality);
    await page.getByRole("button", { name: "Return to Vesper" }).click();
    const warmed = sample(page, "warm-approach", 8000);
    await approach(page);
    const warmApproach = await warmed;
    // Turn across the globe to inspect a shallow horizon instead of pointing at its center.
    if (config.cpuRate === 1) {
      await page.keyboard.down("ArrowUp");
      await page.waitForTimeout(1150);
      await page.keyboard.up("ArrowUp");
      await page.waitForTimeout(900);
      await capture(page, "planet-horizon");
    }
    const heap = await cdp.send("Runtime.getHeapUsage");
    const webgl = await page.evaluate(() => {
      const g = document.querySelector("canvas").getContext("webgl2"),
        e = g.getExtension("WEBGL_debug_renderer_info");
      return e ? g.getParameter(e.UNMASKED_RENDERER_WEBGL) : "unavailable";
    });
    const stages = [firstApproach, descent, surface, warmApproach];
    scenarios.push({
      config,
      startupMs,
      webgl,
      heap,
      stages,
      meetsWarmP95Target:
        surface.p95Ms <= config.targetP95Ms &&
        warmApproach.p95Ms <= config.targetP95Ms,
      final: await state(page),
    });
    await context.close();
  }
  // Exercise the precision fallback independently of EXT_clip_control availability.
  const context = await browser.newContext({
    viewport: { width: 1000, height: 700 },
  });
  await context.addInitScript(() => {
    const original = WebGL2RenderingContext.prototype.getExtension;
    WebGL2RenderingContext.prototype.getExtension = function (name) {
      return name === "EXT_clip_control" ? null : original.call(this, name);
    };
  });
  const page = await context.newPage();
  page.on("pageerror", (e) => errors.push(String(e)));
  page.on("console", (m) => {
    if (m.type() === "error") errors.push(m.text());
  });
  await page.goto(`${base}/play`);
  await page.waitForFunction(
    () => window.aquilaDiagnostics()?.renderedFrames > 3,
  );
  await approach(page);
  assert.equal((await state(page)).depth, "logarithmic");
  await page.getByRole("button", { name: "Launch survey shuttle" }).click();
  await page.waitForFunction(
    () => !window.aquilaDiagnostics().state.descending,
  );
  await capture(page, "planet-log-depth");
  check(
    "Logarithmic depth fallback renders approach and descent without shader errors",
  );
  await context.close();
  assert.deepEqual(errors, []);
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    capturePrefix: prefix,
    limits:
      "Development preview on this desktop GPU. CPU throttling is not a phone or laptop hardware benchmark. Buffer bytes exclude driver overhead.",
    checks,
    scenarios,
    errors,
  };
  await fs.writeFile(
    ".runtime/planet-scale-qa.json",
    JSON.stringify(receipt, null, 2),
  );
  console.log(
    JSON.stringify({
      passed: checks.length,
      scenarios: scenarios.map((s) => ({
        name: s.config.name,
        meetsWarmP95Target: s.meetsWarmP95Target,
      })),
      errors,
    }),
  );
} finally {
  await browser.close();
}
