#version 410

layout(location=0) in vec4 Position;																								
layout(location=1) in vec4 Normal;																									
layout(location=2) in vec4 Tangent;																									
layout(location=3) in vec2 TexCoord;																								
layout(location=4) in vec4 Weights;																									
layout(location=5) in vec4 Indices;																									

out vec3 vPosition;																													
out vec2 vTexCoord;																													
out vec3 vNormal;																													
out vec3 vTangent;																													
out vec3 vBiTangent;		
out vec4 vShadowCoord;																											

uniform mat4 ProjectionView;
uniform mat4 LightMatrix;																										
uniform mat4 global;																												

const int MAX_BONES = 128;																											
uniform mat4 bones[MAX_BONES];	
	
void main()																															
{                           
	vec4 pos = Position;
	pos.x -= 25;
	pos.z -= 50;
	vPosition = pos.xyz;
	vTexCoord = TexCoord;																											
	vNormal = Normal.xyz;																											
	vTangent = Tangent.xyz;																											
	vBiTangent = cross(vNormal, vTangent);																							
	gl_Position = ProjectionView * pos;
	vShadowCoord = LightMatrix * pos;	
	
	ivec4 index = ivec4(Indices);																									
	
	vec4 P = bones[index.x]*pos*Weights.x;																						
	P += bones[index.y]*pos*Weights.y;																							
	P += bones[index.z]*pos*Weights.z;																							
	P += bones[index.w]*pos*Weights.w;																							
	
	gl_Position = ProjectionView * global * P;		
}