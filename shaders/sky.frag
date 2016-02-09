#version __CONTEXT__

uniform uvec2 nStarsTiles;
uniform sampler2D starsTextures[8];

in vec2 vs_st;
in vec3 vs_modelPosition;
in vec3 vs_modelNormal;

in vec3 vs_worldNormal;
in vec3 vs_worldPosition;

in vec3 deb;
out vec4 color;

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

    vec2 texCoords = vs_st + vec2(-0.001, 0.0);

    uint textureIndex;
    vec2 st;
    textureCoords(texCoords, nStarsTiles, st, textureIndex);
    
    float mipMapLevel = mipMapLevel(starsTextures[0], texCoords, nStarsTiles);
    color = textureLod(starsTextures[textureIndex], st, mipMapLevel);

    //color = vec4(vec3(gl_FragCoord.z * 0.1), 1.0);
}
