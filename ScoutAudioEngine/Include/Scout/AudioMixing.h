#pragma once

#include <vector>
#include <stdexcept>
#include <algorithm>

#include <Scout/TypedefAndEnum.h>

// TODO: figure out a way to mix sounds without loosing volume.

namespace Scout
{
	inline void MixSignalsInPlace(std::vector<double>& data0, const std::vector<double>& data1, const MixingPolicy policy)
	{
		if (data0.size() != data1.size()) throw std::runtime_error("MixSignalsInPlace: trying to mix signals of differring sizes.");

		const auto nrOfSamples = data0.size();

		switch (policy)
		{
		case MixingPolicy::SUM_AND_CLAMP:
		{
			for (size_t i = 0; i < nrOfSamples; i++)
			{
				data0[i] += data1[i];
				data0[i] = std::clamp(data0[i], -1.0, 1.0);
			}
		}break;
		case MixingPolicy::SUM:
		{
			for (size_t i = 0; i < nrOfSamples; i++)
			{
				data0[i] += data1[i];
			}
		}break;
		case MixingPolicy::AVERAGE:
		{
			for (size_t i = 0; i < nrOfSamples; i++)
			{
				data0[i] += data1[i];
				data0[i] *= 0.5f;
			}
		}break;

		default:
		{
			throw std::runtime_error("MixSignalsInPlace: unexpected MixingPolicy.");
		}break;
		}
	}

	inline void MixSignalsInPlace(std::vector<float>& data0, const std::vector<float>& data1, const MixingPolicy policy)
	{
		if (data0.size() != data1.size()) throw std::runtime_error("MixSignalsInPlace: trying to mix signals of differring sizes.");

		const auto nrOfSamples = data0.size();

		switch (policy)
		{
		case MixingPolicy::SUM_AND_CLAMP:
		{
			for (size_t i = 0; i < nrOfSamples; i++)
			{
				data0[i] += data1[i];
				data0[i] = std::clamp(data0[i], -1.0f, 1.0f);
			}
		}break;
		case MixingPolicy::SUM:
		{
			for (size_t i = 0; i < nrOfSamples; i++)
			{
				data0[i] += data1[i];
			}
		}break;
		case MixingPolicy::AVERAGE: // Avoids clipping artifacts and does not require actual clipping. Maybe not ideal: mixing a signal with a null signal will result in an output signal half as loud.
		{
			for (size_t i = 0; i < nrOfSamples; i++)
			{
				data0[i] += data1[i];
				data0[i] *= 0.5f;
			}
		}break;

		default:
		{
			throw std::runtime_error("MixSignalsInPlace: unexpected MixingPolicy.");
		}break;
		}
	}
}