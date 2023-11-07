#pragma once

#include <cassert>
#include <functional>
#include <vector>
#include <algorithm>

namespace Scout
{
	auto fx = [](std::vector<float>& audioData)->void
	{
		constexpr const float maxAmplitude = 0.1f;
		for (size_t i = 0; i < audioData.size(); i++)
		{
			audioData[i] = std::clamp(audioData[i], -maxAmplitude, maxAmplitude);
		}
	};

	std::function<void(std::vector<float>& audioData)> MakeLimiterCallback(const float ceiling)
	{
		assert(ceiling >= 0.0f && ceiling <= 1.0f);
		auto callback = [=](std::vector<float>& audioData)->void
		{
			for (size_t i = 0; i < audioData.size(); i++)
			{
				audioData[i] = std::clamp(audioData[i], -ceiling, ceiling);
			}
		};
		return callback;
	}
}