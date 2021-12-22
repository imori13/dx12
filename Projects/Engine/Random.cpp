#include "Random.h"
#include <random>

namespace
{
	constexpr int32_t FLOAT_MIN = INT32_MIN;
	constexpr int32_t FLOAT_MAX = INT32_MAX;

	std::default_random_engine eng;
	std::uniform_real_distribution<float> distr;
}

float Random::Next()
{
	return distr(eng);
}

void Random::Set(float min, float max)
{
	std::random_device rd;
	eng = std::default_random_engine(rd());
	distr = std::uniform_real_distribution<float>(min, max);
}
