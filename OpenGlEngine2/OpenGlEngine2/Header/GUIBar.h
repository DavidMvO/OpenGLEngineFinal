#ifndef GUIBAR_H
#define GUIBAR_H

#include "AntTweakBar.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class GUIBar
{
public:
	GUIBar(GLFWwindow* pWindow);
	~GUIBar();
	void Draw();

	static void OnMouseButton(GLFWwindow*, int b, int a, int m) {TwEventMouseButtonGLFW(b, a); }
	static void OnMousePosition(GLFWwindow*, double x, double y) {TwEventMousePosGLFW((int)x, (int)y);}
	static void OnMouseScroll(GLFWwindow*, double x, double y) {TwEventMouseWheelGLFW((int)y);}
	static void OnKey(GLFWwindow*, int k, int s, int a, int m) {TwEventKeyGLFW(k, a);}
	static void OnChar(GLFWwindow*, unsigned int c) {TwEventCharGLFW(c, GLFW_PRESS);}
	static void OnWindowResize(GLFWwindow*, int w, int h) {TwWindowSize(w, h);glViewport(0, 0, w, h);}

	glm::vec4 m_clearColour;
	glm::vec3 m_lightDir;
	glm::vec3 m_lightColour;

	float m_fps;
	float m_specular;

	float m_maxParticles;
	float m_emitRate;

	float m_lifeSpanMin;
	float m_lifeSpanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColour;
	glm::vec4 m_endColour;

	int m_octaves;
	int  m_scaleMultiplier;
	float m_amplitude;
	float m_persistence;



private:
	TwBar* m_bar;
	GLFWwindow* m_window;
};

#endif