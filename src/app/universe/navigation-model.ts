import type { Vec3 } from './universe-model';

export type HelmOrder = 'hold' | 'approach' | 'range' | 'withdraw';
export const COMBAT_STANDOFF = 35000;
export const WITHDRAWAL_RANGE = 85000;
export const RANGE_TOLERANCE = 1000;

// Orders choose positions. They never aim weapons or change their firing mode.
export function combatDestination(
  position: Vec3,
  target: Vec3,
  withdraw: boolean,
): Vec3 | null {
  const offset = position.map((n, i) => n - target[i]);
  const distance = Math.hypot(...offset);
  const desired = withdraw ? WITHDRAWAL_RANGE : COMBAT_STANDOFF;
  if (
    withdraw
      ? distance >= desired - RANGE_TOLERANCE
      : Math.abs(distance - desired) <= RANGE_TOLERANCE
  )
    return null;
  const direction = distance < 1 ? [0, 0, 1] : offset.map((n) => n / distance);
  return target.map((n, i) => n + direction[i] * desired) as Vec3;
}

// Bounded shortest-angle changes avoid an instant turn when an order changes.
export function turnToward(
  yaw: number,
  pitch: number,
  direction: Vec3,
  dt: number,
): [number, number] {
  const length = Math.hypot(...direction);
  if (
    length < 0.001 ||
    !Number.isFinite(length) ||
    !Number.isFinite(dt) ||
    dt <= 0
  )
    return [yaw, pitch];
  const desiredYaw = Math.atan2(-direction[0], -direction[2]);
  const desiredPitch = Math.asin(
    Math.max(-1, Math.min(1, direction[1] / length)),
  );
  const change = Math.atan2(
    Math.sin(desiredYaw - yaw),
    Math.cos(desiredYaw - yaw),
  );
  const limit = Math.min(dt, 0.05) * 0.45;
  const nextYaw = yaw + Math.max(-limit, Math.min(limit, change));
  return [
    Math.atan2(Math.sin(nextYaw), Math.cos(nextYaw)),
    pitch + Math.max(-limit, Math.min(limit, desiredPitch - pitch)),
  ];
}
