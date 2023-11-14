#pragma once

#include <cstdint>
#include <cmath>
#include <vector>
#include <complex>
#include <random>

namespace Scout
{
	// Mathematical constants ==============================
	constexpr const double PI = 3.141592653589793;
	constexpr const float PIf = (float)PI;

	// General global functions ===============================

	constexpr inline const std::uint64_t RemapToRange(const std::uint64_t inRangeMin, const std::uint64_t inRangeMax, const std::uint64_t outRangeMin, const std::uint64_t outRangeMax, const std::uint64_t value)
	{
		return outRangeMin + (value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin);
	}
	constexpr inline const double RemapToRange(const double inRangeMin, const double inRangeMax, const double outRangeMin, const double outRangeMax, const double value)
	{
		return outRangeMin + (value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin);
	}
	constexpr inline const float RemapToRange(const float inRangeMin, const float inRangeMax, const float outRangeMin, const float outRangeMax, const float value)
	{
		return outRangeMin + (value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin);
	}

	constexpr inline std::uint64_t NearestUpperPowOfTwo(const std::uint64_t val)
	{
		// taken from: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
		std::uint64_t v = val;
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v |= v >> 32;
		v++;
		return v;
	}

	constexpr inline bool IsPowerOfTwo(const std::uint64_t x)
	{
		// Taken from: https://stackoverflow.com/questions/108318/how-can-i-test-whether-a-number-is-a-power-of-2
		return (x & (x - 1)) == 0;
	}

	inline std::complex<double> EulersFormula(const double x)
	{
		return std::complex<double>(std::cos(x), std::sin(x));
	}

	inline std::complex<float> EulersFormula(const float x)
	{
		return std::complex<float>(std::cosf(x), std::sinf(x));
	}

	// https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
	template <typename T> int Sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	// Vectors ==================================

	struct Vec4
	{
		float x = 0;
		float y = 0;
		float z = 0;
		float w = 1; // 1 by default since that's what we usually need when dealing with 4x4 transform matrices.

		constexpr inline bool operator==(const Vec4& other) const
		{
			return	x == other.x &&
				y == other.y &&
				z == other.z &&
				w == other.w;
		}

		constexpr inline Vec4 operator+(const Vec4& other) const
		{
			return
			{
				x + other.x,
				y + other.y,
				z + other.z,
				w + other.w
			};
		}

		constexpr inline Vec4 operator-(const Vec4& other) const
		{
			return
			{
				x - other.x,
				y - other.y,
				z - other.z,
				w - other.w
			};
		}

		constexpr inline Vec4 operator-() const
		{
			return
			{
				-x,
				-y,
				-z,
				-w
			};
		}
	};

	struct Vec3
	{
		float x = 0;
		float y = 0;
		float z = 0;

		constexpr inline bool operator==(const Vec3& other) const
		{
			return	x == other.x &&
				y == other.y &&
				z == other.z;
		}

		constexpr inline Vec3 operator+(const Vec3& other) const
		{
			return
			{
				x + other.x,
				y + other.y,
				z + other.z
			};
		}

		constexpr inline Vec3 operator-(const Vec3& other) const
		{
			return
			{
				x - other.x,
				y - other.y,
				z - other.z
			};
		}

		constexpr inline Vec3 operator-() const
		{
			return
			{
				-x,
				-y,
				-z
			};
		}
	};

	struct Vec2
	{
		float x = 0;
		float y = 0;

		constexpr inline bool operator==(const Vec2& other) const
		{
			return	x == other.x &&
				y == other.y;
		}

		constexpr inline Vec2 operator+(const Vec2& other) const
		{
			return
			{
				x + other.x,
				y + other.y
			};
		}

		constexpr inline Vec2 operator-(const Vec2& other) const
		{
			return
			{
				x - other.x,
				y - other.y
			};
		}

		constexpr inline Vec2 operator-() const
		{
			return
			{
				-x,
				-y
			};
		}
	};

	constexpr const Vec2 VEC2_ZERO = { 0.0f, 0.0f };
	constexpr const Vec2 VEC2_RIGHT = { 1.0f, 0.0f };
	constexpr const Vec2 VEC2_UP = { 0.0f, 1.0f };
	constexpr const Vec2 VEC2_LEFT = -VEC2_RIGHT;
	constexpr const Vec2 VEC2_DOWN = -VEC2_UP;
	constexpr const Vec2 VEC2_ONE = VEC2_RIGHT + VEC2_DOWN;

	constexpr const Vec3 VEC3_ZERO = { 0.0f, 0.0f, 0.0f };
	constexpr const Vec3 VEC3_RIGHT = { 1.0f, 0.0f, 0.0f };
	constexpr const Vec3 VEC3_UP = { 0.0f, 1.0f, 0.0f };
	constexpr const Vec3 VEC3_FRONT = { 0.0f, 0.0f, 1.0f };
	constexpr const Vec3 VEC3_LEFT = -VEC3_RIGHT;
	constexpr const Vec3 VEC3_BACK = -VEC3_FRONT;
	constexpr const Vec3 VEC3_DOWN = -VEC3_UP;
	constexpr const Vec3 VEC3_ONE = VEC3_RIGHT + VEC3_FRONT + VEC3_UP;

	// Following the right-hand-rule as well for 4D coordinates but distinguishing between "true" zero and unit vectors and "regular" versions of them that have the w component set to 1 since Vec4's are usually used in 4x4 matrices.
	constexpr const Vec4 VEC4_TRUE_ZERO = { VEC3_ZERO.x,	VEC3_ZERO.y,	VEC3_ZERO.z,	0.0f };
	constexpr const Vec4 VEC4_TRUE_RIGHT = { VEC3_RIGHT.x, VEC3_RIGHT.y,	VEC3_RIGHT.z,	0.0f };
	constexpr const Vec4 VEC4_TRUE_FRONT = { VEC3_FRONT.x, VEC3_FRONT.y,	VEC3_FRONT.z,	0.0f };
	constexpr const Vec4 VEC4_TRUE_UP = { VEC3_UP.x,	VEC3_UP.y,		VEC3_UP.z,		0.0f };
	constexpr const Vec4 VEC4_W_UNIT = { VEC3_ZERO.x,	VEC3_ZERO.y,	VEC3_ZERO.z,	1.0f };
	constexpr const Vec4 VEC4_ZERO = VEC4_TRUE_ZERO + VEC4_W_UNIT;
	constexpr const Vec4 VEC4_RIGHT = VEC4_TRUE_RIGHT + VEC4_W_UNIT;
	constexpr const Vec4 VEC4_FRONT = VEC4_TRUE_FRONT + VEC4_W_UNIT;
	constexpr const Vec4 VEC4_UP = VEC4_TRUE_UP + VEC4_W_UNIT;
	constexpr const Vec4 VEC4_TRUE_LEFT = -VEC4_TRUE_RIGHT;
	constexpr const Vec4 VEC4_TRUE_BACK = -VEC4_TRUE_FRONT;
	constexpr const Vec4 VEC4_TRUE_DOWN = -VEC4_TRUE_UP;
	constexpr const Vec4 VEC4_LEFT = VEC4_TRUE_LEFT + VEC4_W_UNIT;
	constexpr const Vec4 VEC4_BACK = VEC4_TRUE_BACK + VEC4_W_UNIT;
	constexpr const Vec4 VEC4_DOWN = VEC4_TRUE_DOWN + VEC4_W_UNIT;

	// Matrices ========================
	
	// TODO: add methods to set position, scale and rotation without needing to touch the matrix values manually, this is very prone to mistakes

	struct Mat4x4
	{
		float m00 = 1.0f; float m01 = 0.0f; float m02 = 0.0f; float m03 = 0.0f;
		float m10 = 0.0f; float m11 = 1.0f; float m12 = 0.0f; float m13 = 0.0f;
		float m20 = 0.0f; float m21 = 0.0f; float m22 = 1.0f; float m23 = 0.0f;
		float m30 = 0.0f; float m31 = 0.0f; float m32 = 0.0f; float m33 = 1.0f;

		constexpr inline bool operator==(const Mat4x4& other) const
		{
			return	m00 == other.m00 && m01 == other.m01 && m02 == other.m02 && m03 == other.m03 &&
				m10 == other.m10 && m11 == other.m11 && m12 == other.m12 && m13 == other.m13 &&
				m20 == other.m20 && m21 == other.m21 && m22 == other.m22 && m23 == other.m23 &&
				m30 == other.m30 && m31 == other.m31 && m32 == other.m32 && m33 == other.m33;
		}

		constexpr inline Mat4x4 operator*(const Mat4x4& other) const
		{
			const Mat4x4& a = *this;
			const Mat4x4& b = other;

			return
			{
				a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30,	a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31,	a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32,	a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33,
				a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30,	a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31,	a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32,	a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33,
				a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30,	a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31,	a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32,	a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33,
				a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30,	a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31,	a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32,	a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33
			};
		}

		constexpr inline Vec4 operator*(const Vec4 vec) const
		{
			const Mat4x4& a = *this;
			const Vec4& b = vec;

			return
			{
				a.m00 * b.x + a.m01 * b.y + a.m02 * b.z + a.m03 * b.w,
				a.m10 * b.x + a.m11 * b.y + a.m12 * b.z + a.m13 * b.w,
				a.m20 * b.x + a.m21 * b.y + a.m22 * b.z + a.m23 * b.w,
				a.m30 * b.x + a.m31 * b.y + a.m32 * b.z + a.m33 * b.w
			};
		}

		constexpr inline Mat4x4 Inverse() const
		{
			// Taken from: https://www.thecrazyprogrammer.com/2017/02/c-c-program-find-inverse-matrix.html

			float det = 0.0f;
			const float mat[4][4] =
			{
				{m00, m01, m02, m03},
				{m10, m11, m12, m13},
				{m20, m21, m22, m23},
				{m30, m31, m32, m33}
			};
			float imat[4][4] = {};

			for (size_t i = 0; i < 4; i++)
			{
				det += (mat[0][i] * (mat[1][(i + 1) % 3] * mat[2][(i + 2) % 3] - mat[1][(i + 2) % 3] * mat[2][(i + 1) % 3]));
			}

			for (size_t r = 0; r < 4; r++)
			{
				for (size_t c = 0; c < 4; c++)
				{
					imat[r][c] = ((mat[(c + 1) % 3][(r + 1) % 3] * mat[(c + 2) % 3][(r + 2) % 3]) - (mat[(c + 1) % 3][(r + 2) % 3] * mat[(c + 2) % 3][(r + 1) % 3])) / det;
				}
			}

			return
			{
				imat[0][0], imat[0][1], imat[0][2], imat[0][3],
				imat[1][0], imat[1][1], imat[1][2], imat[1][3],
				imat[2][0], imat[2][1], imat[2][2], imat[2][3],
				imat[3][0], imat[3][1], imat[3][2], imat[3][3]
			};
		}
	};

	constexpr inline Mat4x4 OrthogonalProjectionMatrix(const float near, const float far, const float right, const float left, const float bottom, const float top)
	{
		// Rearranged version of: https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix

		// assert(far > near && left > right && top > bottom && "Invalid bounds for an orthogonal projection matrix.");

		return
		{
			2.0f / (right - left),	0.0f,					0.0f,					-(right + left) / (right - left),
			0.0f,					2.0f / (far - near),	0.0f,					-(far + near) / (far - near),
			0.0f,					0.0f,					2.0f / (top - bottom),	-(top + bottom) / (top - bottom),
			0.0f,					0.0f,					0.0f,					1.0f
		};
	}

	constexpr inline Vec4 MatrixVectorMultiplication(const Mat4x4 a, const Vec4 b)
	{
		return
		{
			a.m00 * b.x + a.m01 * b.y + a.m02 * b.z + a.m03 * b.w,
			a.m10 * b.x + a.m11 * b.y + a.m12 * b.z + a.m13 * b.w,
			a.m20 * b.x + a.m21 * b.y + a.m22 * b.z + a.m23 * b.w,
			a.m30 * b.x + a.m31 * b.y + a.m32 * b.z + a.m33 * b.w
		};
	}

	constexpr inline Mat4x4 MatrixMultiplication(const Mat4x4 a, const Mat4x4 b)
	{
		return
		{
			a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30,	a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31,	a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32,	a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33,
			a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30,	a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31,	a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32,	a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33,
			a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30,	a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31,	a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32,	a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33,
			a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30,	a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31,	a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32,	a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33
		};
	}

	inline Mat4x4 RotationMatrix(const float yaw, const float pitch, const float roll)
	{
		// Taken from: https://en.wikipedia.org/wiki/Rotation_matrix

		const float cosa = std::cosf(yaw);
		const float sina = std::sinf(yaw);
		const float cosb = std::cosf(pitch);
		const float sinb = std::sinf(pitch);
		const float cosy = std::cosf(roll);
		const float siny = std::sinf(roll);

		return
		{
			cosb * cosy,	sina * sinb * cosy - cosa * siny,	cosa * sinb * cosy + sina * siny,	0.0f,
			cosb * siny,	sina * sinb * siny + cosa * cosy,	cosa * sinb * siny - sina * cosy,	0.0f,
			-sinb,			sina * cosb,						cosa * cosb,						0.0f,
			0.0f,			0.0f,								0.0f,								1.0f
		};
	}

	inline Mat4x4 RotationMatrix(const Vec3 axis, const float rad)
	{
		// Taken from: https://en.wikipedia.org/wiki/Rotation_matrix

		const float cos = std::cosf(rad);
		const float sin = std::sinf(rad);
		const float mcos = 1.0f - cos;
		const float x = axis.x;
		const float y = axis.y;
		const float z = axis.z;

		return
		{
			cos + x * x * mcos,	x * y * mcos - z * sin,	x * z * mcos + y * sin,	0.0f,
			y * x * mcos + z * sin,	cos + y * y * mcos,	y * z * mcos - x * sin,	0.0f,
			z * x * mcos - y * sin,	z * y * mcos + x * sin,	cos + z * z * mcos,	0.0f,
			0.0f,			0.0f,			0.0f,			1.0f
		};
	}

	constexpr const Mat4x4 UNIT_ORTHO_PROJECTION = OrthogonalProjectionMatrix(0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f);
	constexpr const Mat4x4 MAT4_IDENTITY = Mat4x4{};

	// Boxes =========================================

	struct Box
	{
		Vec3 min = {-0.5f, -0.5f, -0.5f};
		Vec3 max = { 0.5f,  0.5f,  0.5f};
	};

	constexpr const Box UNIT_BOX = {};

	// Signals ==================================

	inline float GenerateSineSample(const float n, const float sampleRate, const float frequency)
	{
		// 2*PI is a full rotation. n/sampleRate ensures that the full rotation happens at 1 Hz. frequency then changes the pitch of this "unit pitch".
		return std::sinf((2.0f * PIf * n / sampleRate) * frequency);
	}

	inline double GenerateSineSample(const double n, const double sampleRate, const double frequency)
	{
		// 2*PI is a full rotation. n/sampleRate ensures that the full rotation happens at 1 Hz. frequency then changes the pitch of this "unit pitch".
		return std::sin((2.0 * PI * n / sampleRate) * frequency);
	}

	inline std::vector<double> GenerateSineSignal(const double frequency, const double phaseOffset, const std::uint64_t duration)
	{
		std::vector<double> returnVal(duration, 0.0);

		for (size_t i = 0; i < duration; i++)
		{
			returnVal[i] = std::sin((double)i * PI / frequency + phaseOffset);
		}

		return returnVal;
	}

	inline std::vector<float> GenerateSineSignal(const float frequency, const float phaseOffset, const std::uint64_t duration)
	{
		std::vector<float> returnVal(duration, 0.0f);

		for (size_t i = 0; i < duration; i++)
		{
			returnVal[i] = std::sinf((float)i * PIf / frequency + phaseOffset);
		}

		return returnVal;
	}

	constexpr inline void ScaleSignal(std::vector<double>& signal, const double scalar)
	{
		for (auto& sample : signal)
		{
			sample *= scalar;
		}
	}

	constexpr inline void ScaleSignal(std::vector<float>& signal, const float scalar)
	{
		for (auto& sample : signal)
		{
			sample *= scalar;
		}
	}

	inline std::vector<float> WhiteNoise(const std::uint64_t N, const size_t seed)
	{
		static auto e = std::default_random_engine((unsigned int)seed);
		static auto d = std::uniform_real_distribution<float>(-1.0f, 1.0f);

		std::vector<float> returnVal(N);
		for (size_t i = 0; i < N; ++i)
		{
			returnVal[i] = d(e);
		}
		return returnVal;
	}

	inline std::vector<float> GaussianWhiteNoise(const std::uint64_t N, const size_t seed)
	{
		static auto e = std::default_random_engine((unsigned int)seed);
		static auto d = std::normal_distribution<float>(0.0f, 1.0f);

		std::vector<float> returnVal(N);
		for (size_t i = 0; i < N; ++i)
		{
			returnVal[i] = d(e);
		}
		return returnVal;
	}

	inline void DFT(std::vector<std::complex<float>>& out, const std::vector<float>& x, const std::uint64_t K)
	{
		const std::uint64_t N = (std::uint64_t)x.size();
		std::vector<std::complex<float>>& y = out;

		std::fill(y.begin(), y.end(), std::complex<float>(0.0f, 0.0f));

		for (std::uint64_t k = 0; k < K; ++k)
		{
			for (std::uint64_t n = 0; n < N; ++n)
			{
				y[k] += x[n] * EulersFormula(-2.0f * PIf * k * n / N);
			}
		}
	}

	inline void IDFT(std::vector<float>& out, const std::vector<std::complex<float>>& y, const std::uint64_t N)
	{
		const std::uint64_t K = (std::uint64_t)y.size();
		std::vector<float>& x = out;

		std::fill(x.begin(), x.end(), 0.0f);

		for (std::uint64_t n = 0; n < N; ++n)
		{
			for (std::uint64_t k = 0; k < K; ++k)
			{
				x[n] += (y[k] * EulersFormula(2.0f * PIf * k * n / N)).real();
			}
			x[n] /= N;
			x[n] = std::clamp(x[n], -1.0f, 1.0f);
		}
	}

	constexpr inline void InterleaveSignal(std::vector<double>& data, const std::uint64_t nrOfChannels)
	{
		const size_t len = data.size();
		const std::vector<double> dataCopy = data;

		std::vector<size_t> channelOffsets(nrOfChannels, 0);
		for (size_t i = 0; i < nrOfChannels; i++)
		{
			channelOffsets[i] = len / nrOfChannels * i;
		}

		for (size_t i = 0; i < len / nrOfChannels; i++)
		{
			for (size_t chan = 0; chan < nrOfChannels; chan++)
			{
				data[nrOfChannels * i + chan] = dataCopy[channelOffsets[chan] + i];
			}
		}
	}

	constexpr inline void InterleaveSignal(std::vector<float>& data, const std::uint64_t nrOfChannels)
	{
		const size_t len = data.size();
		const std::vector<float> dataCopy = data;

		std::vector<size_t> channelOffsets(nrOfChannels, 0);
		for (size_t i = 0; i < nrOfChannels; i++)
		{
			channelOffsets[i] = len / nrOfChannels * i;
		}

		for (size_t i = 0; i < len / nrOfChannels; i++)
		{
			for (size_t chan = 0; chan < nrOfChannels; chan++)
			{
				data[nrOfChannels * i + chan] = dataCopy[channelOffsets[chan] + i];
			}
		}
	}

	constexpr inline void UninterleaveSignal(std::vector<double>& data, const std::uint64_t nrOfChannels)
	{
		const auto len = data.size();
		const auto singleChannelLen = data.size() / nrOfChannels;
		std::vector<std::vector<double>> channels(nrOfChannels, std::vector<double>(singleChannelLen, 0.0f));

		for (size_t i = 0; i < len; i += nrOfChannels)
		{
			for (size_t chan = 0; chan < nrOfChannels; chan++)
			{
				channels[chan][i / nrOfChannels] = data[i];
			}
		}

		for (size_t i = 0; i < nrOfChannels; i++)
		{
			std::copy(channels[i].begin(), channels[i].end(), data.begin() + i * singleChannelLen);
		}
	}

	constexpr inline void UninterleaveSignal(std::vector<float>& data, const std::uint64_t nrOfChannels)
	{
		const auto len = data.size();
		const auto singleChannelLen = data.size() / nrOfChannels;
		std::vector<std::vector<float>> channels(nrOfChannels, std::vector<float>(singleChannelLen, 0.0f));

		for (size_t i = 0; i < len; i += nrOfChannels)
		{
			for (size_t chan = 0; chan < nrOfChannels; chan++)
			{
				channels[chan][i / nrOfChannels] = data[i];
			}
		}

		for (size_t i = 0; i < nrOfChannels; i++)
		{
			std::copy(channels[i].begin(), channels[i].end(), data.begin() + i * singleChannelLen);
		}
	}

	constexpr inline void SumSignalsInPlace(std::vector<double>& data0, const std::vector<double>& data1)
	{
		for (size_t i = 0; i < data1.size(); i++)
		{
			data0[i] += data1[i];
		}
	}

	constexpr inline void SumSignalsInPlace(std::vector<float>& data0, const std::vector<float>& data1)
	{
		for (size_t i = 0; i < data1.size(); i++)
		{
			data0[i] += data1[i];
		}
	}

	constexpr inline void InverseSignal(std::vector<double>& data)
	{
		for (auto& sample : data)
		{
			sample = -sample;
		}
	}

	constexpr inline void InverseSignal(std::vector<float>& data)
	{
		for (auto& sample : data)
		{
			sample = -sample;
		}
	}

	constexpr inline std::vector<double> ComputeSignalsDifference(const std::vector<double>& data0, const std::vector<double>& data1)
	{
		std::vector<double> returnVal = data0;
		InverseSignal(returnVal);
		SumSignalsInPlace(returnVal, data1);
		return returnVal;
	}

	constexpr inline std::vector<float> ComputeSignalsDifference(const std::vector<float>& data0, const std::vector<float>& data1)
	{
		std::vector<float> returnVal = data0;
		InverseSignal(returnVal);
		SumSignalsInPlace(returnVal, data1);
		return returnVal;
	}

	constexpr inline void ClampSignal(std::vector<double>& data)
	{
		for (auto& sample : data)
		{
			sample = std::clamp(sample, -1.0, 1.0);
		}
	}

	constexpr inline void ClampSignal(std::vector<float>& data)
	{
		for (auto& sample : data)
		{
			sample = std::clamp(sample, -1.0f, 1.0f);
		}
	}

	constexpr inline void NormalizeSignal(std::vector<double>& data)
	{
		const auto len = data.size();

		// Find bounds.
		double maxSample = std::numeric_limits<double>::min();
		double minSample = std::numeric_limits<double>::max();
		for (size_t sample = 0; sample < len; sample++)
		{
			const double currentSample = data[sample];
			maxSample = maxSample < currentSample ? currentSample : maxSample;
			minSample = minSample > currentSample ? currentSample : minSample;
		}

		// Normalize.
		for (size_t sample = 0; sample < len; sample++)
		{
			data[sample] = RemapToRange(minSample, maxSample, -1.0, 1.0, data[sample]);
		}
	}

	constexpr inline void NormalizeSignal(std::vector<float>& data)
	{
		const auto len = data.size();

		// Find bounds.
		float maxSample = std::numeric_limits<float>::min();
		float minSample = std::numeric_limits<float>::max();
		for (size_t sample = 0; sample < len; sample++)
		{
			const float currentSample = data[sample];
			maxSample = maxSample < currentSample ? currentSample : maxSample;
			minSample = minSample > currentSample ? currentSample : minSample;
		}

		// Normalize.
		for (size_t sample = 0; sample < len; sample++)
		{
			data[sample] = RemapToRange(minSample, maxSample, -1.0f, 1.0f, data[sample]);
		}
	}

	constexpr inline void MonoToDualMonoSignal(std::vector<float>& signal, const bool interleaved)
	{
		if (interleaved)
		{
			// signal.resize(signal.size() * 2);
			signal.insert(signal.end(), signal.size(), 0.0f); // Resize vector.
			for (size_t i = (signal.size() / 2) - 1; i < signal.size() / 2; i--)
			{
				signal[i * 2 + 1] = signal[i];
				signal[i * 2] = signal[i];
			}
		}
		else
		{
			throw std::runtime_error("Implement this.");
		}
	}

	constexpr inline void StereoToMonoSignal(std::vector<float>& signal, const bool interleaved)
	{
		if (interleaved)
		{
			for (size_t i = 0; i < signal.size(); i += 2)
			{
				// Average mix.
				signal[i] += signal[i + 1];
				signal[i] *= 0.5f;
			}
			for (size_t i = 0; i < signal.size() / 2; i++)
			{
				signal[i] = signal[i * 2 + 1];
			}
			signal.resize(signal.size() / 2);
			return;
		}
		else
		{
			throw std::runtime_error("Implement this.");
		}
	}

	constexpr inline void ChangeNrOfChannelsInSignal(std::vector<float>& signal, const size_t from, const size_t to, const bool interleaved)
	{
		if (from == to) return;

		if (from == 1 && to == 2) // When going from mono to dual mono, don't fill the second channel with silence, duplicate the left channel into the right one instead.
		{
			MonoToDualMonoSignal(signal, interleaved);
			return;
		}

		if (interleaved)
		{
			if (from < to)
			{
				const size_t originalNrOfSamples = signal.size();
				const size_t nrOfFrames = originalNrOfSamples / from;
				const size_t targetNrOfSamples = nrOfFrames * to;
				const std::vector<float> originalSignal = signal;

				signal.resize(nrOfFrames * to);
				std::fill(signal.begin(), signal.end(), 0.0f);

				for (size_t frame = 0; frame < nrOfFrames; frame++)
				{
					for (size_t sample = 0; sample < from; sample++)
					{
						signal[frame * to + sample] = originalSignal[frame * from + sample];
					}
				}
			}
			else // downmixing
			{
				if (from == 2 && to == 1) // stereo or dual mono to mono
				{
					StereoToMonoSignal(signal, interleaved);
					return;
				}
			}
		}
		else
		{
			throw std::runtime_error("Implement this.");
		}
	}

	constexpr inline void DelaySignal(std::vector<float>& signal, const size_t nrOfChannels, const size_t delayInFrames)
	{
		signal.insert(signal.end(), nrOfChannels * delayInFrames, 0.0f); // Pad signal with silence.
		std::move(signal.begin(), signal.end() - nrOfChannels * delayInFrames, signal.begin() + nrOfChannels * delayInFrames); // Move the signal over.
		std::fill(signal.begin(), signal.begin() + nrOfChannels * delayInFrames, 0.0f); // Write silence to the beginning of the signal.
	}

	inline float ComputeRootMeanSquareOfSignal(const std::vector<float>& signal)
	{
		float accumulated = 0.0f;
		for (size_t i = 0; i < signal.size(); i++)
		{
			accumulated += signal[i] * signal[i];
		}
		accumulated /= signal.size();
		return std::sqrtf(accumulated);
	}
}
