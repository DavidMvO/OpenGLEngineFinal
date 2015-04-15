#ifndef PROCEDURAL_GEN_EXAMPLE
#define PROCEDURAL_GEN_EXAMPLE

#include "Application.h"
#include "GUIBar.h"
#include "Vertex.h"
#include "stdlib.h"
#include <stb_image.h>

class ProceduralGeneration : public Application
{
public:
	ProceduralGeneration() : Application("ProceduralGeneration", 1920, 1200) {}
	~ProceduralGeneration() {}

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update(double dt);
	virtual void Render();

	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateOpenGLBuffers();
	void CreateShaders();
	void GeneratePerlinValues();
	void DiamondSquare(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int range);
	void GenerateDiamondSquareNoise(float rows, float cols);

	float FindHeight(unsigned int row, unsigned int column);

private:

	GUIBar* m_gui;
	Vertex* aoVertices;

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	int gridArray[257][257];

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int m_rows;
	unsigned int m_cols;

	unsigned int m_indexCount;

	unsigned int m_program;
	unsigned int m_perlin_texture;
	unsigned int m_diamond_texture;

	unsigned char* data;

	unsigned int m_grass_texture;
	unsigned int m_water_texture;
	unsigned int m_rocks_texture;

};

#endif