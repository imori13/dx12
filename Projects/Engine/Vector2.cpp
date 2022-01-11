#include "Vector2.h"

const float& Vector2::x() const
{
	return vec.x();
}

float& Vector2::x()
{
	return vec.x();
}

const float& Vector2::y() const
{
	return vec.y();
}

float& Vector2::y()
{
	return vec.y();
}

Vector2 Vector2::operator-() const
{
	return -vec;
}
Vector2 Vector2::operator+(const Vector2& vector) const
{
	return vec + vector.vec;
}
Vector2 Vector2::operator-(const Vector2& vector) const
{
	return vec - vector.vec;
}
Vector2 Vector2::operator*(const float scaler) const
{
	return vec * scaler;
}
Vector2 Vector2::operator/(const float scaler) const
{
	return vec / scaler;
}

Vector2& Vector2::operator+=(const Vector2& vector)
{
	vec += vector.vec;
	return *this;
}
Vector2& Vector2::operator-=(const Vector2& vector)
{
	vec -= vector.vec;
	return *this;
}
Vector2& Vector2::operator*=(const float scaler)
{
	vec *= scaler;
	return *this;
}
Vector2& Vector2::operator/=(const float scaler)
{
	vec /= scaler;
	return *this;
}

bool Vector2::operator==(const Vector2& vector) const
{
	return vec == vector.vec;
}
bool Vector2::operator!=(const Vector2& vector) const
{
	return vec != vector.vec;
}

void Vector2::Normalize()
{
	vec.normalize();
}

Vector2 Vector2::normalized() const
{
	return vec.normalized();
}

float Vector2::length() const
{
	return std::sqrtf(vec.dot(vec));
}

float Vector2::lengthSquared() const
{
	return vec.dot(vec);
}

float Vector2::dot(const Vector2& vector) const
{
	return vec.dot(vector.vec);
}

const float* Vector2::data() const
{
	return vec.data();
}

float* Vector2::data()
{
	return vec.data();
}

Vector2 Vector2::zero() { return Vector2(0); }
Vector2 Vector2::one() { return Vector2(1); }
Vector2 Vector2::up() { return Vector2(0, 1); }
Vector2 Vector2::down() { return Vector2(0, -1); }
Vector2 Vector2::right() { return Vector2(1, 0); }
Vector2 Vector2::left() { return Vector2(-1, 0); }