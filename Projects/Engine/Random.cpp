#include "Random.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

namespace
{
	boost::random::mt19937 gen;
	boost::random::uniform_real_distribution<float> dist;
}

float Random::Next()
{
	return dist(gen);
}

void Random::Set(float min, float max)
{
	boost::random::random_device seed_gen;
	gen = boost::random::mt19937(seed_gen);
	dist = boost::random::uniform_real_distribution<float>(min, max);
}
