#include "Application.h"

#include "Gizmos.h"
#include "BaseCamera.h"
#include <GLFW\glfw3.h>

Application::Application(std::string appName, unsigned int uiWidth, unsigned int uiHeight)
{
	m_appName = appName;
	m_camera = nullptr;

	m_screenWidth = uiWidth;
	m_screenHeight = uiHeight;

	m_totalRunTime = glfwGetTime();
	m_deltaTime = 0.0f;
}

Application::~Application()
{

}

bool Application::InitializeOpenGL()
{
	if (glfwInit() == false)
		return false;

	m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, m_appName.c_str(), nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return false;
	}

	Gizmos::create();

	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST); // enables the depth buffer
	glEnable(GL_FRONT_AND_BACK);

	return true;
}


void Application::Run()
{
	InitializeOpenGL();

	StartUp();

	assert(m_camera != NULL && "Camera not initialized");
	assert(m_camera->GetPerspectiveSet() == true && "Camera Perspective not set");

	bool bRunning = true;
	while (!glfwWindowShouldClose(m_window) && bRunning)
	{
		double currTime = glfwGetTime();
		m_deltaTime = currTime - m_totalRunTime;
		m_totalRunTime = currTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		bRunning = Update(m_deltaTime);

		Render();

		Gizmos::draw(m_camera->GetProjectionView());

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	ShutDown();

	Gizmos::destroy();

	glfwDestroyWindow(m_window);
	glfwTerminate();

}

void Application::DisplayGrid(int uiSize)
{
	assert(uiSize > 0 && "Needs to be a positive size");

	Gizmos::addTransform(glm::mat4(1));
	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	int uiHalfSize = uiSize / 2;
	for (int i = 0; i < uiSize + 1; ++i)
	{
		Gizmos::addLine(glm::vec3(-uiHalfSize + i, 0, uiHalfSize),
			glm::vec3(-uiHalfSize + i, 0, -uiHalfSize),
			i == uiHalfSize ? white : black);

		Gizmos::addLine(glm::vec3(uiHalfSize, 0, -uiHalfSize + i),
			glm::vec3(-uiHalfSize, 0, -uiHalfSize + i),
			i == uiHalfSize ? white : black);
	}
}

void Application::SetUpCamera(FlyCamera* camera)
{
	camera->SetRotationSpeed(1.0f);

	camera->SetInputWindow(m_window);

	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f);
	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	m_camera = camera;
}