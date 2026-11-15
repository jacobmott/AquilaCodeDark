import * as THREE from 'three';
import {
  terrainFieldGLSL,
  terrainHeight,
  planetNoiseGLSL,
} from './planet-fields';
import { Planet, Quality, QUALITY_BUDGETS } from './universe-model';

// Custom materials must use the renderer's depth convention too, including sky and rings.
export function configureShaderDepth(root: THREE.Object3D): void {
  root.traverse((node) => {
    const value = (node as THREE.Mesh).material;
    for (const material of Array.isArray(value)
      ? value
      : value
      ? [value]
      : []) {
      if (
        !(material instanceof THREE.ShaderMaterial) ||
        material.vertexShader.includes('logdepthbuf_pars_vertex')
      )
        continue;
      const end = material.vertexShader.lastIndexOf('}');
      material.vertexShader =
        '#include <common>\n#include <logdepthbuf_pars_vertex>\n' +
        material.vertexShader.slice(0, end) +
        '\n#include <logdepthbuf_vertex>\n' +
        material.vertexShader.slice(end);
      material.fragmentShader =
        '#include <logdepthbuf_pars_fragment>\n' +
        material.fragmentShader.replace(
          /void main\s*\(\s*\)\s*\{/,
          'void main(){\n#include <logdepthbuf_fragment>\n',
        );
      material.needsUpdate = true;
    }
  });
}
export function createPlanet(planet: Planet): THREE.Group {
  const group = new THREE.Group();
  const geometry = new THREE.SphereGeometry(planet.radius, 48, 32);
  const positions = geometry.getAttribute('position'),
    normal = new THREE.Vector3();
  for (let i = 0; i < positions.count; i++) {
    normal.fromBufferAttribute(positions, i).normalize();
    normal.multiplyScalar(
      planet.radius + terrainHeight(normal.x, normal.y, normal.z, planet.seed),
    );
    positions.setXYZ(i, normal.x, normal.y, normal.z);
  }
  geometry.computeVertexNormals();
  const vertex = `varying vec3 direction;varying vec3 norm;varying vec3 eye;
    void main(){direction=normalize(position);norm=normalize(mat3(modelMatrix)*normal);
      vec4 p=modelMatrix*vec4(position,1.);eye=cameraPosition-p.xyz;gl_Position=projectionMatrix*viewMatrix*p;}`;
  const cloudsGLSL = `${planetNoiseGLSL} float clouds(vec3 p,float s){
    float swirl=sin(p.y*7.+s)*.3+cloudNoise(p*4.+s)*.6;
    p.xz=mat2(cos(swirl),-sin(swirl),sin(swirl),cos(swirl))*p.xz;
    float broad=cloudNoise(p*7.+vec3(s,s*.1,2.));
    float fine=cloudNoise(p*45.+s);
    return smoothstep(.42,.62,broad+fine*.18);}`;
  const material = new THREE.ShaderMaterial({
    uniforms: {
      seed: { value: planet.seed },
      detail: { value: 0.6 },
      ocean: {
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
    vertexShader: vertex,
    fragmentShader: `uniform float seed;uniform float detail;uniform vec3 ocean;uniform vec3 land;
      varying vec3 direction;varying vec3 norm;varying vec3 eye;${terrainFieldGLSL} ${cloudsGLSL}
      void main(){vec3 p=normalize(direction),n=normalize(norm),light=normalize(vec3(-.7,.6,1.));
        float h=terrainHeight(p,seed),c=continental(p,seed),coast=smoothstep(.005,.035,c);
        vec3 water=mix(ocean,vec3(.08,.32,.33),smoothstep(-.04,.015,c)*.5);
        float texture=noise3(p*120.+seed);
        vec3 rock=land*(.65+texture*.3*detail);
        rock=mix(rock,vec3(.42,.43,.39),smoothstep(180.,850.,h)*.65);
        rock=mix(rock,vec3(.8,.84,.84),smoothstep(1100.,1500.,h));
        float sun=max(0.,dot(n,light));
        vec3 color=mix(water,rock,coast)*(.10+sun*1.35);
        if(detail>.8)color*=1.-clouds(normalize(p+light*.02),seed)*.18;
        float spec=pow(max(0.,dot(reflect(-light,n),normalize(eye))),70.);
        color+=vec3(.55,.63,.65)*spec*(1.-coast)*detail*.6;
        float rim=pow(1.-max(0.,dot(n,normalize(eye))),4.);
        color+=vec3(.05,.15,.22)*rim*sun;
        gl_FragColor=vec4(color,1.);
        #include <tonemapping_fragment>
        #include <colorspace_fragment>
      }`,
  });
  const globe = new THREE.Mesh(geometry, material);
  globe.name = 'globe';
  group.add(globe);
  const cloud = new THREE.Mesh(
    new THREE.SphereGeometry(planet.radius + 2200, 48, 32),
    new THREE.ShaderMaterial({
      transparent: true,
      depthWrite: false,
      uniforms: { seed: { value: planet.seed }, detail: { value: 0.6 } },
      vertexShader: vertex,
      fragmentShader: `uniform float seed;uniform float detail;varying vec3 direction;varying vec3 norm;varying vec3 eye;${cloudsGLSL}
      void main(){float coverage=clouds(normalize(direction),seed);
        vec3 color=vec3(.84,.9,.94)*(.06+max(0.,dot(normalize(norm),normalize(vec3(-.7,.6,1.))))*1.3);
        gl_FragColor=vec4(color,coverage*.64);
        #include <tonemapping_fragment>
        #include <colorspace_fragment>
      }`,
    }),
  );
  cloud.name = 'clouds';
  group.add(cloud);
  if (planet.rings) {
    const rings = new THREE.Mesh(
      new THREE.RingGeometry(planet.radius * 1.35, planet.radius * 2.1, 96),
      new THREE.ShaderMaterial({
        transparent: true,
        side: THREE.DoubleSide,
        depthWrite: false,
        uniforms: { radius: { value: planet.radius } },
        vertexShader:
          'varying vec3 p;void main(){p=position;gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.);}',
        fragmentShader: `uniform float radius;varying vec3 p;void main(){float r=length(p.xy)/radius;
          float stripes=.35+.35*sin(r*143.)+.15*sin(r*347.);
          float edge=smoothstep(1.35,1.42,r)*(1.-smoothstep(1.99,2.1,r));
          float gap=1.-smoothstep(.008,.016,abs(r-1.78));
          gl_FragColor=vec4(vec3(.52,.50,.43),clamp(stripes,.14,.75)*edge*(1.-gap*.85)*.55);}`,
      }),
    );
    rings.rotation.set(1.23, -0.19, -0.3);
    group.add(rings);
  }
  configureShaderDepth(group);
  return group;
}
export function updatePlanet(
  object: THREE.Group,
  quality: Quality,
  distance: number,
  radius: number,
): void {
  const profile = QUALITY_BUDGETS[quality];
  (
    object.getObjectByName('globe') as THREE.Mesh<
      THREE.BufferGeometry,
      THREE.ShaderMaterial
    >
  ).material.uniforms['detail'].value = profile.detail;
  object.getObjectByName('clouds')!.visible =
    profile.clouds && distance < radius * 8;
  (
    object.getObjectByName('clouds') as THREE.Mesh<
      THREE.BufferGeometry,
      THREE.ShaderMaterial
    >
  ).material.uniforms['detail'].value = profile.detail;
}
