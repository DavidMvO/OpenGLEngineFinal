#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include "Application.h"
#include "FBXFile.h"

class DeferredRendering : public Application
{
public:
	DeferredRendering() : Application("DeferredRendering", 1920, 1200) {}
	~DeferredRendering() {}

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update(double dt);
	virtual void Render();

	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanUpOpenGLBuffers(FBXFile* fbx);

	void CreateDeferredBuffers();

	void CreateShaders();
	void CreateGBufferShaders();
	void CreateCompositeShader();
	void CreateLightShader();

	void DrawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse);

private:

	FBXFile* m_fbx;
	unsigned int m_programID;
	unsigned int m_gpassShader;
	unsigned int m_directionLightShader;
	unsigned int m_compositeShader;

	glm::vec3 m_lightDirection;

	unsigned int m_gpassFBO;
	unsigned int m_albedoTexture;
	unsigned int m_positionTexture;
	unsigned int m_normalTexture;
	unsigned int m_gpassDepth;

	unsigned int m_lightFBO;
	unsigned int m_lightTexture;

	unsigned int status;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int m_fbo;
	unsigned int m_fboDepth;
	unsigned int m_fboTexture;

	unsigned int m_quadVAO;
};	
	
#endif