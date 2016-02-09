#version __CONTEXT__

uniform uvec2 nDayTiles;
uniform uvec2 nNightTiles;
uniform uvec2 nGlowTiles; 
uniform uvec2 nCloudTiles;
uniform uvec2 nWaterTiles;

uniform float scaleDepth = 0.5;
uniform float atmosphereDepth = 0.01;

uniform vec3 sunPosition;
uniform vec3 cameraPosition;

uniform sampler2D dayTextures[8];
uniform sampler2D nightTextures[8];
uniform sampler2D glowTextures[8];
uniform sampler2D cloudTextures[8];
uniform sampler2D waterTextures[1];

uniform vec3 v3InvWavelength;	// 1 / pow(wavelength, 4) for the red, green, and blue channels
uniform float fCameraHeight;	// The camera's current height
uniform float fCameraHeight2;	// fCameraHeight^2
uniform float fOuterRadius;		// The outer (atmosphere) radius
uniform float fOuterRadius2;	// fOuterRadius^2
uniform float fInnerRadius;		// The inner (planetary) radius
uniform float fInnerRadius2;	// fInnerRadius^2
uniform float fKrESun;			// Kr * ESun
uniform float fKmESun;			// Km * ESun
uniform float fKr4PI;			// Kr * 4 * PI
uniform float fKm4PI;			// Km * 4 * PI
uniform float fScale;			// 1 / (fOuterRadius - fInnerRadius)
uniform float fScaleDepth;		// The scale depth (i.e. the altitude at which the atmosphere's average density is found)
uniform float fScaleOverScaleDepth;	// fScale / fScaleDepth

uniform float time;


in vec2 vs_st;
in vec3 vs_modelPosition;
in vec3 vs_modelNormal;

in vec3 vs_worldNormal;
in vec3 vs_worldPosition;

in vec3 atmosphereColor;
in vec3 attenuate;

in vec3 deb;
out vec4 color;





//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

// end of noise



void textureCoords(vec2 inCoord, uvec2 nTiles, inout vec2 outCoord, inout uint textureIndex) {
    ivec2 coords = ivec2(int(inCoord.x * float(nTiles.x)), int(inCoord.y * float(nTiles.y)));
    vec2 fCoords = vec2(coords);
    vec2 fNTiles = vec2(nTiles);
    vec2 scale = vec2(1.0, 1.0)*fNTiles;
    outCoord = inCoord*scale - vec2(coords);
    textureIndex = coords.x + coords.y * nTiles.x;
}

float mipMapLevel(sampler2D texture, vec2 inCoord, uvec2 nTiles) {
    return textureQueryLod(texture, inCoord*vec2(nTiles)).x;
}



void main()
{

    		
    vec3 n = normalize(vs_worldNormal.xyz);
    vec3 lightDirection = normalize(sunPosition);

    vec3 camDirection = vs_worldPosition - cameraPosition;
    float camDistance = length(camDirection);
    camDirection = normalize(camDirection);
    
    
    /// textures!

    vec2 texCoords = vs_st + vec2(-0.001, 0.0);

    uint dayTextureIndex, nightTextureIndex, cloudTextureIndex, glowTextureIndex, waterTextureIndex;
    vec2 daySt, nightSt, glowSt, cloudSt, waterSt;


    float noiseFactor =  0.005 * sin(time/2000.0);
    vec2 displacedTexCoords = mod(texCoords
                                  + noiseFactor * snoise(time/16000.0 + texCoords * 4.0)
                                  + 0.25 * noiseFactor * snoise(time/8000.0 + texCoords * 8.0)                                                           , 1.0);


    
    textureCoords(texCoords, nDayTiles, daySt, dayTextureIndex);
    textureCoords(texCoords, nNightTiles, nightSt, nightTextureIndex);
    textureCoords(texCoords, nGlowTiles, glowSt, glowTextureIndex);
    textureCoords(displacedTexCoords * 0.000001 + texCoords, nCloudTiles, cloudSt, cloudTextureIndex);
    textureCoords(texCoords, nWaterTiles, waterSt, waterTextureIndex);

    float mipMapLevelDay = mipMapLevel(dayTextures[0], texCoords, nDayTiles);
    float mipMapLevelNight = mipMapLevel(nightTextures[0], texCoords, nNightTiles);
    float mipMapLevelGlow = mipMapLevel(glowTextures[0], texCoords, nGlowTiles);
    float mipMapLevelCloud = mipMapLevel(cloudTextures[0], texCoords, nCloudTiles);
    float mipMapLevelWater = mipMapLevel(waterTextures[0], texCoords, nWaterTiles);
    

    vec4 diffuseDay = textureLod(dayTextures[dayTextureIndex], daySt, mipMapLevelDay);

    vec4 diffuseNight = textureLod(nightTextures[nightTextureIndex], nightSt, mipMapLevelNight);
    vec4 diffuseGlow = textureLod(glowTextures[glowTextureIndex], glowSt, mipMapLevelGlow);
    vec4 glow = textureLod(nightTextures[nightTextureIndex], glowSt, mipMapLevelGlow);
    vec4 diffuseCloud = textureLod(cloudTextures[cloudTextureIndex], cloudSt, mipMapLevelCloud);
    float water = textureLod(waterTextures[waterTextureIndex], waterSt, mipMapLevelDay).a;

        
    diffuseDay.xyz += diffuseCloud.xyz;
    diffuseDay = clamp(diffuseDay, 0.0, 1.0);

    diffuseNight.rgb *= 0.8;
    //diffuseNight.rgb -= 0.5*diffuseCloud.rgb;
    diffuseNight.rgb = clamp(diffuseNight.rgb, 0.0, 1.0);
    //diffuseNight.rgb += diffuseCloud.rgb*0.2;

    diffuseNight.rgb += diffuseGlow.rgb;

    
    

    //float sgn = sign(dot(n, lightDirection));
    float mixer = dot(n, lightDirection);

    mixer = clamp(mixer, 0.0, 1.0);

    float dayCoefficient = mixer;
    float nightCoefficient = 1.0 - mixer;
    nightCoefficient *= nightCoefficient;
    nightCoefficient = clamp(nightCoefficient - 0.4, 0.0, 1.0);

    vec3 mixTex = diffuseDay.rgb * dayCoefficient + diffuseNight.rgb * nightCoefficient;
	
    vec4 diffuse = vec4(mixTex.rgb, 1.0);
    
    //color = vec4(vs_st, sunPosition.x * cameraPosition.x, 1.0);
    color = diffuse;
    //color = vec4(vec3(length(entry - exit)), 1.0);
    //color = vec4(1.0);
    //color = vec4(atmosphereColor, 1.0);

    //color.xyz += atmosphereColor + 0.25 * attenuate;
    vec3 ac = atmosphereColor;
    ac.rg *= 0.9;

    color.xyz += clamp(ac, vec3(0.0), vec3(1.0));


    float specular1 = pow(clamp(dot(normalize(lightDirection - camDirection), n), 0.0, 1.0), 20.0);
    float specular2 = pow(clamp(dot(normalize(lightDirection - camDirection), n), 0.0, 1.0), 60.0);

    specular1 *= clamp(water - diffuseCloud.r, 0.0, 1.0);
    specular2 *= clamp(water - diffuseCloud.r, 0.0, 1.0);
    //color = vec4(vec3(specular), 1.0);
    color.rgb += vec3(1.0, 1.0, 0.7) * specular1 * 0.1;
    color.rgb += vec3(1.0, 1.0, 0.7) * specular2 * 0.3;

    //color.rgba = diffuseDay;

    //color.a = smoothstep(length(vec3(0.0, 0.0, 0.7) - diffuse.rgb), 0.7, 2.0);
    
    //color.a = smoothstep(diffuseDay.b - diffuseDay.g, 0.0, 1.0);
    //color = vec4(-cameraPosition, 1.0);
    //color = vec4(vec3(mixer), 1.0);
    //color = vec4(n, 1.0);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
    //color.a = 0.0;
    //color = vec4(0.0, 0.0, 0.0, 1.0);
    //color = vec4(1.0, 0.0, 0.0, 1.0);

    if (deb != vec3(0.0)) {
        color = vec4(deb, 1.0);
    }


    //color = vec4(vec3(mod(time, 20.0)/20.0), 1.0);

    //color = vec4(cloudSt, 0.0, 1.0);
    
    //color = vec4(texture(waterTextures[0], waterSt).a, 0.0, 0.0, 1.0);

    //color = vec4(mod(vs_st, vec2(1.0/8000.0))*vec2(8000.0), 0.0, 1.0);
    //color = vec4(vec3(gl_FragCoord.z * 0.1), 1.0);  
//    color = vec4(0.0);
}
