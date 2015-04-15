#ifndef PARTICLE_EFFECT_H
#define PARTICLE_EFFECT_H

#include "Application.h"
#include "GPUParticleEmitter.h"
#include "GUIBar.h"

class ParticleEffect : public Application
{
public:
	ParticleEffect() : Application("ParticleEffect", 1920, 1200) {}
	~ParticleEffect() {}

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update(double dt);
	virtual void Render();

	GPUParticleEmitter* m_GPUEmitter;
	GUIBar* m_gui;

protected:


private:

};

#endif