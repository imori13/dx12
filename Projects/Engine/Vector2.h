#pragma once
#include <Eigen/Eigen>

class Vector2
{
public:
	Vector2::Vector2() noexcept(false)
		: vec(0, 0)
	{
	}

	Vector2::Vector2(float value) noexcept(false)
		: vec(value, value)
	{
	}

	Vector2::Vector2(float value_x, float value_y) noexcept(false)
		: vec(value_x, value_y)
	{

	}

	Vector2 operator-() const;
	Vector2 operator+(const Vector2& vector) const;
	Vector2& operator+=(const Vector2& vector);
	Vector2 operator-(const Vector2& vector) const;
	Vector2& operator-=(const Vector2& vector);
	Vector2 operator*(const float scaler) const;
	Vector2& operator*=(const float scaler);
	Vector2 operator/(const float scaler) const;
	Vector2& operator/=(const float scaler);
	bool operator==(const Vector2& vector) const;
	bool operator!=(const Vector2& vector) const;

	void Normalize();

	const float& x() const;
	float& x();
	const float& y() const;
	float& y();
	const float* data() const;
	float* data();

	Vector2 Normalized() const;
	float Length() const;
	float LengthSquared() const;
	float Dot(const Vector2& vector) const;

	static Vector2 Zero();
	static Vector2 One();
	static Vector2 Up();
	static Vector2 Down();
	static Vector2 Right();
	static Vector2 Left();

private:
	Vector2::Vector2(const Eigen::Vector2f& vector)
		: vec(0, 0)
	{
		this->vec = vector;
	}

	Eigen::Vector2f vec;
};

