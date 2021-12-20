#pragma once

namespace Math
{
	template<class T>
	constexpr inline T Lerp(T a, T b, T t)
	{
		return a + t * (b - a);
	}
}