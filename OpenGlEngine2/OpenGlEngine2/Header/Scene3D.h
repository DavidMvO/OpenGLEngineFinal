#ifndef SCENE_3D_H
#define SCENE_3D_H

#include "Application.h"
#include "GUIBar.h"
#include "Vertex.h"
#include "stdlib.h"
#include "FBXFile.h"
#include "GPUParticleEmitter.h"
#include <stb_image.h>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Scene3D : public Application
{
public:

	Scene3D() : Application("Scene3D", 1920, 1080) {}
	~Scene3D() {}

	virtual bool StartUp();
	virtual void ShutDown();

	//----generalfunctions------
	virtual bool Update(double dt);
	virtual void Render();

	unsigned int loadShader(unsigned int type, const char* path);
	void GenerateAllShaders(); \
	void GenerateShaders(unsigned int program, const char* fragment, const char* vertex);
	void GenerateShaders(unsigned int program, const char* fragment, const char* vertex, const char* geom);


	//-----shadowMapFunctions-----
	void GenerateShadowMapShaders();
	void CreateShadowFrameBuffer();
	void RenderShadowGeometry();

	//-----Terrain Generation-----
	void GenerateOpenGLBuffersTerrain();
	void CleanUpOpenGlBuffersTerrain();
	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateTerrainShaders();
	void GeneratePerlinValues();
	void RenderTerrain();

	void GenerateNormal(Vertex* vert1, Vertex* vert2, Vertex* vert3);
	void Regenerate();

	//-----ObjectLoading---------
	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanUpOpenGlBuffers(FBXFile* fbx);
	void GenerateOjectShaders();
	void RenderObject(unsigned int program, FBXFile* fbx);
	void CreateOpenGLBuffersAnimated(FBXFile* fbx);
	void RenderAnimatedObject(unsigned int program, FBXFile* fbx);
	void GenerateAnimatedObjectShaders();

private:

	GUIBar* m_gui;
	GPUParticleEmitter* m_GPUEmitter;

	glm::mat4 lightMatrix;

	//shadow mapping variables
	unsigned int m_shadowGenProgram;

	unsigned int m_shadowFBO;
	unsigned int m_shadowFBODepth;

	glm::vec3 m_lightDirection;
	glm::mat4 m_lightMatrix;

	//terrain variables
	unsigned int m_useTerrainProgram;

	unsigned int m_rows;
	unsigned int m_cols;

	unsigned char* data;
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned int m_grass_texture;
	unsigned int m_water_texture;
	unsigned int m_rocks_texture;

	unsigned m_perlin_texture;

	unsigned int m_terrainVao;
	unsigned int m_terrainVbo;
	unsigned int m_terrainIbo;

	unsigned int m_indexCount;

	float *perlin_data;
	Vertex *aoVertices;
	unsigned int* auiIndices;

	//object variables
	FBXFile* m_tree;
	FBXFile* m_pyro;

	FBXFile* m_dragon;

	unsigned int m_objectVao;
	unsigned int m_objectVbo;
	unsigned int m_objectIbo;

	unsigned int m_object_program;
	unsigned int m_animated_program;

	unsigned int object_texture_normal;
	unsigned int object_texture_diffuse;

	FBXSkeleton* skeleton;
	FBXAnimation* animation;
	float m_timer;
	
}; 

#endif