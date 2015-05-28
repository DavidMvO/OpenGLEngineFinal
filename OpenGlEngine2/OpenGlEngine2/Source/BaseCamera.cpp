#include "BaseCamera.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>

BaseCamera::BaseCamera()
: BaseCamera(glm::mat4(1))
{

}

BaseCamera::BaseCamera(glm::vec3 position)
: BaseCamera()
{
	m_worldTransform[3] = glm::vec4(position, 1);
	m_bPerspectiveSet = false;
}

BaseCamera::BaseCamera(glm::mat4 transform)
: m_worldTransform(transform)
{
	UpdateProjectionViewTransform();
}

void BaseCamera::SetTransform(glm::mat4 transform)
{
	m_worldTransform = transform;
	UpdateProjectionViewTransform();
}

const glm::mat4 BaseCamera::GetTransform() const
{
	return m_worldTransform;
}

void BaseCamera::SetPosition(glm::vec3 position)
{
	m_worldTransform[3] = glm::vec4(position, 1);
	UpdateProjectionViewTransform();
}


void BaseCamera::LookAt(glm::vec3 lookAt, glm::vec3 worldUp)
{
	glm::vec4 vLocation = m_worldTransform[3];
	m_worldTransform = glm::inverse(glm::lookAt(vLocation.xyz(), lookAt, worldUp));
	UpdateProjectionViewTransform();
}

void BaseCamera::LookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 worldUp)
{
	m_worldTransform = glm::inverse(glm::lookAt(position, lookAt, worldUp));
	UpdateProjectionViewTransform();
}

void BaseCamera::SetupPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
	m_bPerspectiveSet = true;

	UpdateProjectionViewTransform();
}

void BaseCamera::UpdateProjectionViewTransform()
{
	m_viewTransform = glm::inverse(m_worldTransform);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

glm::vec3 BaseCamera::PickAgainstPlane(float x, float y, const glm::vec4& plane) const {

	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= m_projectionTransform[0][0];
	screenPos.y /= m_projectionTransform[1][1];

	glm::vec3 dir = glm::normalize(m_worldTransform * glm::vec4(screenPos, 0)).xyz();

	float d = (plane.w - glm::dot(m_worldTransform[3].xyz(), plane.xyz()) / glm::dot(dir, plane.xyz()));

	return m_worldTransform[3].xyz() + dir * d;
}

