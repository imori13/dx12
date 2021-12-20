#pragma once
#include "Matrix4x4.h"

class Camera
{
public:
	Camera()noexcept
		: m_Fov(0)
		, m_NearZ(0)
		, m_FarZ(0)
	{
	}
	void Create(float fovDeg, float nearZ, float farZ);
	void LookAt(const Vector3& target);
	void Update();

	const Matrix4x4& GetViewMatrix() const noexcept { return m_ViewMatrix; }
	const Matrix4x4& GetProjMatrix() const noexcept { return m_ProjMatrix; }

	Vector3 Position;
	Vector3 Rotation;
private:
	Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_ProjMatrix;
	float m_Fov;
	float m_NearZ;
	float m_FarZ;
};

