#pragma once

namespace sge
{
	template <typename T>
	constexpr const T RemapToRange(const T inRangeMin, const T inRangeMax, const T outRangeMin, const T outRangeMax, const T value)
	{
		return outRangeMin + (value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin);
	}

	template<typename First, typename Second, typename Third, typename Fourth>
	class Quadruple
	{
		First first = {};
		Second second = {};
		Third third = {};
		Fourth fourth = {};
	};
	template<typename First, typename Second, typename Third>
	class Triple
	{
		First first = {};
		Second second = {};
		Third third = {};
	};
	template<typename First, typename Second>
	class Pair
	{
		First first = {};
		Second second = {};
	};
}//!sge