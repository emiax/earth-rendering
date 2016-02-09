#version __CONTEXT__

uniform mat4 viewProjection;
uniform mat4 model;

uniform vec3 sunPosition;
uniform vec3 cameraPosition;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_st;
layout(location = 2) in vec3 in_normal;

out vec2 vs_st;
out vec3 vs_modelNormal;
out vec3 vs_modelPosition;

out vec3 vs_worldNormal;
out vec3 vs_worldPosition;

out vec3 deb;


void main()
{

    float sunSize = 20000;
    // Forward model space variables to fragment shader
    vs_modelPosition = vec3(in_position * sunSize);
    vs_modelNormal = in_normal;
	vs_st = in_st;

    // To world space
	vec4 position = model * vec4(in_position * sunSize, 1.0);
    vs_worldPosition = vec3(position);
    vs_worldNormal = mat3(model) * in_normal;

    // To perspective space (via view space)
    gl_Position = position = viewProjection * position;

}

