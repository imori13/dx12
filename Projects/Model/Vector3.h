#pragma once

struct Vector3
{
	float x;
	float y;
	float z;

	// ÉRÉìÉXÉgÉâÉNÉ^
	explicit constexpr Vector3::Vector3() noexcept
		: x(0)
		, y(0)
		, z(0)
	{
	}

	explicit constexpr Vector3::Vector3(float value) noexcept
		: x(value)
		, y(value)
		, z(value)
	{
	}

	explicit constexpr Vector3::Vector3(float value_x, float value_y, float value_z) noexcept
		: x(value_x)
		, y(value_y)
		, z(value_z)
	{
	}

	// ââéZ
	Vector3& operator+=(const Vector3& vector) noexcept
	{
		this->x += vector.x;
		this->y += vector.y;
		this->z += vector.z;

		return *this;
	}
	Vector3& operator-=(const Vector3& vector) noexcept
	{
		this->x -= vector.x;
		this->y -= vector.y;
		this->z -= vector.z;

		return *this;
	}
	Vector3& operator*=(const float scaler) noexcept
	{
		this->x *= scaler;
		this->y *= scaler;
		this->z *= scaler;

		return *this;
	}
	Vector3& operator/=(const float scaler) noexcept
	{
		EXPECTS(scaler != 0, L"É[ÉçèúéZ");

		return *this *= (1.0f / scaler);
	}

	// ê≥ãKâª
	void Normalize() noexcept
	{
		*this /= Length();
	}

	inline Vector3 Normalized() const noexcept;
	inline float Length() const noexcept;
	inline constexpr float LengthSquared() const noexcept;
	inline constexpr float Dot(const Vector3& vector) const noexcept;
	inline constexpr Vector3 Cross(const Vector3& vector) const noexcept;
	inline constexpr DirectX::XMFLOAT3 XMFloat() const noexcept;

	inline constexpr static Vector3 Zero() noexcept;
	inline constexpr static Vector3 One() noexcept;
	inline constexpr static Vector3 Up() noexcept;
	inline constexpr static Vector3 Down() noexcept;
	inline constexpr static Vector3 Right() noexcept;
	inline constexpr static Vector3 Left() noexcept;
	inline constexpr static Vector3 Foward() noexcept;
	inline constexpr static Vector3 Backward() noexcept;
};

inline constexpr Vector3 operator-(const Vector3& vector) noexcept;
inline constexpr Vector3 operator+(const Vector3& vector_A, const Vector3& vector_B) noexcept;
inline constexpr Vector3 operator-(const Vector3& vector_A, const Vector3& vector_B) noexcept;
inline constexpr Vector3 operator*(const Vector3& vector, const float scaler) noexcept;
inline constexpr Vector3 operator*(const float scaler, const Vector3& vector) noexcept;
inline constexpr Vector3 operator/(const Vector3& vector, const float scaler) noexcept;
inline constexpr Vector3 operator/(const float scaler, const Vector3& vector) noexcept;
inline constexpr bool operator==(const Vector3& vector_A, const Vector3& vector_B) noexcept;
inline constexpr bool operator!=(const Vector3& vector_A, const Vector3& vector_B) noexcept;

// ÉCÉìÉâÉCÉìíËã`-------------------------------------------------------------------------------

inline constexpr DirectX::XMFLOAT3 Vector3::XMFloat() const noexcept
{
	return DirectX::XMFLOAT3(x, y, z);
}

inline Vector3 Vector3::Normalized() const noexcept
{
	return *this / Length();
}

inline float Vector3::Length() const noexcept
{
	return std::sqrt(Dot(*this));
}

inline constexpr float Vector3::LengthSquared() const noexcept
{
	return Dot(*this);
}

inline constexpr float Vector3::Dot(const Vector3& vector) const noexcept
{
	return x * vector.x + y * vector.y + z * vector.z;
}

inline constexpr Vector3 Vector3::Cross(const Vector3& vector) const noexcept
{
	return Vector3(y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x);
}

inline constexpr Vector3 operator-(const Vector3& vector) noexcept
{
	return Vector3(-vector.x, -vector.y, -vector.z);
}

inline constexpr Vector3 operator+(const Vector3& vector_A, const Vector3& vector_B) noexcept
{
	return Vector3(vector_A.x + vector_B.x, vector_A.y + vector_B.y, vector_A.z + vector_B.z);
}

inline constexpr Vector3 operator-(const Vector3& vector_A, const Vector3& vector_B) noexcept
{
	return Vector3(vector_A.x - vector_B.x, vector_A.y - vector_B.y, vector_A.z - vector_B.z);
}

inline constexpr Vector3 operator*(const Vector3& vector, const float scaler) noexcept
{
	return Vector3(vector.x * scaler, vector.y * scaler, vector.z * scaler);
}

inline constexpr Vector3 operator*(const float scaler, const Vector3& vector) noexcept
{
	return Vector3(scaler * vector.x, scaler * vector.y, scaler * vector.z);
}

inline constexpr Vector3 operator/(const Vector3& vector, const float scaler) noexcept
{
	EXPECTS(scaler != 0);
	return Vector3(vector.x / scaler, vector.y / scaler, vector.z / scaler);
}

inline constexpr Vector3 operator/(const float scaler, const Vector3& vector) noexcept
{
	EXPECTS(scaler != 0);
	return Vector3(scaler / vector.x, scaler / vector.y, scaler / vector.z);
}

inline constexpr bool operator==(const Vector3& vector_A, const Vector3& vector_B) noexcept
{
	return vector_A.x == vector_B.x && vector_A.y == vector_B.y && vector_A.z == vector_B.z;
}

inline constexpr bool operator!=(const Vector3& vector_A, const Vector3& vector_B) noexcept
{
	return vector_A.x != vector_B.x || vector_A.y != vector_B.y || vector_A.z != vector_B.z;
}

inline constexpr Vector3 Vector3::Zero() noexcept
{
	return Vector3(0);
}
inline constexpr Vector3 Vector3::One() noexcept
{
	return Vector3(1);
}
inline constexpr Vector3 Vector3::Up() noexcept
{
	return Vector3(0,1,0);
}
inline constexpr Vector3 Vector3::Down() noexcept
{
	return Vector3(0, -1, 0);
}
inline constexpr Vector3 Vector3::Right() noexcept
{
	return Vector3(1, 0, 0);
}
inline constexpr Vector3 Vector3::Left() noexcept
{
	return Vector3(-1, 0, 0);
}
inline constexpr Vector3 Vector3::Foward() noexcept
{
	return Vector3(0, 0, 1);
}
inline constexpr Vector3 Vector3::Backward() noexcept
{
	return Vector3(0, 0, -1);
}