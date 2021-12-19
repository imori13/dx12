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
	return Scale(vec.x, vec.y, vec.z);
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

Matrix4x4 Matrix4x4::Translate(const Vector3& vec)
{
	return Translate(vec.x, vec.y, vec.z);
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
	return RotateRollPitchYaw(vec.x, vec.y, vec.z);
}

Matrix4x4 Matrix4x4::RotateRollPitchYaw(float x, float y, float z)
{
	return Identity() * RotateZ(z) * RotateX(x) * RotateY(y);
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
	float height = 1.0f / tanf(fovRad / 2.0f);
	float width = height / aspect;

	Eigen::Matrix4f m;
	m(0, 0) = width;
	m(1, 1) = height;
	m(2, 2) = Far / (Far - Near);
	m(2, 3) = -1;
	m(3, 2) = -Far * Near / (Far - Near);

	return m;
}
