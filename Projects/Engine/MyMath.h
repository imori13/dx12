#pragma once
#include "Vector3.h"

namespace Math
{
	template<class T, class Y>constexpr inline T Lerp(T a, T b, Y t) noexcept
	{ return a + t * (b - a); }
}