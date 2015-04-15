#include "GUIBar.h"

GUIBar::GUIBar(GLFWwindow* pWindow)
{
	m_window = pWindow;
	m_clearColour = glm::vec4(0.3, 0.3, 0.3, 1);
	m_lightColour = glm::vec3(255, 255, 255);
	m_lightDir = glm::vec3(0, -1, -0);
	m_specular = 4;

	m_octaves = 5;
	m_scaleMultiplier = 4;
	m_amplitude = 0.6f;
	m_persistence = 0.5f;

	m_startColour = glm::vec4(1, 0, 0, 1);
	m_endColour = glm::vec4(1, 1, 0, 1);

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1920, 1080);

	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);	m_bar = TwNewBar("Options");	TwAddVarRW(m_bar, "ClearColour", TW_TYPE_COLOR4F, &m_clearColour, "group=Background");	TwAddVarRW(m_bar, "LightDirection", TW_TYPE_DIR3F, &m_lightDir, "group=Light");	TwAddVarRW(m_bar, "LightColour", TW_TYPE_COLOR3F, &m_lightColour, "group=Light");	TwAddVarRW(m_bar, "Specular", TW_TYPE_FLOAT, &m_specular, "group=Light");

	TwAddVarRW(m_bar, "Octaves", TW_TYPE_INT32, &m_octaves, "group=GenerateTerrain");
	TwAddVarRW(m_bar, "ScaleMultiplier", TW_TYPE_INT32, &m_scaleMultiplier, "group=GenerateTerrain");
	TwAddVarRW(m_bar, "Amplitude", TW_TYPE_FLOAT, &m_amplitude, "group=GenerateTerrain");
	TwAddVarRW(m_bar, "Persistence", TW_TYPE_FLOAT, &m_persistence, "group=GenerateTerrain");
	TwAddButton(m_bar, "Press Space to Re-Generate", NULL, NULL, "group=GenerateTerrain");
}

void GUIBar::Draw()
{
	TwDraw();
}

GUIBar::~GUIBar()
{
	TwDeleteAllBars();
	TwTerminate();
}