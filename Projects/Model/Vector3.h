#pragma once

#include <Eigen/Eigen>

struct Vector3
{
	Vector3::Vector3() noexcept(false)
		: vec(0, 0, 0)
	{
	}

	Vector3::Vector3(float value) noexcept(false)
		: vec(value, value, value)
	{
	}

	Vector3::Vector3(float value_x, float value_y, float value_z) noexcept(false)
		: vec(value_x, value_y, value_z)
	{
		
	}

	Vector3 operator-() const;
	Vector3 operator+(const Vector3& vector) const;
	Vector3& operator+=(const Vector3& vector);
	Vector3 operator-(const Vector3& vector) const;
	Vector3& operator-=(const Vector3& vector);
	Vector3 operator*(const float scaler) const;
	Vector3& operator*=(const float scaler);
	Vector3 operator/(const float scaler) const;
	Vector3& operator/=(const float scaler);
	bool operator==(const Vector3& vector) const;
	bool operator!=(const Vector3& vector) const;

	// ³‹K‰»
	void Normalize();

	const float& x() const;
	float& x();
	const float& y() const;
	float& y();
	const float& z() const;
	float& z();
	const float* data() const;
	float* data();

	Vector3 Normalized() const;
	float Length() const;
	float LengthSquared() const;
	float Dot(const Vector3& vector) const;
	Vector3 Cross(const Vector3& vector) const;
	DirectX::XMFLOAT3 XMFloat() const;

	static Vector3 Zero();
	static Vector3 One();
	static Vector3 Up();
	static Vector3 Down();
	static Vector3 Right();
	static Vector3 Left();
	static Vector3 Foward();
	static Vector3 Backward();

private:
	Vector3::Vector3(const Eigen::Vector3f& vector)
		: vec(0, 0, 0)
	{
		this->vec = vector;
	}

	Eigen::Vector3f vec;
};