#pragma once

#include <cmath>

namespace Scout
{
	struct Color
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;

		constexpr inline Color operator+(const Color& other) const
		{
			return
			{
				r + other.r,
				g + other.g,
				b + other.b,
				a + other.a
			};
		}

		constexpr inline Color operator-(const Color& other) const
		{
			return
			{
				r - other.r,
				g - other.g,
				b - other.b,
				a - other.a
			};
		}
	};

	constexpr inline Color NormalizedColor(const Color c)
	{
		// Reinhard tonemapping.
		return
		{
			c.r / (1.0f + c.r),
			c.g / (1.0f + c.g),
			c.b / (1.0f + c.b),
			c.a / (1.0f + c.a)
		};
	}

	constexpr const Color COLOR_CLEAR = { 0.0f, 0.0f, 0.0f, 0.0f };
	constexpr const Color COLOR_BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
	constexpr const Color COLOR_RED = { COLOR_BLACK.r + 1.0f, COLOR_BLACK.g + 0.0f, COLOR_BLACK.b + 0.0f, COLOR_BLACK.a };
	constexpr const Color COLOR_GREEN = { COLOR_BLACK.r + 0.0f, COLOR_BLACK.g + 1.0f, COLOR_BLACK.b + 0.0f, COLOR_BLACK.a };
	constexpr const Color COLOR_BLUE = { COLOR_BLACK.r + 0.0f, COLOR_BLACK.g + 0.0f, COLOR_BLACK.b + 1.0f, COLOR_BLACK.a };
	constexpr const Color COLOR_WHITE = { COLOR_RED.r, COLOR_GREEN.g, COLOR_BLUE.b, COLOR_BLACK.a };
}