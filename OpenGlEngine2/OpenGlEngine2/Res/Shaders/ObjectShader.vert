#version 410

layout(location=0) in vec4 Position;																								
layout(location=1) in vec4 Normal;																									
layout(location=2) in vec4 Tangent;																									
layout(location=3) in vec2 TexCoord;																																																
																																								
out vec3 vPosition;																													
out vec2 vTexCoord;																													
out vec3 vNormal;																													
out vec3 vTangent;																													
out vec3 vBiTangent;
out vec4 vShadowCoord;																												
																																								
uniform mat4 ProjectionView;
uniform mat4 LightMatrix;

void main()
{
	vec4 pos = Position;
	vNormal = Normal.xyz;
	vTexCoord = TexCoord;
	vTangent = Tangent.xyz;
	vBiTangent = cross(vNormal, vTangent);
	gl_Position = ProjectionView * pos;
	vShadowCoord = LightMatrix * Position;
}