#pragma once
#include "Vector3.h"

#include <Eigen/Eigen>

class Matrix4x4
{
public:
	Matrix4x4() noexcept
	{

	}

	Matrix4x4& operator+=(const Matrix4x4& m);
	Matrix4x4 operator+(const Matrix4x4& m) const;
	Matrix4x4& operator-=(const Matrix4x4& m);
	Matrix4x4 operator-(const Matrix4x4& m) const;
	Matrix4x4& operator*=(const Matrix4x4& m);
	Matrix4x4 operator*(const Matrix4x4& m) const;
	Matrix4x4 operator*(float s) const;
	Vector3 operator*(const Vector3& vec) const;

	DirectX::XMFLOAT4X4 Data() const;
	Matrix4x4 Transpose() const;
	Matrix4x4 Inverse() const;

	static Matrix4x4 Identity();
	// モデル行列
	static Matrix4x4 Scale(const Vector3& vec);
	static Matrix4x4 Scale(float x, float y, float z);
	static Matrix4x4 Translate(const Vector3& vec);
	static Matrix4x4 Translate(float x, float y, float z);
	static Matrix4x4 RotateRollPitchYaw(const Vector3& vec);
	static Matrix4x4 RotateRollPitchYaw(float x, float y, float z);
	static Matrix4x4 RotateX(float radian);
	static Matrix4x4 RotateY(float radian);
	static Matrix4x4 RotateZ(float radian);
	static Matrix4x4 RotateAxis(const Vector3& vector, float radian);
	// ビュー行列
	static Matrix4x4 LookAt(const Vector3& cameraPos, const Vector3& targetPos, const Vector3& upward);
	// プロジェクション行列
	static Matrix4x4 ParrallelProjection(float top, float bottom, float right, float left, float Near, float Far);
	static Matrix4x4 PerspectiveProjection(float fovRad, float aspect, float Near, float Far);
private:
	Matrix4x4(const Eigen::Matrix4f& m)
	{
		matrix = m;
	}

	Eigen::Matrix4f matrix;
};

inline Matrix4x4 operator*(const float s, const Matrix4x4& m)
{
	return m * s;
}