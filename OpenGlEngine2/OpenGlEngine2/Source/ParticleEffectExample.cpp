#include "ParticleEffectExample.h"

#include "FlyCamera.h"

bool ParticleEffect::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_gui = new GUIBar(m_window);


	//GPU PARTICLE EMITTER
	m_GPUEmitter = new GPUParticleEmitter(m_gui);
	m_GPUEmitter->Initialise(100000, 0.1f, 5.0f, 5, 20, 0.001f, 0.01f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));

	return true;
}

void ParticleEffect::ShutDown()
{

}

bool ParticleEffect::Update(double dt)
{
	m_camera->Update(dt);

	return true;
}

void ParticleEffect::Render()
{
	m_gui->Draw();
	DisplayGrid(20);

	//DRAW GPU PARTICLE EMITTER
	m_GPUEmitter->Draw((float)glfwGetTime(),
		m_camera->GetTransform(),
		m_camera->GetProjectionView());
}