#pragma once

#include <cassert>
#include <functional>
#include <vector>
#include <algorithm>

namespace Scout
{
	std::function<void(std::vector<float>& audioData)> MakeLimiterSfx(const float ceiling)
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

	std::function<void(std::vector<float>& audioData)> MakeNoiseGateSfx(const float threshold)
	{
		assert(threshold > 0.0f && threshold < 1.0f);
		auto callback = [=](std::vector<float>& signal)
		{
			for (size_t i = 0; i < signal.size(); i++)
			{
				if (abs(signal[i]) < threshold)
				{
					signal[i] = 0.0f;
				}
			}
		};
		return callback;
	}

	std::function<void(std::vector<float>& audioData)> MakeGainSfx(const float gain)
	{
		assert(gain >= -1.0f && gain <= 1.0f);
		auto callback = [=](std::vector<float>& signal)
		{
			for (size_t i = 0; i < signal.size(); i++)
			{
				signal[i] *= gain;
			}
		};
		return callback;
	}

	std::function<void(std::vector<float>& audioData)> MakeTanhCompressorSfx(const float slopeMultiplier = 2.45f)
	{
		assert(slopeMultiplier > 0.0f);
		auto callback = [=](std::vector<float>& signal)->void
		{
			for (size_t i = 0; i < signal.size(); i++)
			{
				signal[i] = tanhf(signal[i] * slopeMultiplier);
			}
		};
		return callback;
	}

	std::function<void(std::vector<float>& audioData)> MakeStaticCurveCompressorSfx(const float limiterEnd = 0.05f, const float upwardCompThreshold = 0.183f, const float downwardCompThreshold = 0.673f, const float limiterStart = 0.95f, const float makeupGainFactor = 0.17f, const float maxAbsSignalValue = 0.954f)
	{
		// TODO: add asserts to check validity of arguments.

		auto callback = [=](std::vector<float>& signal)
		{
			// https://www.desmos.com/calculator/dywe6plrzk

			// X values
			const float& i = limiterEnd;
			const float& j = upwardCompThreshold;
			const float& k = downwardCompThreshold;
			const float& l = limiterStart;

			// Y values
			const float& g = makeupGainFactor;
			const float& M = maxAbsSignalValue;

			auto a = [=](const float x)->float {return 0; };
			auto c = [=](const float x)->float {return x + g; };
			auto b = [=](const float x)->float {return M; };
			auto d = [=](const float x)->float
			{
				return
					x * ((c(j) - a(i)) / (j - i))
					- i * ((c(j) - a(i)) / (j - i))
					;
			};
			auto h = [=](const float x)->float
			{
				return
					x * ((b(l) - c(k)) / (l - k))
					+ (k - k * ((b(l) - c(k)) / (l - k)) + g)
					;
			};

			auto q = [=](const float x)->float {return 0; };
			auto w = [=](const float x)->float {return x - g; };
			auto r = [=](const float x)->float {return -M; };
			auto t = [=](const float x)->float
			{
				return
					x * ((q(-i) - w(-j)) / (j - i))
					+ i * ((q(-i) - w(-j)) / (j - i))
					;
			};
			auto u = [=](const float x)->float
			{
				return
					x * ((w(-k) - r(-l)) / (l - k))
					+ (r(-l) + l * ((w(-k) - r(-l)) / (l - k)))
					;
			};

			for (size_t sample = 0; sample < signal.size(); sample++)
			{
				float& val = signal[sample];
				if (val >= -1.0f && val <= -l)
				{
					val = r(val);
					continue;
				}
				if (val > -l && val <= -k)
				{
					val = u(val);
					continue;
				}
				if (val > -k && val <= -j)
				{
					val = w(val);
					continue;
				}
				if (val > -j && val <= -i)
				{
					val = t(val);
					continue;
				}
				if (val > -i && val <= 0.0f)
				{
					val = q(val);
					continue;
				}
				if (val > 0.0f && val <= i)
				{
					val = a(val);
					continue;
				}
				if (val > i && val <= j)
				{
					val = d(val);
					continue;
				}
				if (val > j && val <= k)
				{
					val = c(val);
					continue;
				}
				if (val > k && val <= l)
				{
					val = h(val);
					continue;
				}
				if (val > l && val <= 1.0f)
				{
					val = b(val);
					continue;
				}
			}
		};
		return callback;
	}
}