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

#include "./atmospheric.vert"

out vec3 atmosphereColor;
out vec3 attenuate;

out vec3 deb;


void main()
{
    // Forward model space variables to fragment shader
    vs_modelPosition = vec3(in_position * fInnerRadius);
    vs_modelNormal = in_normal;
	vs_st = in_st;

    // To world space
	vec4 position = model * vec4(in_position * fInnerRadius, 1.0);
    vs_worldPosition = vec3(position);
    vs_worldNormal = mat3(model) * in_normal;

    // To perspective space (via view space)
    gl_Position = position = viewProjection * position;


	vec3 v3Pos = vs_worldPosition.xyz;
	vec3 v3Ray = v3Pos - cameraPosition;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * dot(cameraPosition, v3Ray);
	float C = dot(cameraPosition, cameraPosition) - fOuterRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	float fNear = 0.5 * (-B - sqrt(fDet));

	//deb = -0.01*vec3(B);
	//deb = vec3(v3Pos);
	//deb = vec3(-cameraPosition);
	//deb = vec3(v3Ray);
	
// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = cameraPosition + v3Ray * fNear;

	fFar -= fNear;

	
	vec3 sunDirection = normalize(sunPosition);
	
	float fDepth = exp((fInnerRadius - fOuterRadius) / fScaleDepth);
	float fCameraAngle = dot(-v3Ray, v3Pos) / length(v3Pos);
	float fLightAngle = dot(normalize(sunDirection), v3Pos) / length(v3Pos);

	//deb = vec3(-fCameraAngle);
	
	float fCameraScale = scale(fCameraAngle);
	float fLightScale = scale(fLightAngle);


	
	float fCameraOffset = fDepth*fCameraScale;
	float fTemp = (fLightScale + fCameraScale);

	//attenuate = vec3(fLightScale);

			
	// Initialize the scattering loop variables
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;
//	atmosphereColor =  mod(v3SamplePoint, 0.1)/0.1;
//	atmosphereColor =  mod(v3Start, 0.1)/0.1;
	//atmosphereColor =  vec3(mod(v3Ray, 0.5)/0.5);


		//atmosphereColor = v3SampleRay;
	
	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0);
	vec3 v3Attenuate = vec3(0.0);




	atmosphereColor = vec3(0.0);

	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);

		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fScatter = fDepth*fTemp - fCameraOffset;

		//deb = vec3(fScatter*0.01); break;
		//deb = vec3(fTemp*0.000000000000000001); break;
		
		v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	//atmosphereColor = vec3(fSampleLength);
	
	atmosphereColor = v3FrontColor * (v3InvWavelength * fKrESun + fKmESun);
	// Calculate the attenuation factor for the ground
	attenuate = vec3(v3Attenuate);


	//deb.a = 0.0;
	//deb = vec3(v3SamplePoint);
}

