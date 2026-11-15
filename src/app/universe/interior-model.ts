// Human-scale deck coordinates; rooms are authored independently of the capital hull.
export type DeckPoint = [number, number]; // X, Z
export type DeckRoom = 'Bridge' | 'Passage' | 'Hangar';
export const EYE_HEIGHT = 1.75;
// Reject isolated cursor-warp spikes instead of clamping them into a visible snap.
// Continuous turns retain their full range; this bound applies to one input sample.
export function walkingLookDelta(
  dx: number,
  dy: number,
  sensitivity: number,
): [number, number] {
  const angle = (pixels: number): number => {
    const radians = pixels * 0.0025 * sensitivity;
    return Number.isFinite(radians) && Math.abs(radians) <= 0.35 ? radians : 0;
  };
  return [angle(dx), angle(dy)];
}
export const HELM: DeckPoint = [0, -2.4];
export const TACTICAL: DeckPoint = [6.5, -2.4];
export const BOARDING: DeckPoint = [0, 65];
type Rect = readonly [number, number, number, number];
const decks: readonly Rect[] = [
  [-12.6, 12.6, -13.6, 11.6],
  [-2.6, 2.6, 10.8, 38.6],
  [-47.6, 47.6, 37.8, 145.6],
];
const obstacles: readonly Rect[] = [
  [-4.4, 4.4, -7.4, -3.7], // helm console
  [5.1, 7.9, -6.2, -3.7], // tactical console
  [-12.6, -8.1, -8.4, 4.4],
  [8.1, 12.6, -8.4, 4.4], // side stations
  [-34.8, 34.8, 69, 135], // parked shuttle and service cradle
];
const contains = (r: Rect, x: number, z: number): boolean =>
  x >= r[0] && x <= r[1] && z >= r[2] && z <= r[3];
export function deckWalkable(x: number, z: number): boolean {
  return (
    decks.some((r) => contains(r, x, z)) &&
    !obstacles.some((r) => contains(r, x, z))
  );
}
export function moveOnDeck(position: DeckPoint, delta: DeckPoint): DeckPoint {
  let [x, z] = position;
  // Substeps protect door edges and furniture even after a long frame.
  const steps = Math.max(1, Math.ceil(Math.hypot(...delta) / 0.15));
  for (let i = 0; i < steps; i++) {
    const dx = delta[0] / steps,
      dz = delta[1] / steps;
    if (deckWalkable(x + dx, z)) x += dx;
    if (deckWalkable(x, z + dz)) z += dz;
  }
  return [x, z];
}
export const deckRoom = (z: number): DeckRoom =>
  z < 12 ? 'Bridge' : z < 38 ? 'Passage' : 'Hangar';
export function deckInteraction(
  position: DeckPoint,
): 'helm' | 'tactical' | 'shuttle' | null {
  if (Math.hypot(position[0] - HELM[0], position[1] - HELM[1]) < 3)
    return 'helm';
  if (Math.hypot(position[0] - TACTICAL[0], position[1] - TACTICAL[1]) < 2.5)
    return 'tactical';
  if (Math.hypot(position[0] - BOARDING[0], position[1] - BOARDING[1]) < 4)
    return 'shuttle';
  return null;
}
