#include "Camera.h"
#include "Display.h"
#include "Input.h"
#include "MyMath.h"
#include "Timer.h"

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
	{ m_DestPosition.z() += moveSpeed; }
	else if(Input::IsKey(KeyCode::S))
	{ m_DestPosition.z() -= moveSpeed; }

	if(Input::IsKey(KeyCode::D))
	{ m_DestPosition.x() += moveSpeed; }
	else if(Input::IsKey(KeyCode::A))
	{ m_DestPosition.x() -= moveSpeed; }

	if(Input::IsKey(KeyCode::R))
	{ m_DestPosition.y() += moveSpeed; }
	else if(Input::IsKey(KeyCode::F))
	{ m_DestPosition.y() -= moveSpeed; }

	const Vector2 mousePos = Input::MousePos();
	MouseRotate(mousePos);
	MouseMove(mousePos);

	// ï‚äÆ
	const float rate = 0.1f * Timer::g_FrameTime * 60.0f;
	m_Position = Vector3::Lerp(m_Position, m_DestPosition, rate);
	m_Longitude = Math::Lerp(m_Longitude, m_DestLongitude, rate);
	m_Latitude = Math::Lerp(m_Latitude, m_DestLatitude, rate);

	// à‹ìx/åoìxÇãÅÇﬂÇÈ
	m_Rotation.x() = cos(m_Longitude) * cos(m_Latitude);
	m_Rotation.y() = sin(m_Latitude);
	m_Rotation.z() = sin(m_Longitude) * cos(m_Latitude);

	m_ViewMatrix = Matrix4x4::LookAt(m_Position, m_Position + m_Rotation, Vector3::Up());
	m_ProjMatrix = Matrix4x4::PerspectiveProjection(m_Fov, Display::g_Aspect, m_NearZ, m_FarZ);


	m_PrevMousePos = Input::MousePos();
}

void Camera::MouseRotate(const Vector2& mousePos)
{
	if(Input::IsLeft())
	{
		if(mousePos != m_PrevMousePos && m_PrevMousePos != Vector2::Zero())
		{
			constexpr float speed = 2.0f;
			constexpr float speedX = 1.0f;
			constexpr float speedY = 1.5f;

			const auto mouseDir = (mousePos - m_PrevMousePos) * speed;
			constexpr float toRad = 3.141592f * 180.0f;
			m_DestLongitude += -mouseDir.x() / toRad * speedX;
			m_DestLatitude += -mouseDir.y() / toRad * speedY;

			m_DestLatitude = std::clamp(m_DestLatitude, -1.5f, 1.5f);
		}
	}
}

void Camera::MouseMove(const Vector2& mousePos)
{
	if(Input::IsRight())
	{
		constexpr float speedXY = 0.05f;
		if(mousePos != m_PrevMousePos && m_PrevMousePos != Vector2::Zero())
		{
			const auto mouseDir = (mousePos - m_PrevMousePos) * speedXY;
			m_DestPosition.x() += mouseDir.x();
			m_DestPosition.z() += -mouseDir.y();
		}
	}

	const auto wheelval = Input::GetWheelValue();
	if(wheelval != 0)
	{
		constexpr float speedY = 1.0f;
		m_DestPosition.y() += wheelval * speedY;
	}
}
