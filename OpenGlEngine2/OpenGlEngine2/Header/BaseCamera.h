#ifndef _BASE_CAMERA_H_
#define _BASE_CAMERA_H_

#include <glm/glm.hpp>

class BaseCamera
{
public:
	BaseCamera();
	BaseCamera(glm::vec3 position);
	BaseCamera(glm::mat4 transform);

	virtual void Update(double dt) = 0;

	void SetTransform(glm::mat4 transform);
	const glm::mat4 GetTransform() const;

	void SetPosition(glm::vec3 position);
	glm::vec3 GetPosition() const { return m_worldTransform[3].xyz(); }


	void LookAt(glm::vec3 lookAt, glm::vec3 up);
	void LookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);

	void SetupPerspective(float fieldOfView, float aspectRatio, float near = 0.1f, float far = 1000.0f);

	const glm::mat4& GetProjection() const { return m_projectionTransform; }
	const glm::mat4& GetView() const { return m_viewTransform; }
	const glm::mat4& GetProjectionView() const { return m_projectionViewTransform; }

	bool GetPerspectiveSet() const { return m_bPerspectiveSet; }

	// returns the point of intersection of a camera ray and a world-space plane
	// the plane has a normal of XYZ and is offset from (0,0,0) by -W in the direction of the normal
	glm::vec3  PickAgainstPlane(float x, float y, const glm::vec4& plane) const;

protected:
	void UpdateProjectionViewTransform();


	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;

private:
	glm::mat4 m_worldTransform;

	bool m_bPerspectiveSet;

	float		m_speed;
	glm::vec3	m_up;
	glm::mat4	m_transform;
	glm::mat4	m_projection;
	glm::mat4	m_view;
	glm::mat4	m_projectionView;
};

#endif // !CAMERA_H_
