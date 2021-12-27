#pragma once
#include "MyMath.h"

#include <Eigen/Eigen>

struct Vector3
{
	explicit Vector3() noexcept(false)
		: vec(0, 0, 0)
	{
	}

	explicit Vector3(float value) noexcept(false)
		: vec(value, value, value)
	{
	}

	explicit Vector3(float value_x, float value_y, float value_z) noexcept(false)
		: vec(value_x, value_y, value_z)
	{

	}

	Vector3 operator-() const;
	Vector3 operator+(const Vector3& vector) const;
	Vector3 operator-(const Vector3& vector) const;
	Vector3 operator*(const float scaler) const;
	Vector3 operator/(const float scaler) const;
	Vector3& operator+=(const Vector3& vector);
	Vector3& operator-=(const Vector3& vector);
	Vector3& operator*=(const float scaler);
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

	Vector3 normalized() const;
	float length() const;
	float lengthSquared() const;
	float dot(const Vector3& vector) const;
	Vector3 cross(const Vector3& vector) const;
	Vector3 lerp(const Vector3& b, float t) const;

	static Vector3 zero();
	static Vector3 one();
	static Vector3 up();
	static Vector3 down();
	static Vector3 right();
	static Vector3 left();
	static Vector3 foward();
	static Vector3 backward();
private:
	Vector3(const Eigen::Vector3f& vector)
		: vec(0, 0, 0)
	{
		this->vec = vector;
	}

	Eigen::Vector3f vec;
};