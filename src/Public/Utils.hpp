#pragma once

#include <iostream>
#include <functional>
#include <cstdint>
#include <random>

namespace crsp {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	inline int randomRange(int minInclusive, int maxExclusive) {
		return minInclusive + rand() % maxExclusive;
	}

	inline float randomFloatRange(float minVal, float maxVal) {
		static std::random_device rd;  // only used to seed once
		static std::mt19937 gen(rd()); // Mersenne Twister RNG
		std::uniform_real_distribution<float> dist(minVal, maxVal);

		return dist(gen);
	}

	inline uint32_t nextPowerOfTwo(uint32_t v)
	{
		if (v == 0) return 1;

		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}
}