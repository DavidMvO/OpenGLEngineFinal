#ifndef SHADOW_MAPPING_H
#define SHADOW_MAPPING_H

#include "Application.h"
#include "GUIBar.h"
#include "FBXFile.h"

class ShadowMapping : public Application
{
public:
	ShadowMapping() : Application("ShadowMapping", 1920, 1200) {}
	~ShadowMapping() {}

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update(double dt);
	virtual void Render();

	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanUpOpenGlBuffers(FBXFile* fbx);

	void CreateShaders();
	void CreateShadowShaders();

	void CreateShadowFrameBuffer();

private:

	GUIBar* m_gui;
	FBXFile* m_fbx;

	unsigned int m_shadowGenProgram;
	unsigned int m_useShadowProgram;

	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

	glm::vec3 m_lightDirection;
	glm::mat4 m_lightMatrix;

};

#endif