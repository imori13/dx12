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
Vector3& Vector3::operator+=(const Vector3& vector)
{
	vec += vector.vec;
	return *this;
}
Vector3 Vector3::operator-(const Vector3& vector) const
{
	return vec - vector.vec;
}
Vector3& Vector3::operator-=(const Vector3& vector)
{
	vec -= vector.vec;
	return *this;
}
Vector3 Vector3::operator*(const float scaler) const
{
	return vec * scaler;
}
Vector3& Vector3::operator*=(const float scaler)
{
	vec *= scaler;
	return *this;
}
Vector3 Vector3::operator/(const float scaler) const
{
	return vec / scaler;
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

Vector3 Vector3::Normalized() const
{
	return vec.normalized();
}

float Vector3::Length() const
{
	return std::sqrtf(vec.dot(vec));
}

float Vector3::LengthSquared() const
{
	return vec.dot(vec);
}

float Vector3::Dot(const Vector3& vector) const
{
	return vec.dot(vector.vec);
}

Vector3 Vector3::Cross(const Vector3& vector) const
{
	return vec.cross(vector.vec);
}

DirectX::XMFLOAT3 Vector3::XMFloat() const
{
	return DirectX::XMFLOAT3(vec.data());
}

const float* Vector3::data() const
{
	return vec.data();
}

float* Vector3::data()
{
	return vec.data();
}

Vector3 Vector3::Zero() { return Vector3(0); }
Vector3 Vector3::One() { return Vector3(1); }
Vector3 Vector3::Up() { return Vector3(0, 1, 0); }
Vector3 Vector3::Down() { return Vector3(0, -1, 0); }
Vector3 Vector3::Right() { return Vector3(1, 0, 0); }
Vector3 Vector3::Left() { return Vector3(-1, 0, 0); }
Vector3 Vector3::Foward() { return Vector3(0, 0, 1); }
Vector3 Vector3::Backward() { return Vector3(0, 0, -1); }