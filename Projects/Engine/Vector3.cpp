#include "Vector3.h"

const float& Vector3::x() const
{
	return vec.x();
}

float& Vector3::x()
{
	return vec.x();
}

const float& Vector3::y() const
{
	return vec.y();
}

float& Vector3::y()
{
	return vec.y();
}

const float& Vector3::z() const
{
	return vec.z();
}

float& Vector3::z()
{
	return vec.z();
}

Vector3 Vector3::operator-() const
{
	return -vec;
}
Vector3 Vector3::operator+(const Vector3& vector) const
{
	return vec + vector.vec;
}
Vector3 Vector3::operator-(const Vector3& vector) const
{
	return vec - vector.vec;
}
Vector3 Vector3::operator*(const float scaler) const
{
	return vec * scaler;
}
Vector3 Vector3::operator/(const float scaler) const
{
	return vec / scaler;
}

Vector3& Vector3::operator+=(const Vector3& vector)
{
	vec += vector.vec;
	return *this;
}
Vector3& Vector3::operator-=(const Vector3& vector)
{
	vec -= vector.vec;
	return *this;
}
Vector3& Vector3::operator*=(const float scaler)
{
	vec *= scaler;
	return *this;
}
Vector3& Vector3::operator/=(const float scaler)
{
	vec /= scaler;
	return *this;
}

bool Vector3::operator==(const Vector3& vector) const
{
	return vec == vector.vec;
}
bool Vector3::operator!=(const Vector3& vector) const
{
	return vec != vector.vec;
}

void Vector3::Normalize()
{
	vec.normalize();
}

Vector3 Vector3::normalized() const
{
	return vec.normalized();
}

float Vector3::length() const
{
	return std::sqrtf(vec.dot(vec));
}

float Vector3::lengthSquared() const
{
	return vec.dot(vec);
}

float Vector3::dot(const Vector3& vector) const
{
	return vec.dot(vector.vec);
}

Vector3 Vector3::cross(const Vector3& vector) const
{
	return vec.cross(vector.vec);
}

Vector3 Vector3::lerp(const Vector3& b, float t) const
{
	return Vector3(Math::Lerp(vec.x(), b.x(), t),
				   Math::Lerp(vec.y(), b.y(), t),
				   Math::Lerp(vec.z(), b.z(), t));
}

const float* Vector3::data() const
{
	return vec.data();
}

float* Vector3::data()
{
	return vec.data();
}

Vector3 Vector3::zero() { return Vector3(0); }
Vector3 Vector3::one() { return Vector3(1); }
Vector3 Vector3::up() { return Vector3(0, 1, 0); }
Vector3 Vector3::down() { return Vector3(0, -1, 0); }
Vector3 Vector3::right() { return Vector3(1, 0, 0); }
Vector3 Vector3::left() { return Vector3(-1, 0, 0); }
Vector3 Vector3::foward() { return Vector3(0, 0, 1); }
Vector3 Vector3::backward() { return Vector3(0, 0, -1); }