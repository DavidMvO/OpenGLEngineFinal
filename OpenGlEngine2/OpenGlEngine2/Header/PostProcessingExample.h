#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include "Application.h"

class PostProcessing : public Application
{
public:
	PostProcessing() : Application("PostProcessing", 1920, 1080) {}
	~PostProcessing() {}

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update(double dt);
	virtual void Render();

	void CreateOpenGLBuffers();
	void CleanUpOpenGLBuffers();

	void CreateShaders();

private:

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

	unsigned int m_vao;
	unsigned int m_vbo;

	unsigned int m_programID;
};

#endif