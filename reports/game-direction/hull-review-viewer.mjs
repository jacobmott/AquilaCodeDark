import * as THREE from 'three';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';
import { GLTFLoader } from 'three/examples/jsm/loaders/GLTFLoader.js';
import { RoomEnvironment } from 'three/examples/jsm/environments/RoomEnvironment.js';

const ships = {
  'delta-tree': { name: 'Delta Tree', image: '58-delta-tree', eye: [.96,.76,-1], roll: -1, lift: .034 },
  'glacier-tongue': { name: 'Glacier Tongue', image: '41-glacier-tongue', eye: [.94,.205,-1], roll: -5, lift: .045 },
};
const params = new URLSearchParams(location.search);
const requested = params.get('ship');
const key = Object.hasOwn(ships, requested) ? requested : 'delta-tree';
const ship = ships[key];
const element = id => document.getElementById(id);
const stage = element('stage');
const requestedRound = params.get('round');
const revisions = ['174','175','176','177','178','179','184','185','186','187','192','193','194','195','196','197','198','200'];
const comparing = revisions.includes(requestedRound);
const round = comparing ? Number(requestedRound) : 174;
const requestedPair = params.get('compare');
const index = revisions.indexOf(String(round));
const next = revisions[index + 1];
const latest = next && requestedPair === next ? Number(next) : Math.max(175,round);
const earlier = Number(revisions[revisions.indexOf(String(latest))-1]);
const previous = comparing && round < latest;
const matchedFrame = latest >= 193;
function revisionUrl(shipKey, version) {
  const selection = new URLSearchParams({ship: shipKey, round: String(version)});
  if (version >= 175 && version < latest) selection.set('compare',String(latest));
  return '?' + selection;
}
const prefix = 'media/r' + round + '-' + key;
const glb = prefix + '.glb';
const modelLabel = previous ? 'Previous model' : 'Revised model';
element('model-label').textContent = modelLabel;
if (comparing) {
  for (const link of document.querySelectorAll('[data-ship]')) link.href = revisionUrl(link.dataset.ship,round);
  const other = element('revision');
  other.href = revisionUrl(key,previous ? latest : earlier);
  other.textContent = previous ? 'Show revised model' : 'Show previous model';
  other.hidden = false;
}
element('title').textContent = ship.name;
document.title = ship.name + ' · Hull review';
document.querySelector('[data-ship="' + key + '"]').setAttribute('aria-current', 'page');
element('original').src = 'media/r173-' + ship.image + '.png';
element('original').alt = ship.name + ' in the original concept';
element('fallback').src = matchedFrame && previous ? 'media/r' + latest + '-' + key + '-earlier-gallery.png' : prefix + '-gallery.png';
element('fallback').alt = ship.name + ' · ' + modelLabel + ' at the gallery angle';
element('download').href = glb;

async function startViewer() {
  const renderer = new THREE.WebGLRenderer({ antialias: true, powerPreference: 'high-performance' });
  renderer.setPixelRatio(Math.min(devicePixelRatio, 2));
  renderer.setClearColor(0x0c151c);
  renderer.toneMapping = THREE.ACESFilmicToneMapping;
  renderer.toneMappingExposure = .88;
  renderer.domElement.tabIndex = 0;
  renderer.domElement.setAttribute('aria-label', 'Rotate ' + ship.name);
  const scene = new THREE.Scene();
  const prefilter = new THREE.PMREMGenerator(renderer);
  const room = new RoomEnvironment();
  const reflections = prefilter.fromScene(room);
  scene.environment = reflections.texture;
  scene.environmentIntensity = .44;
  room.dispose();
  prefilter.dispose();
  for (const [position, intensity, color] of [[[1,1.6,.3],2.6,0xffefe0],[[-.6,1,-.95],1.6,0xbacfea],[[.2,.3,-1],.64,0xd9e6ff]]) {
    const light = new THREE.DirectionalLight(color, intensity);
    light.position.set(...position);
    scene.add(light);
  }
  const fittedLens = latest === 200 && key === 'delta-tree';
  const camera = fittedLens ? new THREE.PerspectiveCamera(28,1,1,14000) : new THREE.OrthographicCamera(-1,1,1,-1,1,14000);
  const controls = new OrbitControls(camera, renderer.domElement);
  controls.minZoom = .36;
  controls.maxZoom = 20;
  controls.rotateSpeed = .78;
  controls.zoomSpeed = .9;
  const [asset, savedView] = await Promise.all([
    new GLTFLoader().loadAsync(glb),
    comparing ? fetch(fittedLens ? 'media/r200-perspective.json' : matchedFrame ? 'media/r193-gallery-views.json' : 'media/r175-gallery-views.json').then(response => {
      if (!response.ok) throw new Error('Gallery camera unavailable');
      return response.json();
    }).then(views => views[key]) : null,
  ]);
  const model = asset.scene;
  scene.add(model);
  model.updateMatrixWorld(true);
  const meshes = [];
  const points = [];
  const clay = new THREE.MeshStandardMaterial({ color: 0xa8adb0, roughness: .76, metalness: .02, side: THREE.DoubleSide });
  model.traverse(node => {
    if (!node.isMesh) return;
    meshes.push([node, node.material]);
    const vertices = node.geometry.attributes.position;
    const step = Math.max(1, Math.floor(vertices.count/7500));
    for (let i=0; i<vertices.count; i+=step) points.push(new THREE.Vector3().fromBufferAttribute(vertices,i).applyMatrix4(node.matrixWorld));
  });
  let framing = null;
  let verticalSpan = 1440;
  let paintedCenter = false;
  const draw = () => renderer.render(scene,camera);
  controls.addEventListener('change',draw);
  controls.addEventListener('start',() => { framing = null; });
  function resize() {
    const w = Math.max(1,stage.clientWidth), h = Math.max(1,stage.clientHeight);
    renderer.setSize(w,h,false);
    if (fittedLens) {
      const span = Math.max(1152,1152*h/w);
      camera.aspect = w/h;
      camera.fov = THREE.MathUtils.radToDeg(2*Math.atan(span/(2*savedView.focal_px)));
      camera.clearViewOffset();
      if (paintedCenter) {
        const [cx,cy] = savedView.principal_xy;
        camera.setViewOffset(w,h,(512-cx)*h/span,(512-cy)*h/span,w,h);
      }
    } else {
      const half = verticalSpan/2;
      Object.assign(camera,{left:-half*w/h,right:half*w/h,top:half,bottom:-half});
    }
    camera.updateProjectionMatrix();
    draw();
  }
  function frame(stern = false) {
    if (!stern && savedView) {
      const m = savedView.camera_to_world;
      const column = i => new THREE.Vector3(m[0][i],m[2][i],-m[1][i]);
      const dir = column(2).normalize();
      if (fittedLens) {
        paintedCenter = true;
        camera.up.copy(column(1)).normalize();
        camera.position.copy(column(3));
        controls.target.set(0,0,0);
        camera.zoom=1;
        camera.lookAt(controls.target);
        controls.update();
        framing=false;
        resize();
        return;
      }
      const [left,right,bottom,top] = savedView.frame;
      const aspect = stage.clientWidth / Math.max(1,stage.clientHeight);
      verticalSpan = Math.max(top-bottom,(right-left)/aspect);
      camera.up.copy(column(1)).normalize();
      camera.position.copy(column(3)).addScaledVector(column(0),(left+right)/2).addScaledVector(camera.up,(top+bottom)/2);
      controls.target.copy(camera.position).addScaledVector(dir,-2300);
      camera.zoom=1;
      camera.lookAt(controls.target);
      controls.update();
      framing=false;
      resize();
      return;
    }
    const dir = new THREE.Vector3(...(stern ? [-1,.60,.68] : ship.eye)).normalize();
    camera.up.set(0,1,0).applyAxisAngle(dir, THREE.MathUtils.degToRad(stern ? 0 : ship.roll));
    const right = new THREE.Vector3().crossVectors(camera.up,dir).normalize();
    const up = new THREE.Vector3().crossVectors(dir,right).normalize();
    let minX=Infinity,maxX=-Infinity,minY=Infinity,maxY=-Infinity,minZ=Infinity,maxZ=-Infinity;
    for (const p of points) {
      const x=p.dot(right),y=p.dot(up),z=p.dot(dir);
      minX=Math.min(minX,x);maxX=Math.max(maxX,x);minY=Math.min(minY,y);maxY=Math.max(maxY,y);
      minZ=Math.min(minZ,z);maxZ=Math.max(maxZ,z);
    }
    const aspect = stage.clientWidth/Math.max(1,stage.clientHeight);
    verticalSpan = Math.max((maxX-minX)/aspect,maxY-minY)/.94;
    const center = right.multiplyScalar((maxX+minX)/2)
      .addScaledVector(up,(maxY+minY)/2-(stern?0:ship.lift*verticalSpan)).addScaledVector(dir,(maxZ+minZ)/2);
    controls.target.copy(center);
    paintedCenter=false;
    const distance = fittedLens ? verticalSpan/(2*Math.tan(THREE.MathUtils.degToRad(camera.fov)/2))+(maxZ-minZ)/2 : 2400;
    camera.position.copy(center).addScaledVector(dir,distance);
    camera.zoom=1;
    camera.lookAt(center);
    controls.update();
    framing=stern;
    resize();
  }
  element('reset').addEventListener('click',() => frame(false));
  element('stern').addEventListener('click',() => frame(true));
  element('clay').addEventListener('click',() => {
    const enabled=element('clay').getAttribute('aria-pressed')!=='true';
    element('clay').setAttribute('aria-pressed',String(enabled));
    element('model-label').textContent=enabled?modelLabel+' · plain gray':modelLabel;
    for (const [mesh,finish] of meshes) mesh.material=enabled?clay:finish;
    draw();
  });
  stage.append(renderer.domElement);
  frame();
  new ResizeObserver(()=>{ if(framing===null) resize(); else frame(framing); }).observe(stage);
  element('fallback').hidden=true;
  element('help').textContent='Drag to rotate. Scroll or pinch to zoom.';
}
startViewer().catch(() => {
  stage.querySelector('canvas')?.remove();
  element('fallback').hidden=false;
  element('views').hidden=true;
  element('help').textContent='Showing the rendered view. Download the model to rotate it in a 3D viewer.';
});
