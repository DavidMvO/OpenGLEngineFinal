#ifndef APPLICATION_H
#define APPLICATION_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "FlyCamera.h"


class BaseCamera;

class Application
{
public:
	Application(std::string appName, unsigned int uiWidth = 1920, unsigned int uiHeight = 1080);
	~Application();

	virtual bool StartUp() = 0;
	virtual void ShutDown() = 0;

	virtual bool Update(double dt) = 0;
	virtual void Render() = 0;

	void Run();

protected:
	//Helper functions that can be used by children
	void DisplayGrid(int uiSize);
	void SetUpCamera(FlyCamera* camera);

	unsigned int GetScreenWidth() const { return m_screenWidth; }
	unsigned int GetScreenHeight() const { return m_screenHeight; }

	std::string m_appName;
	BaseCamera* m_camera;
	GLFWwindow* m_window;

private:
	bool InitializeOpenGL();

	unsigned int m_screenWidth;
	unsigned int m_screenHeight;

	double m_totalRunTime;
	double m_deltaTime;
};

#endif