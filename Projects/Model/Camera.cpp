#include "Camera.h"
#include "Display.h"
#include "Input.h"

void Camera::Create(float fovDeg, float nearZ, float farZ)
{
	m_Fov = fovDeg * 3.141592f / 180.0f;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::Update()
{
	const Vector2 mousePos = Input::MousePos();
	if(Input::IsLeft())
	{
		if(mousePos != prevMousePos && prevMousePos != Vector2::Zero())
		{
			const auto mouseDir = (mousePos - prevMousePos);
			constexpr float toRad = 3.141592f * 180.0f;
			m_Longitude += mouseDir.x() / toRad;
			m_Latitude += mouseDir.y() / toRad;

			LOGLINE(L"x:%.1f y:%.1f", m_Longitude, m_Latitude);

			//Foward = Matrix4x4::RotateRollPitchYaw(temp) * Vector3::One();
		}
	}
	prevMousePos = mousePos;

	Vector3 temp;
	temp.x() = cos(m_Longitude) * cos(m_Latitude);
	temp.y() = sin(m_Latitude);
	temp.z() = sin(m_Longitude) * cos(m_Latitude);

	Rotation = temp;

	m_ViewMatrix = Matrix4x4::LookAt(Position, Position + Rotation, Vector3::Up());
	m_ProjMatrix = Matrix4x4::PerspectiveProjection(m_Fov, Display::g_Aspect, m_NearZ, m_FarZ);
}
