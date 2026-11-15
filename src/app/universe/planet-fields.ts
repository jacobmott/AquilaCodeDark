// The same smooth field drives orbital color, terrain geometry and ground clearance.
// Units are metres. Detail is deterministic in planet space, independent of tile LOD.
export function continental(
  x: number,
  y: number,
  z: number,
  seed: number,
): number {
  return (
    Math.sin(x * 5 + seed * 0.7) * Math.cos(z * 4.2 - seed * 0.3) * 0.55 +
    Math.sin(y * 7.3 + z * 3 + seed) * 0.28 +
    Math.sin((x + z) * 11 - y * 9 - seed * 0.2) * 0.12
  );
}
export function terrainHeight(
  x: number,
  y: number,
  z: number,
  seed: number,
): number {
  const land = Math.max(0, continental(x, y, z, seed) - 0.015);
  const ridge = 1 - Math.abs(Math.sin(x * 87 + y * 63 + z * 51 + seed));
  const hills = Math.sin(x * 380 + z * 290) * Math.cos(y * 320 - z * 170) * 18;
  return Math.max(
    0,
    land * land * 1850 + Math.min(1, land * 8) * (ridge * ridge * 145 + hills),
  );
}
export const terrainFieldGLSL = `
float continental(vec3 p,float s){
  return sin(p.x*5.+s*.7)*cos(p.z*4.2-s*.3)*.55
    +sin(p.y*7.3+p.z*3.+s)*.28+sin((p.x+p.z)*11.-p.y*9.-s*.2)*.12;
}
float terrainHeight(vec3 p,float s){
  float land=max(0.,continental(p,s)-.015);
  float ridge=1.-abs(sin(p.x*87.+p.y*63.+p.z*51.+s));
  float hills=sin(p.x*380.+p.z*290.)*cos(p.y*320.-p.z*170.)*18.;
  return max(0.,land*land*1850.+min(1.,land*8.)*(ridge*ridge*145.+hills));
}`;

export const planetNoiseGLSL = `
float hash3(vec3 p){p=fract(p*.3183099+vec3(.1,.2,.3));p*=17.;return fract(p.x*p.y*p.z*(p.x+p.y+p.z));}
float noise3(vec3 p){vec3 i=floor(p),f=fract(p);f=f*f*(3.-2.*f);
 return mix(mix(mix(hash3(i),hash3(i+vec3(1,0,0)),f.x),mix(hash3(i+vec3(0,1,0)),hash3(i+vec3(1,1,0)),f.x),f.y),
 mix(mix(hash3(i+vec3(0,0,1)),hash3(i+vec3(1,0,1)),f.x),mix(hash3(i+vec3(0,1,1)),hash3(i+vec3(1,1,1)),f.x),f.y),f.z);}
float cloudNoise(vec3 p){float value=0.,a=.5;for(int i=0;i<4;i++){if(i==2&&detail<.8)break;value+=noise3(p)*a;p=p*2.04+13.;a*=.5;}return value;}
`;
