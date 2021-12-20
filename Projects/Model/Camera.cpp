#include "Camera.h"
#include "Display.h"

void Camera::Create(float fovDeg, float nearZ, float farZ)
{
	m_Fov = fovDeg * 3.141592f / 180.0f;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::LookAt(const Vector3& target)
{
	m_ViewMatrix = Matrix4x4::LookAt(Position, target, Vector3::Up());
	m_ProjMatrix = Matrix4x4::PerspectiveProjection(m_Fov, Display::g_Aspect, m_NearZ, m_FarZ);
}
