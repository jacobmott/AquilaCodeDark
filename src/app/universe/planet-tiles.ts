export interface Tile {
  face: number;
  level: number;
  x: number;
  y: number;
}
export const tileKey = (t: Tile): string =>
  `${t.face}/${t.level}/${t.x}/${t.y}`;
export const roots = (): Tile[] =>
  Array.from({ length: 6 }, (_, face) => ({ face, level: 0, x: 0, y: 0 }));
export function children(t: Tile): Tile[] {
  return [0, 1, 2, 3].map((i) => ({
    face: t.face,
    level: t.level + 1,
    x: t.x * 2 + (i % 2),
    y: t.y * 2 + Math.floor(i / 2),
  }));
}
// Face bases all have outward winding, and adjacent faces sample the same 3D field.
export function cubeDirection(
  face: number,
  u: number,
  v: number,
): [number, number, number] {
  const p = [
    [1, v, -u],
    [-1, v, u],
    [u, 1, -v],
    [u, -1, v],
    [u, v, 1],
    [-u, v, -1],
  ][face];
  const d = Math.hypot(...p);
  return [p[0] / d, p[1] / d, p[2] / d];
}
export function tileDirection(
  t: Tile,
  u: number,
  v: number,
): [number, number, number] {
  const count = 2 ** t.level;
  return cubeDirection(
    t.face,
    ((t.x + u) / count) * 2 - 1,
    ((t.y + v) / count) * 2 - 1,
  );
}
// A bounded greedy quadtree; ancestors remain resident as hole-free loading fallbacks.
export function planTiles(
  radius: number,
  observer: readonly number[],
  maxLeaves: number,
  maxLevel = 10,
): Tile[] {
  const leaves = roots(),
    all = [...leaves];
  const altitude = Math.max(80, Math.hypot(...observer) - radius);
  while (leaves.length + 3 <= maxLeaves) {
    let best = -1,
      score = 2.1;
    for (let i = 0; i < leaves.length; i++) {
      const t = leaves[i];
      if (t.level >= maxLevel) continue;
      const p = tileDirection(t, 0.5, 0.5),
        width = (radius * 2) / 2 ** t.level;
      const d = Math.hypot(...p.map((n, j) => n * radius - observer[j]));
      const priority = width / Math.max(altitude, d - width * 0.65);
      if (priority > score) {
        score = priority;
        best = i;
      }
    }
    if (best < 0) break;
    const next = children(leaves.splice(best, 1)[0]);
    leaves.push(...next);
    all.push(...next);
  }
  return all;
}
