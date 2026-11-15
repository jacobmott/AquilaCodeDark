import * as THREE from 'three';
import {
  terrainHeight,
  terrainFieldGLSL,
  planetNoiseGLSL,
} from './planet-fields';
import {
  children,
  planTiles,
  roots,
  Tile,
  tileDirection,
  tileKey,
} from './planet-tiles';
import { Planet, Quality, QUALITY_BUDGETS } from './universe-model';

export class PlanetTerrain {
  readonly group = new THREE.Group();
  private readonly tiles = new Map<string, THREE.Mesh>();
  private wanted = new Map<string, Tile>();
  private queue: Tile[] = [];
  private nextPlan = 0;
  private quality: Quality;
  private generated = 0;
  private evicted = 0;
  private peakResident = 0;
  private peakBuildMs = 0;
  private lastBuildMs = 0;
  private visibleTiles = 0;
  private readonly material: THREE.ShaderMaterial;

  constructor(
    readonly planet: Planet,
    quality: Quality,
  ) {
    this.quality = quality;
    this.material = new THREE.ShaderMaterial({
      uniforms: {
        seed: { value: planet.seed },
        detail: { value: QUALITY_BUDGETS[quality].detail },
        surface: { value: 0 },
        sea: {
          value: new THREE.Color(
            planet.hue < 0.4
              ? '#163e50'
              : planet.hue < 0.7
              ? '#483123'
              : '#32596b',
          ),
        },
        land: {
          value: new THREE.Color(
            planet.hue < 0.4
              ? '#6e8060'
              : planet.hue < 0.7
              ? '#b28d66'
              : '#adbec6',
          ),
        },
      },
      vertexShader: `attribute vec3 planetDirection; varying vec3 direction; varying vec3 norm; varying vec3 eye; varying float depth;
        #include <common>
        #include <logdepthbuf_pars_vertex>
        void main(){direction=planetDirection;norm=normalize(mat3(modelMatrix)*normal);
          vec4 p=modelMatrix*vec4(position,1.);eye=cameraPosition-p.xyz;
          vec4 view=viewMatrix*p;depth=length(view.xyz);gl_Position=projectionMatrix*view;
          #include <logdepthbuf_vertex>
        }`,
      fragmentShader: `uniform float seed;uniform float detail;uniform float surface;uniform vec3 sea;uniform vec3 land;
        varying vec3 direction;varying vec3 norm;varying vec3 eye;varying float depth;
        #include <logdepthbuf_pars_fragment>
        ${terrainFieldGLSL} ${planetNoiseGLSL}
        void main(){
          #include <logdepthbuf_fragment>
          vec3 p=normalize(direction),n=normalize(norm),light=normalize(vec3(-.7,.6,1.));
          float h=terrainHeight(p,seed),coast=smoothstep(.005,.035,continental(p,seed));
          float filtered=1.-smoothstep(.5,2.,max(length(dFdx(p)),length(dFdy(p)))*9300.);
          float grain=noise3(p*9300.)*filtered;
          float bands=noise3(p*120.+seed);
          vec3 rock=land*(.65+bands*.3*detail+grain*.12*detail);
          rock=mix(rock,vec3(.42,.43,.39),smoothstep(180.,850.,h)*.65);
          rock=mix(rock,vec3(.8,.84,.84),smoothstep(1100.,1500.,h));
          vec3 water=mix(sea,vec3(.08,.32,.33),smoothstep(-.04,.015,continental(p,seed))*.5);
          float shine=pow(max(0.,dot(reflect(-light,n),normalize(eye))),70.);
          water+=vec3(.55,.63,.65)*shine*.6*detail;
          vec3 color=mix(water,rock,coast)*(.10+max(0.,dot(n,light))*1.35);
          float haze=(1.-exp(-depth*.00007))*surface;
          color=mix(color,vec3(.37,.49,.55),haze);
          gl_FragColor=vec4(color,1.);
          #include <tonemapping_fragment>
          #include <colorspace_fragment>
        }`,
    });
  }
  setQuality(quality: Quality): void {
    this.quality = quality;
    this.material.uniforms['detail'].value = QUALITY_BUDGETS[quality].detail;
    this.nextPlan = 0;
  }
  get ready(): boolean {
    return roots().every((t) => this.tiles.has(tileKey(t)));
  }
  update(observer: THREE.Vector3, time: number, surface: boolean): void {
    this.material.uniforms['surface'].value = Number(surface);
    if (time >= this.nextPlan) {
      this.nextPlan = time + 0.25;
      const plan = planTiles(
        this.planet.radius,
        observer.toArray(),
        QUALITY_BUDGETS[this.quality].terrainLeaves,
      );
      this.wanted = new Map(plan.map((t) => [tileKey(t), t]));
      for (const [key, mesh] of this.tiles)
        if (!this.wanted.has(key)) {
          this.group.remove(mesh);
          mesh.geometry.dispose();
          this.tiles.delete(key);
          this.evicted++;
        }
      this.queue = plan.filter((t) => !this.tiles.has(tileKey(t)));
    }
    // One small fixed-resolution tile per animation frame; never build a planet in one blocking loop.
    const start = performance.now(),
      tile = this.queue.shift();
    if (tile) {
      const mesh = this.build(tile);
      this.tiles.set(tileKey(tile), mesh);
      this.group.add(mesh);
      this.generated++;
      this.peakResident = Math.max(this.peakResident, this.tiles.size);
    }
    this.lastBuildMs = performance.now() - start;
    this.peakBuildMs = Math.max(this.peakBuildMs, this.lastBuildMs);
    this.tiles.forEach((mesh) => (mesh.visible = false));
    this.visibleTiles = 0;
    const show = (t: Tile): void => {
      const parts = children(t);
      if (
        parts.every(
          (c) => this.wanted.has(tileKey(c)) && this.tiles.has(tileKey(c)),
        )
      )
        parts.forEach(show);
      else {
        const mesh = this.tiles.get(tileKey(t));
        if (mesh) {
          mesh.visible = true;
          this.visibleTiles++;
        }
      }
    };
    roots().forEach(show);
  }
  private build(t: Tile): THREE.Mesh {
    const segments = 12,
      stride = segments + 1;
    const center = new THREE.Vector3(
      ...tileDirection(t, 0.5, 0.5),
    ).multiplyScalar(this.planet.radius);
    const positions: number[] = [],
      normals: number[] = [],
      directions: number[] = [],
      indices: number[] = [];
    const n = new THREE.Vector3(),
      tangent = new THREE.Vector3(),
      bitangent = new THREE.Vector3();
    const displaced = (p: THREE.Vector3): THREE.Vector3 =>
      p
        .clone()
        .multiplyScalar(
          this.planet.radius + terrainHeight(p.x, p.y, p.z, this.planet.seed),
        );
    for (let y = 0; y <= segments; y++)
      for (let x = 0; x <= segments; x++) {
        n.fromArray(tileDirection(t, x / segments, y / segments));
        const point = displaced(n).sub(center);
        positions.push(...point.toArray());
        directions.push(...n.toArray());
        tangent
          .crossVectors(
            Math.abs(n.y) > 0.9
              ? new THREE.Vector3(1, 0, 0)
              : new THREE.Vector3(0, 1, 0),
            n,
          )
          .normalize();
        bitangent.crossVectors(n, tangent).normalize();
        const gradient = (axis: THREE.Vector3): THREE.Vector3 =>
          displaced(n.clone().addScaledVector(axis, 0.00001).normalize()).sub(
            displaced(n.clone().addScaledVector(axis, -0.00001).normalize()),
          );
        const normal = gradient(tangent).cross(gradient(bitangent)).normalize();
        normals.push(...normal.toArray());
        if (x < segments && y < segments) {
          const a = y * stride + x;
          indices.push(a, a + 1, a + stride, a + 1, a + stride + 1, a + stride);
        }
      }
    // Inward skirts cover different-resolution borders without changing the shared surface field.
    const edge: number[] = [];
    for (let x = 0; x < segments; x++) edge.push(x);
    for (let y = 0; y < segments; y++) edge.push(y * stride + segments);
    for (let x = segments; x > 0; x--) edge.push(segments * stride + x);
    for (let y = segments; y > 0; y--) edge.push(y * stride);
    const skirt = Math.min(
      2500,
      Math.max(15, (this.planet.radius / 2 ** t.level) * 0.09),
    );
    const first = positions.length / 3;
    for (const i of edge) {
      positions.push(
        positions[i * 3] - directions[i * 3] * skirt,
        positions[i * 3 + 1] - directions[i * 3 + 1] * skirt,
        positions[i * 3 + 2] - directions[i * 3 + 2] * skirt,
      );
      normals.push(...normals.slice(i * 3, i * 3 + 3));
      directions.push(...directions.slice(i * 3, i * 3 + 3));
    }
    for (let i = 0; i < edge.length; i++) {
      const j = (i + 1) % edge.length;
      indices.push(edge[i], first + i, edge[j], edge[j], first + i, first + j);
    }
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute(
      'position',
      new THREE.Float32BufferAttribute(positions, 3),
    );
    geometry.setAttribute(
      'normal',
      new THREE.Float32BufferAttribute(normals, 3),
    );
    geometry.setAttribute(
      'planetDirection',
      new THREE.Float32BufferAttribute(directions, 3),
    );
    geometry.setIndex(indices);
    geometry.computeBoundingSphere();
    const mesh = new THREE.Mesh(geometry, this.material);
    mesh.position.copy(center);
    return mesh;
  }
  diagnostics(): object {
    let bytes = 0;
    this.tiles.forEach((m) => {
      Object.values(m.geometry.attributes).forEach(
        (a) => (bytes += a.array.byteLength),
      );
      bytes += m.geometry.index?.array.byteLength ?? 0;
    });
    return {
      planet: this.planet.id,
      resident: this.tiles.size,
      visible: this.visibleTiles,
      queued: this.queue.length,
      limit:
        6 +
        Math.floor((QUALITY_BUDGETS[this.quality].terrainLeaves - 6) / 3) * 4,
      geometryBytes: bytes,
      generated: this.generated,
      evicted: this.evicted,
      peakResident: this.peakResident,
      lastBuildMs: this.lastBuildMs,
      peakBuildMs: this.peakBuildMs,
      ready: this.ready,
    };
  }
  dispose(): void {
    this.tiles.forEach((m) => m.geometry.dispose());
    this.tiles.clear();
    this.queue = [];
    this.material.dispose();
    this.group.clear();
  }
}
