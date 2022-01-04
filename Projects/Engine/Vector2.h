#pragma once

class Vector2
{
public:
	Vector2() noexcept(false)
		: vec(0, 0)
	{
	}

	Vector2(float value) noexcept(false)
		: vec(value, value)
	{
	}

	Vector2(float value_x, float value_y) noexcept(false)
		: vec(value_x, value_y)
	{

	}

	Vector2 operator-() const;
	Vector2 operator+(const Vector2& vector) const;
	Vector2 operator-(const Vector2& vector) const;
	Vector2 operator*(const float scaler) const;
	Vector2 operator/(const float scaler) const;
	Vector2& operator+=(const Vector2& vector);
	Vector2& operator-=(const Vector2& vector);
	Vector2& operator*=(const float scaler);
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

	Vector2 normalized() const;
	float length() const;
	float lengthSquared() const;
	float dot(const Vector2& vector) const;

	static Vector2 zero();
	static Vector2 one();
	static Vector2 up();
	static Vector2 down();
	static Vector2 right();
	static Vector2 left();

private:
	Vector2(const Eigen::Vector2f& vector)
		: vec(0, 0)
	{
		this->vec = vector;
	}

	Eigen::Vector2f vec;
};

