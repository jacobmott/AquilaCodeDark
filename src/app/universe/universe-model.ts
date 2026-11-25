export type Vec3 = [number, number, number];
export type View = 'home' | 'ships' | 'atlas' | 'play' | 'settings';
export type Quality = 'low' | 'balanced' | 'high';
export const QUALITY_BUDGETS = {
  low: {
    pixelRatio: 1,
    maxPixels: 1_200_000,
    terrainLeaves: 54,
    detail: 0,
    clouds: false,
    stars: 2200,
  },
  balanced: {
    pixelRatio: 1.4,
    maxPixels: 2_200_000,
    terrainLeaves: 90,
    detail: 0.6,
    clouds: true,
    stars: 4000,
  },
  high: {
    pixelRatio: 2,
    maxPixels: 4_200_000,
    terrainLeaves: 138,
    detail: 1,
    clouds: true,
    stars: 6500,
  },
} as const;
// A rotation-independent envelope protects the entire hull, including a turn at the boundary.
export const hullRadius = (vessel: Vessel): number =>
  Math.hypot(vessel.width, vessel.height, vessel.length) / 2 +
  vessel.length * 0.04;
export const orbitAltitude = (planet: Planet, clearance = 0): number =>
  Math.max(3500, planet.radius * 0.065) + clearance;
export const arrivalAltitude = (planet: Planet, clearance = 0): number =>
  orbitAltitude(planet, clearance) + 1300;
export const descentRange = (planet: Planet, clearance = 0): number =>
  orbitAltitude(planet, clearance) + 3000;

export interface Vessel {
  id: string;
  name: string;
  role: string;
  description: string;
  length: number;
  width: number;
  height: number;
  capital?: boolean;
  speed: number;
  handling: number;
  color: number;
  accent: number;
}
export const FLEET: readonly Vessel[] = [
  {
    id: 'peregrine',
    name: 'Peregrine',
    role: 'Survey shuttle',
    description:
      'Your eyes beyond the hull. A nimble survey shuttle for scouting, close approaches and planetary expeditions.',
    length: 68,
    width: 64,
    height: 16,
    speed: 260,
    handling: 0.95,
    color: 0xc6c5bc,
    accent: 0xef936a,
  },
  {
    id: 'osprey',
    name: 'Osprey',
    role: 'Fast scout shuttle',
    description:
      'Fast, precise, and a little restless. Twin drives carry a small crew far beyond familiar stars.',
    length: 46,
    width: 52,
    height: 14,
    speed: 340,
    handling: 1.25,
    color: 0xa7b8c2,
    accent: 0x86d5df,
  },
  {
    id: 'aegis',
    name: 'Aegis',
    role: 'Heavy utility shuttle',
    description:
      'A broad-hulled utility shuttle. Steady engines and a cargo hold for the work between ship and shore.',
    length: 112,
    width: 84,
    height: 25,
    speed: 190,
    handling: 0.65,
    color: 0xa6aaa7,
    accent: 0xdfb573,
  },
  {
    id: 'resolute',
    name: 'Delta Tree',
    role: 'Research battleship',
    description:
      'An armored home for the long voyage. Twin hangar galleries, a raised command island and six main drives.',
    length: 13600,
    width: 4800,
    height: 1280,
    capital: true,
    speed: 1100,
    handling: 0.18,
    color: 0x929b9f,
    accent: 0xd9a56c,
  },
];
export const FLAGSHIP = FLEET[3];
// Open lateral bays, in hull-local metres. Forward is -Z. These also reserve later docking/window anchors.
export const HANGARS = [-1, 1].map((side) => ({
  side,
  mouth: [side * 2400, (-0.9 / 6.4) * FLAGSHIP.height, -1000] as Vec3,
  approach: [side * 3400, (-0.9 / 6.4) * FLAGSHIP.height, -1000] as Vec3,
  width: 4800,
  height: (2.4 / 6.4) * FLAGSHIP.height,
}));

export interface Planet {
  id: string;
  name: string;
  kind: string;
  position: Vec3;
  radius: number;
  seed: number;
  hue: number;
  rings: boolean;
}
export const SECTOR_SIZE = 800000;
export const HOME_WORLD: Planet = {
  id: '0:0:0',
  name: 'Vesper',
  kind: 'Ocean world',
  position: [70000, 18000, -180000],
  radius: 60000,
  seed: 18,
  hue: 0,
  rings: true,
};
export function seedRandom(seed: number): () => number {
  let state = seed >>> 0;
  return () => {
    state += 0x6d2b79f5;
    let t = Math.imul(state ^ (state >>> 15), 1 | state);
    t ^= t + Math.imul(t ^ (t >>> 7), 61 | t);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}
export function sectorAt(position: readonly number[]): Vec3 {
  return position.map((n) =>
    Math.floor((n + SECTOR_SIZE / 2) / SECTOR_SIZE),
  ) as Vec3;
}
export function sectorKey(sector: readonly number[]): string {
  return sector.join(':');
}
export function sectorPlanet(sector: Vec3): Planet {
  const id = sectorKey(sector);
  if (id === HOME_WORLD.id)
    return { ...HOME_WORLD, position: [...HOME_WORLD.position] };
  let hash = 2166136261;
  for (const char of id) hash = Math.imul(hash ^ char.charCodeAt(0), 16777619);
  const rng = seedRandom(hash);
  const names = [
    'Aster',
    'Nacre',
    'Solace',
    'Iona',
    'Lyra',
    'Meridian',
    'Eos',
    'Thalassa',
  ];
  const hue = rng();
  return {
    id,
    name: `${names[Math.floor(rng() * names.length)]} ${
      ((hash >>> 0) % 899) + 100
    }`,
    kind: hue < 0.4 ? 'Ocean world' : hue < 0.7 ? 'Mineral world' : 'Ice world',
    position: sector.map(
      (n) => n * SECTOR_SIZE + (rng() - 0.5) * 400000,
    ) as Vec3,
    radius: 30000 + rng() * 40000,
    seed: rng() * 500,
    hue,
    rings: rng() > 0.62,
  };
}
export function nearbySectors(position: readonly number[]): Vec3[] {
  const [x, y, z] = sectorAt(position);
  const sectors: Vec3[] = [];
  for (let a = -1; a <= 1; a++)
    for (let b = -1; b <= 1; b++)
      for (let c = -1; c <= 1; c++) {
        sectors.push([x + a, y + b, z + c]);
      }
  return sectors;
}
export interface FlightSave {
  version: 2;
  position: Vec3;
  rotation: [number, number];
  vessel: string;
}
// Reject corrupt, stale, or extreme saves before they can reach the renderer.
export function readFlightSave(raw: string | null): FlightSave | null {
  try {
    const value = JSON.parse(raw ?? 'null');
    const finite = (a: unknown, length: number, limit: number): boolean =>
      Array.isArray(a) &&
      a.length === length &&
      a.every(
        (n) =>
          typeof n === 'number' && Number.isFinite(n) && Math.abs(n) <= limit,
      );
    if (
      ![1, 2].includes(value?.version) ||
      !finite(
        value.position,
        3,
        value.version === 1 ? (1e12 * 12000) / SECTOR_SIZE : 1e12,
      ) ||
      !finite(value.rotation, 2, Math.PI * 2) ||
      !FLEET.some((ship) => ship.id === value.vessel)
    )
      return null;
    return {
      version: 2,
      // Preserve the old sector and selected ship. Startup enforces the new orbital boundary.
      position: value.position.map(
        (n: number) => n * (value.version === 1 ? SECTOR_SIZE / 12000 : 1),
      ) as Vec3,
      rotation: [
        value.rotation[0],
        Math.max(-1.48, Math.min(1.48, value.rotation[1])),
      ],
      vessel: value.vessel,
    };
  } catch {
    return null;
  }
}
export interface Preferences {
  quality: Quality;
  reducedMotion: boolean;
  sensitivity: number;
}
export function readPreferences(
  raw: string | null,
  reducedMotion = false,
): Preferences {
  const fallback: Preferences = {
    quality: 'balanced',
    reducedMotion,
    sensitivity: 1,
  };
  try {
    const value = JSON.parse(raw ?? 'null');
    return {
      quality: ['low', 'balanced', 'high'].includes(value?.quality)
        ? value.quality
        : fallback.quality,
      reducedMotion:
        typeof value?.reducedMotion === 'boolean'
          ? value.reducedMotion
          : reducedMotion,
      sensitivity:
        typeof value?.sensitivity === 'number' &&
        Number.isFinite(value.sensitivity)
          ? Math.max(0.4, Math.min(1.8, value.sensitivity))
          : 1,
    };
  } catch {
    return fallback;
  }
}
export function distance(a: readonly number[], b: readonly number[]): number {
  return Math.hypot(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}
export function nearestPlanet(
  position: readonly number[],
  planets: Planet[],
): Planet {
  return [...planets].sort(
    (a, b) =>
      distance(a.position, position) -
      a.radius -
      distance(b.position, position) +
      b.radius,
  )[0];
}

// Earliest segment/sphere contact prevents fast cruise from tunneling through a world.
export function sphereContact(
  start: readonly number[],
  end: readonly number[],
  center: readonly number[],
  radius: number,
): number | null {
  const d = end.map((n, i) => n - start[i]),
    o = start.map((n, i) => n - center[i]);
  const a = d.reduce((sum, n) => sum + n * n, 0),
    c = o.reduce((sum, n) => sum + n * n, 0) - radius * radius;
  if (c < -1) return 0;
  if (a < 1e-10) return null;
  const b = 2 * d.reduce((sum, n, i) => sum + n * o[i], 0),
    disc = b * b - 4 * a * c;
  if (disc < 0) return null;
  const t = (-b - Math.sqrt(disc)) / (2 * a);
  return t >= 0 && t <= 1 ? t : null;
}

// Arrival assist takes short exterior arcs around planets blocking its course.
// Each arc chord stays outside the collision sphere, including opposite-side departures.
export function flightPath(
  start: Vec3,
  end: Vec3,
  planets: Planet[],
  clearance = 0,
): Vec3[] {
  const path = [start, end];
  const unit = (v: number[]): Vec3 => {
    const size = Math.hypot(...v);
    return v.map((n) => n / size) as Vec3;
  };
  for (let attempt = 0; attempt < 8; attempt++) {
    let changed = false;
    for (let i = 0; i < path.length - 1 && !changed; i++) {
      for (const planet of planets) {
        const radius = planet.radius + orbitAltitude(planet, clearance);
        if (
          sphereContact(path[i], path[i + 1], planet.position, radius) === null
        )
          continue;
        const a = unit(path[i].map((n, j) => n - planet.position[j]));
        const b = unit(path[i + 1].map((n, j) => n - planet.position[j]));
        const dot = Math.max(
          -1,
          Math.min(
            1,
            a.reduce((sum, n, j) => sum + n * b[j], 0),
          ),
        );
        let tangent = b.map((n, j) => n - a[j] * dot);
        if (Math.hypot(...tangent) < 1e-6) {
          const axis = Math.abs(a[1]) < 0.9 ? [0, 1, 0] : [1, 0, 0];
          const projection = axis.reduce((sum, n, j) => sum + n * a[j], 0);
          tangent = axis.map((n, j) => n - a[j] * projection);
        }
        tangent = unit(tangent);
        const angle = Math.acos(dot),
          count = Math.max(1, Math.ceil(angle / (Math.PI / 8)));
        const arcRadius = radius * 1.1 + 1000;
        const arc = Array.from({ length: count + 1 }, (_, step) => {
          const turn = (angle * step) / count;
          return a.map(
            (n, j) =>
              planet.position[j] +
              arcRadius * (n * Math.cos(turn) + tangent[j] * Math.sin(turn)),
          ) as Vec3;
        });
        path.splice(i + 1, 0, ...arc);
        changed = true;
        break;
      }
    }
    if (!changed) break;
  }
  return path;
}
