#ifndef RANDOM_H
#define RANDOM_H

#include <ctime>
#include <random>
#include <algorithm>

namespace RandomConstants {
	static const double inverseRandMaxD = 1. / static_cast <double> (RAND_MAX);
	static const double inverseRandMaxF = 1.f / static_cast <float>  (RAND_MAX);
}


class Random {
public:
	// Initialize some static values
	static void init() { std::srand(static_cast<unsigned int>(std::time(nullptr))); }
	static void init(unsigned int seed) { std::srand(seed); }

	static float randomFloat(float LO, float HI) {
		if (LO == HI) return LO;
		return LO + static_cast <float> (rand()) * (HI - LO) * static_cast<float>(RandomConstants::inverseRandMaxF);
	}

	static double randomDouble(double LO, double HI) {
		if (LO == HI) return LO;
		return LO + static_cast <double> (rand()) * (HI - LO) * RandomConstants::inverseRandMaxD;
	}

	static int randomInt(int LO, int HI) {
		if (LO == HI) return LO;
		return rand() % (HI - LO) + LO;
	}

	static std::vector<int> permutationInt(int LO, int HI, unsigned int number) {
		if (number > (unsigned int)(HI - LO))
			throw std::exception("Unable to do permutationInt");
		std::vector<int> ret = std::vector<int>(HI - LO);
		for (int i = LO; i < HI; i++)
			ret[i] = i;
		randomShuffle(ret);
		return std::vector<int>(ret.begin(), ret.begin() + number);
	}

	template< typename T>
	static void randomShuffle(std::vector<T>& v) {
		for (int i = 0; i < v.size(); i++)
			std::swap(v[i], v[randomInt(0, v.size())]);
	}

	static glm::vec3 randomVec3(float xLO, float xHI, float yLO, float yHI, float zLO, float zHI) {
		return glm::vec3(randomFloat(xLO, xHI), randomFloat(yLO, yHI), randomFloat(zLO, zHI));
	}

private:
	Random();
};

#endif //RANDOM_H
