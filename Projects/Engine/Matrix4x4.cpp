#include "Matrix4x4.h"

namespace
{
	const Eigen::Matrix4f IDENTITY = Eigen::Matrix4f::Identity();
}

Matrix4x4 Matrix4x4::identity()
{
	return IDENTITY;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& m)
{
	matrix += m.matrix;
	return *this;
}
Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& m)
{
	matrix -= m.matrix;
	return *this;
}
Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m)
{
	matrix *= m.matrix;
	return *this;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m) const
{
	return matrix + m.matrix;
}
Matrix4x4 Matrix4x4::operator-(const Matrix4x4& m) const
{
	return matrix - m.matrix;
}
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const
{
	return matrix * m.matrix;
}
Matrix4x4 Matrix4x4::operator*(float s) const
{
	return matrix * s;
}

Vector3 Matrix4x4::operator*(const Vector3& vec) const
{
	Eigen::Vector4f vec4 = matrix.transpose() * Eigen::Vector4f(vec.x(), vec.y(), vec.z(), 1);
	return Vector3(vec4.x(), vec4.y(), vec4.z());
}

DirectX::XMFLOAT4X4 Matrix4x4::data() const
{
	return DirectX::XMFLOAT4X4(matrix.data());
}

Matrix4x4 Matrix4x4::transpose() const
{
	return matrix.transpose();
}

Matrix4x4 Matrix4x4::inverse() const
{
	return matrix.inverse();
}

Vector3 Matrix4x4::vector3() const
{
	auto hoge = matrix * Eigen::Vector4f(1, 1, 1, 1);
	return Vector3(hoge.x(), hoge.y(), hoge.z());
}

float& Matrix4x4::at(uint8_t row, uint8_t col)
{
	return matrix(row, col);
}

const float& Matrix4x4::at(uint8_t row, uint8_t col) const
{
	return matrix(row, col);
}

Matrix4x4 Matrix4x4::scale(const Vector3& vec) const
{
	return *this * scale(vec.x(), vec.y(), vec.z());
}

Matrix4x4 Matrix4x4::scale(float x, float y, float z) const
{
	auto m = identity();
	m.at(0, 0) = x;
	m.at(1, 1) = y;
	m.at(2, 2) = z;
	return *this * m;
}

Matrix4x4 Matrix4x4::rotateX(float radian) const
{
	auto m = identity();
	m.at(1, 1) = cos(radian);
	m.at(1, 2) = sin(radian);
	m.at(2, 1) = -sin(radian);
	m.at(2, 2) = cos(radian);

	return *this * m;
}

Matrix4x4 Matrix4x4::rotateY(float radian) const
{
	auto m = identity();
	m.at(0, 0) = cos(radian);
	m.at(0, 2) = -sin(radian);
	m.at(2, 0) = sin(radian);
	m.at(2, 2) = cos(radian);

	return *this * m;
}

Matrix4x4 Matrix4x4::rotateZ(float radian) const
{
	auto m = identity();
	m.at(0, 0) = cos(radian);
	m.at(0, 1) = sin(radian);
	m.at(1, 0) = -sin(radian);
	m.at(1, 1) = cos(radian);

	return *this * m;
}

Matrix4x4 Matrix4x4::rotateAxis(const Vector3& vector, float radian) const
{
	float X = vector.x();
	float Y = vector.y();
	float Z = vector.z();

	float Cos = cos(radian);
	float Sin = sin(radian);
	float A = 1 - Cos;

	auto m = identity();
	m.at(0, 0) = X * X * A + Cos;
	m.at(0, 1) = X * Y * A - Z * Sin;
	m.at(0, 2) = Z * X * A + Y * Sin;

	m.at(1, 0) = X * Y * A + Z * Sin;
	m.at(1, 1) = Y * Y * A + Cos;
	m.at(1, 2) = Y * Z * A - X * Sin;

	m.at(2, 0) = Z * X * A - Y * Sin;
	m.at(2, 1) = Y * Z * A + X * Sin;
	m.at(2, 2) = Z * Z * A + Cos;

	return *this * m;
}

Matrix4x4 Matrix4x4::translation(const Vector3& vec) const
{
	auto m = identity();
	m.at(3, 0) = vec.x();
	m.at(3, 1) = vec.y();
	m.at(3, 2) = vec.z();

	return *this * m;
}

Matrix4x4 Matrix4x4::translation(float x, float y, float z) const
{
	auto m = identity();
	m.at(3, 0) = x;
	m.at(3, 1) = y;
	m.at(3, 2) = z;

	return *this * m;
}

Matrix4x4 Matrix4x4::rotation(const Vector3& vec) const
{
	return rotation(vec.x(), vec.y(), vec.z());
}

Matrix4x4 Matrix4x4::rotation(float x, float y, float z) const
{
	return rotateZ(z) * rotateX(x) * rotateY(y);
}

Matrix4x4 Matrix4x4::lookAt(const Vector3& cameraPos, const Vector3& targetPos, const Vector3& upward) const
{
	Vector3 Z = (targetPos - cameraPos);
	Z = (Z != Vector3::zero()) ? (Z.normalized()) : (Vector3::zero());

	Vector3 X = upward.cross(Z);
	Vector3 Y = Z.cross(X);

	X = (X != Vector3::zero()) ? (X.normalized()) : (Vector3::zero());
	Y = (Y != Vector3::zero()) ? (Y.normalized()) : (Vector3::zero());

	auto m = identity();
	m.at(0, 0) = X.x();
	m.at(1, 0) = X.y();
	m.at(2, 0) = X.z();
	m.at(3, 0) = -X.dot(cameraPos);

	m.at(0, 1) = Y.x();
	m.at(1, 1) = Y.y();
	m.at(2, 1) = Y.z();
	m.at(3, 1) = -Y.dot(cameraPos);

	m.at(0, 2) = Z.x();
	m.at(1, 2) = Z.y();
	m.at(2, 2) = Z.z();
	m.at(3, 2) = -Z.dot(cameraPos);

	return *this * m;
}

Matrix4x4 Matrix4x4::parrallelProjection(float top, float bottom, float right, float left, float Near, float Far) const
{
	auto m = identity();
	m.at(0, 0) = 2.0f / (right - left);
	m.at(1, 1) = 2.0f / (top - bottom);
	m.at(2, 2) = 2.0f / (Far - Near);

	m.at(0, 3) = -((right + left) / (right - left));
	m.at(1, 3) = -((top + bottom) / (top - bottom));
	m.at(2, 3) = -((Far + Near) / (Far - Near));

	return *this * m;
}

Matrix4x4 Matrix4x4::perspectiveProjection(float fovRad, float aspect, float Near, float Far) const
{
	float Height = 1.0f / tanf(fovRad / 2.0f);
	float Width = Height / aspect;
	float fRange = Far / (Far - Near);

	auto m = identity();
	m.at(0, 0) = Width;
	m.at(1, 1) = Height;
	m.at(2, 2) = fRange;
	m.at(2, 3) = 1;
	m.at(3, 2) = -fRange * Near;
	m.at(3, 3) = 0;

	return *this * m;
}
