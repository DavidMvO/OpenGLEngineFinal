#version 410

layout(location=0) in vec4 Position; 
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 Colour;  
layout(location=3) in vec4 Normal;  

out vec2 frag_texcoord;
out vec4 vColour;
out vec4 vNormal;
out vec4 vShadowCoord;

out vec4 vPosition;

uniform mat4 ProjectionView;
uniform mat4 LightMatrix;
 
uniform sampler2D perlin_texture;
uniform sampler2D m_grass_texture;
uniform sampler2D m_water_texture;
uniform sampler2D m_rocks_texture;

void main()
{
	vNormal = Normal;
	vec4 pos = Position;
	pos.y += texture(perlin_texture, texcoord).r;
	frag_texcoord = texcoord;
	vPosition = pos;
	gl_Position = ProjectionView * pos;
	vShadowCoord = LightMatrix * pos;
}