#pragma once
#include "Matrix4x4.h"
#include "Vector2.h"

class Camera
{
public:
	Camera()noexcept
		: m_Fov(0)
		, m_NearZ(0)
		, m_FarZ(0)
	{
	}
	void Create(float fovDeg, float nearZ, float farZ) noexcept;
	void Update();

	const Vector3& GetPosition() const noexcept { return m_Position; }
	const Vector3& GetRotate() const noexcept { return m_Rotate; }
	const Matrix4x4& GetViewMatrix() const noexcept { return m_ViewMatrix; }
	const Matrix4x4& GetProjMatrix() const noexcept { return m_ProjMatrix; }
private:
	void MouseRotate(const Vector2& mousePos);
	void MouseMove(const Vector2& mousePos);

	Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_ProjMatrix;
	float m_Fov;
	float m_NearZ;
	float m_FarZ;

	Vector3 m_DestPosition;
	Vector3 m_Position;
	Vector3 m_Rotate;

	float m_Longitude = 1.5f;
	float m_Latitude = 0;
	float m_DestLongitude = 1.5f;
	float m_DestLatitude = 0;
};

