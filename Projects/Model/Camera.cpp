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
	const Vector2 mouseVelocity = Input::MouseVelocity();
	MouseRotate(mouseVelocity);
	MouseMove(mouseVelocity);

	// 補完
	const float rate = 0.1f * Timer::g_FrameTime * 60.0f;
	m_Position = m_Position.lerp(m_DestPosition, rate);
	m_Longitude = Math::Lerp(m_Longitude, m_DestLongitude, rate);
	m_Latitude = Math::Lerp(m_Latitude, m_DestLatitude, rate);

	// 緯度/経度を求める
	m_Rotate.x() = cos(m_Longitude) * cos(m_Latitude);
	m_Rotate.y() = sin(m_Latitude);
	m_Rotate.z() = sin(m_Longitude) * cos(m_Latitude);

	m_ViewMatrix = Matrix4x4::identity().lookAt(m_Position, m_Position + m_Rotate, Vector3::up());
	m_ProjMatrix = Matrix4x4::identity().perspectiveProjection(m_Fov, Display::g_Aspect, m_NearZ, m_FarZ);
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
	constexpr float rightRad = 90.0f / 3.141592f * 180.0f;
	const auto foward = Vector3(cos(m_Longitude), 0, sin(m_Longitude));
	const auto right = Vector3(cos(m_Longitude + rightRad), 0, sin(m_Longitude + rightRad));

	if(Input::IsMouseHold(Mouse::Right))
	{
		constexpr float speedXY = 0.05f;
		// 前後
		if(mouseVelocity.y() != 0)
		{
			m_DestPosition += foward * -mouseVelocity.y() * speedXY;
		}
		// 左右
		if(mouseVelocity.x() != 0)
		{
			m_DestPosition += right * mouseVelocity.x() * speedXY;
		}
	}

	const auto wheelval = Input::MouseWheelVelocity();
	if(wheelval != 0)
	{
		constexpr float speedY = 1.0f;
		m_DestPosition.y() += wheelval * speedY;
	}


	constexpr float moveSpeed = 0.1f;
	if(Input::IsKeyHold(Keys::W))
	{ m_DestPosition += foward * moveSpeed; }
	else if(Input::IsKeyHold(Keys::S))
	{ m_DestPosition += -foward * moveSpeed; }

	if(Input::IsKeyHold(Keys::D))
	{ m_DestPosition += right * moveSpeed; }
	else if(Input::IsKeyHold(Keys::A))
	{ m_DestPosition += -right * moveSpeed; }

	if(Input::IsKeyHold(Keys::R))
	{ m_DestPosition.y() += moveSpeed; }
	else if(Input::IsKeyHold(Keys::F))
	{ m_DestPosition.y() -= moveSpeed; }
}
