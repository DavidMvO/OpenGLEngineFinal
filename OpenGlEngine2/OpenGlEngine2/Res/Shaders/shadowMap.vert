#version 410

in vec4 Position;
uniform mat4 LightMatrix;
void main()
{
	gl_Position = LightMatrix * Position;
}