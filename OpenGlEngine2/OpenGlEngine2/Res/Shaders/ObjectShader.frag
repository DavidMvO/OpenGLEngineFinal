#version 410

in vec2 vTexCoord;																													
in vec3 vNormal;																													
in vec3 vTangent;																													
in vec3 vBiTangent;	
in vec4 vShadowCoord;

out vec4 FragColour;
uniform vec3 lightDir;
uniform sampler2D shadowMap;
uniform float shadowBias;

uniform sampler2D diffuse;
uniform sampler2D normal;

void main()
{
	float a = 0.05;
	mat3 TBN = mat3(normalize( vTangent ), normalize( vBiTangent ), normalize( vNormal ));
	vec3 N = texture(normal, vTexCoord).xyz * 2 - 1;
	float d = max(0, dot(normalize(vNormal.xyz), lightDir));
	
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias)
	{
		d = 0;
	}
	FragColour = vec4(d+a, d+a, d+a, 1)*texture(diffuse, vTexCoord);
}




