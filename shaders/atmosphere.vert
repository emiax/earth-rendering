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

out vec3 mieColor;
out vec3 rayleighColor;
out vec3 rayDirection;

out vec3 deb;


void main(void)
{
    // Forward model space variables to fragment shader
    vs_modelPosition = vec3(in_position * (fOuterRadius));
    vs_modelNormal = in_normal;
	vs_st = in_st;

    // To world space
	vec4 position = model * vec4(in_position * (fOuterRadius), 1.0);
    vs_worldPosition = vec3(position);
    vs_worldNormal = mat3(model) * in_normal;

    // To perspective space (via view space)
    gl_Position = position = viewProjection * position;




// Get the ray from the camera to the vertex and its length (which is the far point of the ray passing through the atmosphere)
	vec3 v3Pos = vs_worldPosition.xyz;
	vec3 v3Ray = v3Pos - cameraPosition;
	float fFar = length(v3Ray);
	v3Ray /= fFar;

	//vec4 d = viewProjection * vec4(cameraPosition, 1.0);
	//deb = vs_worldPosition * 0.1;

	
	
//    deb = normalize(v3Pos - cameraPosition);

	//deb = vec3(1.0, 0.0, 0.0);
    
	
	// Calculate the closest intersection of the ray with the outer atmosphere (which is the near point of the ray passing through the atmosphere)
	float B = 2.0 * dot(cameraPosition, v3Ray);
	float C = dot(cameraPosition, cameraPosition) - fOuterRadius2;
	float fDet = max(0.0, B*B - 4.0 * C);
	float fNear = 0.5 * (-B - sqrt(fDet));

	
	//deb = -0.01*vec3(B);
	//deb = vec3(v3Ray);
	//deb = vec3(-cameraPosition);
	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 v3Start = cameraPosition + v3Ray * fNear;
	fFar -= fNear;
	float fStartAngle = dot(v3Ray, v3Start) / fOuterRadius;
	float fStartDepth = exp(-1.0 / fScaleDepth);
	float fStartOffset = fStartDepth*scale(fStartAngle);
	//deb = vec3(v3Start - v3Pos);
	//deb = vec3(vs_worldPosition.xyz);

	//deb = vec3(cameraPosition);
	
	// Initialize the scattering loop variables
	//gl_FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * fScale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);

    vec3 sunDirection = normalize(sunPosition);
	
	//deb = v3SampleRay;
	//deb = vec3(v3SamplePoint);
	
	
	for(int i=0; i<nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
		float fLightAngle = dot(sunDirection, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle) - scale(fCameraAngle)));
		vec3 v3Attenuate = exp(-fScatter * (v3InvWavelength * fKr4PI + fKm4PI));
		
		//rayleighColor = vec3(0.0000000000001 * fScatter * (v3InvWavelength * fKr4PI + fKm4PI));// * (v3InvWavelength * fKr4PI + fKm4PI);
		
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;

		//deb = vec3(v3Attenuate);
			
	}

	// Finally, scale the Mie and Rayleigh colors and set up the varying variables for the pixel shader
	mieColor = v3FrontColor * fKmESun;
	rayleighColor = v3FrontColor * (v3InvWavelength * fKrESun);
//	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	rayDirection = cameraPosition - v3Pos;

	//deb = rayleighColor;




}
