#version __CONTEXT__

//
// Atmospheric scattering fragment shader
//
// Author: Sean O'Neil
//
// Copyright (c) 2004 Sean O'Neil
//

uniform vec3 sunPosition;
uniform float g = 0.001;
uniform float g2 = 0.5;

in vec3 rayDirection;
in vec3 rayleighColor;
in vec3 mieColor;  
in vec3 deb;

out vec4 color;

void main (void)
{
    vec3 sunDirection = normalize(sunPosition);
	float fCos = dot(-sunDirection, rayDirection) / length(rayDirection);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
    color = vec4(rayleighColor, 1.0) + fMiePhase * vec4(mieColor, 1.0);
    color.a = color.b;
    color.rg *= 0.5;

    color.rgba *= 0.5;

    



    //color = vec4(0.0);
    //color.a *= 0.1;

    //    color = vec4(1.0, 0.0, 0.0, 1.0);
    //color = vec4(rayleighColor, 1.0);

    if (deb != vec3(0.0)) {
        color = vec4(deb, 1.0);
    }

    
}
