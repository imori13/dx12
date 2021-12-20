#include "Camera.h"
#include "Display.h"
#include "Input.h"
#include "MyMath.h"

void Camera::Create(float fovDeg, float nearZ, float farZ) noexcept
{
	m_Fov = fovDeg * 3.141592f / 180.0f;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::Update()
{
	constexpr float moveSpeed = 0.1f;
	if(Input::IsKey(KeyCode::W))
	{ Position.z() += moveSpeed; }
	else if(Input::IsKey(KeyCode::S))
	{ Position.z() -= moveSpeed; }

	if(Input::IsKey(KeyCode::D))
	{ Position.x() += moveSpeed; }
	else if(Input::IsKey(KeyCode::A))
	{ Position.x() -= moveSpeed; }

	if(Input::IsKey(KeyCode::R))
	{ Position.y() += moveSpeed; }
	else if(Input::IsKey(KeyCode::F))
	{ Position.y() -= moveSpeed; }

	// 画面ドラッグで移動量をとる
	const Vector2 mousePos = Input::MousePos();

	if(Input::IsLeft())
	{
		if(mousePos != prevMousePos && prevMousePos != Vector2::Zero())
		{
			constexpr float speed = 1.0f;
			constexpr float Xspeed = 1.0f;
			constexpr float Yspeed = 1.5f;

			const auto mouseDir = (mousePos - prevMousePos) * speed;
			constexpr float toRad = 3.141592f * 180.0f;
			m_DestLongitude += mouseDir.x() / toRad * Xspeed;
			m_DestLatitude += mouseDir.y() / toRad * Yspeed;

			m_DestLatitude = std::clamp(m_DestLatitude, -1.5f, 1.5f);
		}
	}
	prevMousePos = mousePos;

	// 補完
	m_Longitude = Math::Lerp(m_Longitude, m_DestLongitude, 0.05f);
	m_Latitude = Math::Lerp(m_Latitude, m_DestLatitude, 0.05f);

	// 緯度/経度を求める
	Rotation.x() = cos(m_Longitude) * cos(m_Latitude);
	Rotation.y() = sin(m_Latitude);
	Rotation.z() = sin(m_Longitude) * cos(m_Latitude);

	m_ViewMatrix = Matrix4x4::LookAt(Position, Position + Rotation, Vector3::Up());
	m_ProjMatrix = Matrix4x4::PerspectiveProjection(m_Fov, Display::g_Aspect, m_NearZ, m_FarZ);
}
