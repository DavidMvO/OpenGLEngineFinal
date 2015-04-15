#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include <string>
#include "gl_core_4_4.h"
#include "glm\glm.hpp"

struct Vertex
{
	glm::vec4 position;
	glm::vec4 colour;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

struct Vertex2
{
	glm::vec3 position;
	glm::vec3 colour;
	glm::vec3 normal;
};

struct VertexNormal
{
	float x, y, z, w;
	float nx, ny, nz, nw;
	float tx, ty, tz, tw;
	float s, t;
};

struct ParticleVertex
{
	glm::vec4 position;
	glm::vec4 colour;
};


#endif