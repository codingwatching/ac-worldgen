#pragma once

#include <cstdlib>

#include "worldgen/cpu/supp/worldgen_cpu_utils.h"

class ContainerUtils {

public:
	/// Takes sampleCount random samples and returns index of the one with best scoreFunc(value)
	template<typename C, typename ScoreFunc>
	static size_t probabilisticSelect(const C &c, const ScoreFunc &scoreFunc, size_t sampleCount = 32) {
		using Score = decltype(scoreFunc(std::declval<decltype(c[0])>()));

		if(c.isEmpty())
			return -1;

		const size_t sz = c.size();
		sampleCount = std::min(sz, sampleCount);

		int bestIx = std::rand() % sz;
		Score bestScore = scoreFunc(c[bestIx]);
		for(size_t i = 1; i < sampleCount; i++) {
			const size_t ix = std::rand() % sz;
			const Score score = scoreFunc(c[ix]);

			if(score > bestScore) {
				bestScore = score;
				bestIx = ix;
			}
		}

		return bestIx;
	}

	/// Pseudo-randomly shuffles the order of the items using Fisher-Yates shuffle
	template<typename I>
	static void randomShuffle(I begin, I end, uint32_t seed) {
		const size_t sz = end - begin;
		size_t ix = 0;
		for(auto i = begin; i < end; i++) {
			const auto j = i + (WorldGen_CPU_Utils::hash(ix, seed) % (sz - ix));
			std::swap(*i, *j);
			ix++;
		}
	}

};