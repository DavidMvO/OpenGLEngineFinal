#version 410

in vec2 frag_texcoord;
in vec4 vColour;
in vec4 vShadowCoord;
in vec4 vNormal;

in vec4 vPosition;

out vec4 out_color;

uniform sampler2D perlin_texture;
uniform sampler2D m_grass_texture;
uniform sampler2D m_water_texture;
uniform sampler2D m_rocks_texture;

uniform vec3 lightDir;
uniform vec3 LightDir;
uniform sampler2D shadowMap;
uniform float shadowBias;

uniform vec3 LightColour; 
uniform vec3 cameraPos;
uniform float SpecPow;

void main()
{
	float d = max(0, dot(normalize(vNormal.xyz), lightDir));
	float a = 0.3;
	vec3 E = normalize(cameraPos - vPosition.xyz);
	vec3 R = reflect( -LightDir, vNormal.xyz);
	float s = max(0, dot(E, R));
	s = pow(s, SpecPow);
	float height = texture(perlin_texture, frag_texcoord).r;
	out_color = texture(perlin_texture, frag_texcoord).rrrr;
	out_color.a = 1;
	
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias)
	{
		d = 0;
	}
	
	if(height <= 0.45)
	{
		out_color = texture(perlin_texture, frag_texcoord).rrrr*texture(m_water_texture, frag_texcoord*2)*vec4( LightColour * s +LightColour * d, 1);
	}
	else if(height >= 0.45 && height <= 0.455)
	{
		out_color = texture(perlin_texture, frag_texcoord).rrrr*texture(m_rocks_texture, frag_texcoord*2)*vec4( LightColour * s +LightColour * d, 1);
	}
	else
	{
		out_color = texture(perlin_texture, frag_texcoord).rrrr*texture(m_grass_texture, frag_texcoord*2)*vec4( LightColour * s +LightColour * d, 1);
	}
}