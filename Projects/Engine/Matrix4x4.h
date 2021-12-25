#pragma once
#include "Vector3.h"

#include <Eigen/Core>

#define EIGEN_DEFAULT_TO_ROW_MAJOR

class Matrix4x4
{
public:
	Matrix4x4() noexcept
	{

	}

	Matrix4x4& operator+=(const Matrix4x4& m);
	Matrix4x4& operator-=(const Matrix4x4& m);
	Matrix4x4& operator*=(const Matrix4x4& m);

	Matrix4x4 operator+(const Matrix4x4& m) const;
	Matrix4x4 operator-(const Matrix4x4& m) const;
	Matrix4x4 operator*(const Matrix4x4& m) const;
	Matrix4x4 operator*(float s) const;
	Vector3 operator*(const Vector3& vec) const;

	DirectX::XMFLOAT4X4 data() const;
	Matrix4x4 transpose() const;
	Matrix4x4 inverse() const;

	static Matrix4x4 identity();

	float& at(uint8_t row, uint8_t col);
	const float& at(uint8_t row, uint8_t col) const;

	// モデル行列
	Matrix4x4 scale(const Vector3& vec) const;
	Matrix4x4 scale(float x, float y, float z) const;
	Matrix4x4 translation(const Vector3& vec) const;
	Matrix4x4 translation(float x, float y, float z) const;
	Matrix4x4 rotateRollPitchYaw(const Vector3& vec) const;
	Matrix4x4 rotateRollPitchYaw(float x, float y, float z) const;
	Matrix4x4 rotateX(float radian) const;
	Matrix4x4 rotateY(float radian) const;
	Matrix4x4 rotateZ(float radian) const;
	Matrix4x4 rotateAxis(const Vector3& vector, float radian) const;
	// ビュー行列
	Matrix4x4 lookAt(const Vector3& cameraPos, const Vector3& targetPos, const Vector3& upward) const;
	// プロジェクション行列
	Matrix4x4 parrallelProjection(float top, float bottom, float right, float left, float Near, float Far) const;
	Matrix4x4 perspectiveProjection(float fovRad, float aspect, float Near, float Far) const;
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