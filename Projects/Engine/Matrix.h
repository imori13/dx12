#pragma once
#include "Vector3.h"

class Matrix
{
public:
	Matrix() noexcept = default;

	constexpr Matrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) noexcept
		: mV{ m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33 }
	{
	}

	Matrix operator*(const Matrix& m) const noexcept
	{
		return Matrix(
		mV[0][0] * m.mV[0][0] + mV[0][1] * m.mV[1][0] + mV[0][2] * m.mV[2][0] + mV[0][3] * m.mV[3][0],
		mV[0][0] * m.mV[0][1] + mV[0][1] * m.mV[1][1] + mV[0][2] * m.mV[2][1] + mV[0][3] * m.mV[3][1],
		mV[0][0] * m.mV[0][2] + mV[0][1] * m.mV[1][2] + mV[0][2] * m.mV[2][2] + mV[0][3] * m.mV[3][2],
		mV[0][0] * m.mV[0][3] + mV[0][1] * m.mV[1][3] + mV[0][2] * m.mV[2][3] + mV[0][3] * m.mV[3][3],

		mV[1][0] * m.mV[0][0] + mV[1][1] * m.mV[1][0] + mV[1][2] * m.mV[2][0] + mV[1][3] * m.mV[3][0],
		mV[1][0] * m.mV[0][1] + mV[1][1] * m.mV[1][1] + mV[1][2] * m.mV[2][1] + mV[1][3] * m.mV[3][1],
		mV[1][0] * m.mV[0][2] + mV[1][1] * m.mV[1][2] + mV[1][2] * m.mV[2][2] + mV[1][3] * m.mV[3][2],
		mV[1][0] * m.mV[0][3] + mV[1][1] * m.mV[1][3] + mV[1][2] * m.mV[2][3] + mV[1][3] * m.mV[3][3],

		mV[2][0] * m.mV[0][0] + mV[2][1] * m.mV[1][0] + mV[2][2] * m.mV[2][0] + mV[2][3] * m.mV[3][0],
		mV[2][0] * m.mV[0][1] + mV[2][1] * m.mV[1][1] + mV[2][2] * m.mV[2][1] + mV[2][3] * m.mV[3][1],
		mV[2][0] * m.mV[0][2] + mV[2][1] * m.mV[1][2] + mV[2][2] * m.mV[2][2] + mV[2][3] * m.mV[3][2],
		mV[2][0] * m.mV[0][3] + mV[2][1] * m.mV[1][3] + mV[2][2] * m.mV[2][3] + mV[2][3] * m.mV[3][3],

		mV[3][0] * m.mV[0][0] + mV[3][1] * m.mV[1][0] + mV[3][2] * m.mV[2][0] + mV[3][3] * m.mV[3][0],
		mV[3][0] * m.mV[0][1] + mV[3][1] * m.mV[1][1] + mV[3][2] * m.mV[2][1] + mV[3][3] * m.mV[3][1],
		mV[3][0] * m.mV[0][2] + mV[3][1] * m.mV[1][2] + mV[3][2] * m.mV[2][2] + mV[3][3] * m.mV[3][2],
		mV[3][0] * m.mV[0][3] + mV[3][1] * m.mV[1][3] + mV[3][2] * m.mV[2][3] + mV[3][3] * m.mV[3][3]
		);
	}

	Matrix& operator*=(const Matrix& m) noexcept
	{
		*this = *this * m;

		return *this;
	}

	constexpr inline Matrix identity() const noexcept
	{
		return Matrix(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	constexpr Matrix transpose() const noexcept
	{
		return Matrix(
			mV[0][0], mV[1][0], mV[2][0], mV[3][0],
			mV[0][1], mV[1][1], mV[2][1], mV[3][1],
			mV[0][2], mV[1][2], mV[2][2], mV[3][2],
			mV[0][3], mV[1][3], mV[2][3], mV[3][3]
		);
	}

	constexpr DirectX::XMFLOAT4X4 data() const noexcept
	{
		return DirectX::XMFLOAT4X4(
			mV[0][0], mV[0][1], mV[0][2], mV[0][3],
			mV[1][0], mV[1][1], mV[1][2], mV[1][3],
			mV[2][0], mV[2][1], mV[2][2], mV[2][3],
			mV[3][0], mV[3][1], mV[3][2], mV[3][3]);
	}

	Matrix scale(float x, float y, float z)
	{
		auto m = identity();
		m.mV[0][0] = x;
		m.mV[1][1] = y;
		m.mV[2][2] = z;

		return *this * m;
	}

	Matrix translation(float x, float y, float z)
	{
		auto m = identity();
		m.mV[3][0] = x;
		m.mV[3][1] = y;
		m.mV[3][2] = z;

		return *this * m;
	}
private:
	float mV[4][4];
};