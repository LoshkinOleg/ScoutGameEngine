#pragma once

namespace sge
{
	template <typename T>
	constexpr const T RemapToRange(const T inRangeMin, const T inRangeMax, const T outRangeMin, const T outRangeMax, const T value)
	{
		return outRangeMin + (value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin);
	}
}//!sge