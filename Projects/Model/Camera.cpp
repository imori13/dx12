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
	if(Input::IsKeyHold(Key::W))
	{ m_DestPosition.z() += moveSpeed; }
	else if(Input::IsKeyHold(Key::S))
	{ m_DestPosition.z() -= moveSpeed; }

	if(Input::IsKeyHold(Key::D))
	{ m_DestPosition.x() += moveSpeed; }
	else if(Input::IsKeyHold(Key::A))
	{ m_DestPosition.x() -= moveSpeed; }

	if(Input::IsKeyHold(Key::R))
	{ m_DestPosition.y() += moveSpeed; }
	else if(Input::IsKeyHold(Key::F))
	{ m_DestPosition.y() -= moveSpeed; }

	const Vector2 mouseVelocity = Input::MouseVelocity();
	MouseRotate(mouseVelocity);
	MouseMove(mouseVelocity);

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
}

void Camera::MouseRotate(const Vector2& mouseVelocity)
{
	if(Input::IsMouseHold(Mouse::Left))
	{
		constexpr float speed = 2.0f;
		constexpr float speedX = 1.0f;
		constexpr float speedY = 1.5f;

		constexpr float toRad = 3.141592f * 180.0f;
		m_DestLongitude += -mouseVelocity.x() / toRad * speed * speedX;
		m_DestLatitude += -mouseVelocity.y() / toRad * speed * speedY;

		m_DestLatitude = std::clamp(m_DestLatitude, -1.5f, 1.5f);
	}
}

void Camera::MouseMove(const Vector2& mouseVelocity)
{
	if(Input::IsMouseHold(Mouse::Right))
	{
		constexpr float speedXY = 0.05f;
		if(mouseVelocity != Vector2::Zero())
		{
			m_DestPosition.x() += mouseVelocity.x() * speedXY;
			m_DestPosition.z() += -mouseVelocity.y() * speedXY;
		}
	}

	const auto wheelval = Input::MouseWheelVelocity();
	if(wheelval != 0)
	{
		constexpr float speedY = 1.0f;
		m_DestPosition.y() += wheelval * speedY;
	}
}
