#include "Matrix4x4.h"

// プライベートクラス
class APrivate
{
public:
	Eigen::Matrix4f matrix;
};

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& m)
{
	matrix += m.matrix;
	return *this;
}
Matrix4x4 Matrix4x4::operator+(const Matrix4x4& m) const
{
	return matrix + m.matrix;
}
Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& m)
{
	matrix -= m.matrix;
	return *this;
}
Matrix4x4 Matrix4x4::operator-(const Matrix4x4& m) const
{
	return matrix - m.matrix;
}
Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m)
{
	matrix *= m.matrix;
	return *this;
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

DirectX::XMMATRIX Matrix4x4::Data() const
{
	return DirectX::XMMATRIX(matrix.data());
}

Matrix4x4 Matrix4x4::Transpose() const
{
	return matrix.transpose();
}

Matrix4x4 Matrix4x4::Inverse() const
{
	return matrix.inverse();
}

Matrix4x4 Matrix4x4::Identity()
{
	return Eigen::Matrix4f::Identity();
}

Matrix4x4 Matrix4x4::Scale(const Vector3& vec)
{
	return Scale(vec.x(), vec.y(), vec.z());
}

Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(0, 0) = x;
	m(1, 1) = y;
	m(2, 2) = z;

	return m;
}

Matrix4x4 Matrix4x4::RotateX(float radian)
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(1, 1) = cos(radian);
	m(1, 2) = sin(radian);
	m(2, 1) = -sin(radian);
	m(2, 2) = cos(radian);

	return m;
}

Matrix4x4 Matrix4x4::RotateY(float radian)
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(0, 0) = cos(radian);
	m(0, 2) = -sin(radian);
	m(2, 0) = sin(radian);
	m(2, 2) = cos(radian);

	return m;
}

Matrix4x4 Matrix4x4::RotateZ(float radian)
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(0, 0) = cos(radian);
	m(0, 1) = sin(radian);
	m(1, 0) = -sin(radian);
	m(1, 1) = cos(radian);

	return m;
}

Matrix4x4 Matrix4x4::RotateAxis(const Vector3& vector, float radian)
{
	float X = vector.x();
	float Y = vector.y();
	float Z = vector.z();

	float Cos = cos(radian);
	float Sin = sin(radian);
	float A = 1 - Cos;

	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(0, 0) = X * X * A + Cos;
	m(0, 1) = X * Y * A - Z * Sin;
	m(0, 2) = Z * X * A + Y * Sin;

	m(1, 0) = X * Y * A + Z * Sin;
	m(1, 1) = Y * Y * A + Cos;
	m(1, 2) = Y * Z * A - X * Sin;

	m(2, 0) = Z * X * A - Y * Sin;
	m(2, 1) = Y * Z * A + X * Sin;
	m(2, 2) = Z * Z * A + Cos;

	return m;
}

Matrix4x4 Matrix4x4::Translate(const Vector3& vec)
{
	return Translate(vec.x(), vec.y(), vec.z());
}

Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(3, 0) = x;
	m(3, 1) = y;
	m(3, 2) = z;

	return m;
}

Matrix4x4 Matrix4x4::RotateRollPitchYaw(const Vector3& vec)
{
	return RotateRollPitchYaw(vec.x(), vec.y(), vec.z());
}

Matrix4x4 Matrix4x4::RotateRollPitchYaw(float x, float y, float z)
{
	return Identity() * RotateZ(z) * RotateX(x) * RotateY(y);
}

Matrix4x4 Matrix4x4::LookAt(const Vector3& cameraPos, const Vector3& targetPos, const Vector3& upward)
{
	Vector3 Z = (targetPos - cameraPos);
	Z = (Z != Vector3::Zero()) ? (Z.Normalized()) : (Vector3::Zero());

	Vector3 X = upward.Cross(Z);
	Vector3 Y = Z.Cross(X);

	X = (X != Vector3::Zero()) ? (X.Normalized()) : (Vector3::Zero());
	Y = (Y != Vector3::Zero()) ? (Y.Normalized()) : (Vector3::Zero());

	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();

	m(0, 0) = X.x();
	m(1, 0) = X.y();
	m(2, 0) = X.z();
	m(3, 0) = -X.Dot(cameraPos);

	m(0, 1) = Y.x();
	m(1, 1) = Y.y();
	m(2, 1) = Y.z();
	m(3, 1) = -Y.Dot(cameraPos);

	m(0, 2) = Z.x();
	m(1, 2) = Z.y();
	m(2, 2) = Z.z();
	m(3, 2) = -Z.Dot(cameraPos);

	return m;
}

Matrix4x4 Matrix4x4::ParrallelProjection(float top, float bottom, float right, float left, float Near, float Far)
{
	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(0, 0) = 2.0f / (right - left);
	m(1, 1) = 2.0f / (top - bottom);
	m(2, 2) = 2.0f / (Far - Near);

	m(0, 3) = -((right + left) / (right - left));
	m(1, 3) = -((top + bottom) / (top - bottom));
	m(2, 3) = -((Far + Near) / (Far - Near));

	return m;
}

Matrix4x4 Matrix4x4::PerspectiveProjection(float fovRad, float aspect, float Near, float Far)
{
	float Height = 1.0f / tanf(fovRad / 2.0f);
	float Width = Height / aspect;
	float fRange = Far / (Far - Near);

	Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
	m(0, 0) = Width;
	m(1, 1) = Height;
	m(2, 2) = fRange;
	m(2, 3) = 1;
	m(3, 2) = -fRange * Near;
	m(3, 3) = 0;

	return m;
}
