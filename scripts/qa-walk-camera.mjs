// Native capture/drag checks plus explicit injected input faults; no simulation setters.
import { chromium } from "../.runtime/browser-qa/node_modules/playwright-core/index.mjs";
import assert from "node:assert/strict";
import fs from "node:fs/promises";

const base = process.env.AQUILA_QA_URL || "http://127.0.0.1:4200";
const browser = await chromium.launch({ channel: "chrome", headless: true });
const page = await browser.newPage({ viewport: { width: 1440, height: 900 } });
const errors = [],
  checks = [];
page.on("pageerror", (error) => errors.push(String(error)));
page.on("console", (message) => {
  if (message.type() === "error") errors.push(message.text());
});
const state = () => page.evaluate(() => window.aquilaDiagnostics());
const check = (name, detail) => {
  checks.push({ name, detail });
  console.log("PASS", name);
};
const close = (a, b) =>
  assert.ok(Math.abs(a - b) < 0.000001, `${a} differs from ${b}`);
const inject = async (dx, dy) =>
  page.evaluate(
    ({ dx, dy }) => {
      document.dispatchEvent(
        new MouseEvent("mousemove", {
          movementX: dx,
          movementY: dy,
          bubbles: true,
        }),
      );
    },
    { dx, dy },
  );

try {
  await page.addInitScript(() => {
    const original = Element.prototype.requestPointerLock;
    window.qaLockRequests = [];
    Element.prototype.requestPointerLock = function (options) {
      window.qaLockRequests.push(options ?? null);
      return original.call(this, options);
    };
  });
  await page.goto(`${base}/play`);
  await page.getByRole("button", { name: "Enter bridge" }).click();
  await page.mouse.click(740, 430);
  await page.waitForFunction(() => Boolean(document.pointerLockElement));
  assert.equal(
    await page.evaluate(() => window.qaLockRequests[0].unadjustedMovement),
    true,
  );
  const start = await state();
  await page.mouse.move(860, 450, { steps: 24 });
  const normal = await state();
  close(normal.deck.yaw - start.deck.yaw, -0.3);
  close(normal.deck.pitch - start.deck.pitch, -0.05);
  assert.equal(normal.yaw, start.yaw);
  check(
    "Native capture requests raw input and normal mouse movement keeps its expected sensitivity",
  );

  const samples = [];
  for (const dy of [700, -700, 400, -400, 12000, -12000]) {
    const before = await state();
    await inject(4, dy);
    const after = await state();
    close(after.deck.pitch, before.deck.pitch);
    close(after.deck.yaw, before.deck.yaw - 0.01);
    samples.push({
      injectedY: dy,
      pitchBefore: before.deck.pitch,
      pitchAfter: after.deck.pitch,
    });
  }
  const vertical = await state();
  await inject(12000, 8);
  const horizontal = await state();
  close(horizontal.deck.yaw, vertical.deck.yaw);
  close(horizontal.deck.pitch, vertical.deck.pitch - 0.02);
  await page.waitForTimeout(350);
  close((await state()).deck.pitch, horizontal.deck.pitch);
  check(
    "Injected spikes in either direction are discarded without losing the good axis or moving the view later",
    samples,
  );

  await page.keyboard.down("s");
  await page.waitForTimeout(600);
  await page.keyboard.up("s");
  const walked = await state();
  assert.notDeepEqual(walked.deck.position, horizontal.deck.position);
  close(walked.deck.pitch, horizontal.deck.pitch);
  close(walked.deck.yaw, horizontal.deck.yaw);
  await page.keyboard.press("Escape");
  await page.waitForFunction(
    () =>
      !document.pointerLockElement && window.aquilaDiagnostics().state.paused,
  );
  await page.getByRole("button", { name: "Resume flight" }).click();
  await page.mouse.click(900, 400);
  await page.waitForFunction(() => Boolean(document.pointerLockElement));
  const recaptured = await state();
  close(recaptured.deck.pitch, walked.deck.pitch);
  close(recaptured.deck.yaw, walked.deck.yaw);
  await page.mouse.move(920, 410, { steps: 8 });
  close((await state()).deck.pitch, recaptured.deck.pitch - 0.025);
  await page.keyboard.press("Escape");
  await page.getByRole("button", { name: "Resume flight" }).click();
  check(
    "Walking alone preserves pitch; Escape and recapture preserve look without a stale drag jump",
  );

  await page.mouse.move(850, 400);
  await page.mouse.down({ button: "right" });
  const beforeDrag = await state();
  await page.mouse.move(890, 420, { steps: 10 });
  const dragged = await state();
  close(dragged.deck.yaw, beforeDrag.deck.yaw - 0.1);
  close(dragged.deck.pitch, beforeDrag.deck.pitch - 0.05);
  // A discontinuous absolute pointer position in the drag path must be rejected and rebased.
  await page.evaluate(() => {
    const canvas = document.querySelector("canvas");
    canvas.dispatchEvent(
      new PointerEvent("pointermove", {
        pointerId: 1,
        buttons: 2,
        clientX: 894,
        clientY: 1120,
        bubbles: true,
      }),
    );
    canvas.dispatchEvent(
      new PointerEvent("pointermove", {
        pointerId: 1,
        buttons: 2,
        clientX: 898,
        clientY: 1124,
        bubbles: true,
      }),
    );
  });
  const dragFault = await state();
  close(dragFault.deck.pitch, dragged.deck.pitch - 0.01);
  await page.mouse.up({ button: "right" });
  check(
    "Native drag look remains smooth; a discontinuous drag sample rebases without a pitch snap",
  );

  await page.evaluate(() => {
    const raw = Element.prototype.requestPointerLock;
    window.qaFallbackCalls = [];
    Element.prototype.requestPointerLock = function (options) {
      window.qaFallbackCalls.push(options ?? null);
      if (options?.unadjustedMovement)
        return Promise.reject(
          new DOMException("Raw input unavailable", "NotSupportedError"),
        );
      return raw.call(this);
    };
  });
  await page.mouse.click(800, 410);
  await page.waitForFunction(() => Boolean(document.pointerLockElement));
  assert.deepEqual(await page.evaluate(() => window.qaFallbackCalls), [
    { unadjustedMovement: true },
    null,
  ]);
  await page.keyboard.press("Escape");
  await page.getByRole("button", { name: "Resume flight" }).click();
  await page.evaluate(() => {
    Element.prototype.requestPointerLock = () =>
      Promise.reject(new DOMException("Denied", "NotAllowedError"));
  });
  await page.mouse.move(800, 400);
  await page.mouse.down();
  await page.waitForTimeout(50);
  const denied = await state();
  await page.mouse.move(840, 410, { steps: 8 });
  close((await state()).deck.pitch, denied.deck.pitch - 0.025);
  await page.mouse.up();
  assert.equal(
    await page.evaluate(() => Boolean(document.pointerLockElement)),
    false,
  );
  check(
    "Unsupported raw input falls back to native capture; denied capture retains drag controls",
  );

  assert.deepEqual(errors, []);
  const receipt = {
    time: new Date().toISOString(),
    base,
    browser: await browser.version(),
    checks,
    errors,
    scope:
      "Native Chrome input plus explicitly injected movement faults and unsupported/denied API responses; user hardware spike not directly captured",
  };
  await fs.writeFile(
    ".runtime/walk-camera-qa.json",
    JSON.stringify(receipt, null, 2),
  );
  console.log(JSON.stringify({ passed: checks.length, errors }));
} finally {
  await browser.close();
}
