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


void main()

{


    vec2 center = vec2(0.5, 0.5);
    vec2 fromCenter = vs_st - center;
    float dist = length(fromCenter);
    float inside = 1.0 - smoothstep(0.01, 0.10, dist);

    float angle = atan(fromCenter.y, fromCenter.x);

    float ray = 10.0 * clamp(pow(abs(clamp(sin(angle*4) - 3*dist, 0.0, 1.0)), 8.0), 0.0, 1.0) * (0.3 - dist);

    ray += 10.0 * clamp(pow(abs(clamp(sin((angle + 0.1)*16) - 6*dist, 0.0, 1.0)), 5.0), 0.0, 1.0) * (0.3 - dist);

    
    //color = vec4(vec3(ray), 1.0);
    
    //ray += 40.0 * clamp(pow(abs(sin(angle*8)) - dist*7, 4.0), 0.0, 1.0) * (0.3 - dist);
    
                             //ray += 40.0 * clamp(pow(abs(sin(angle*16)) - dist*7, 4.0), 0.0, 1.0) * (0.3 - dist);

    float glare = (1.0 - smoothstep(0.0, 0.5, pow(dist, 0.6)));

    vec3 light = vec3(1.0, 1.0, 1.0) * inside + vec3(1.0, 0.7, 0.4) * glare +  vec3(1.0, 1.0, 1.0) * ray;
    
    color = vec4(light.x, light.y, light.z, light.r);
    //    color = vec4(vec3(gl_FragCoord.z * 0.1), 1.0);  
    //color = vec4(vec3(ray), 1.0);

    //color = vec4(vec3(, 1.0);
}
