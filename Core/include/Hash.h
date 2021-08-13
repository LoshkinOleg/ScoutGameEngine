#pragma once

using uint64_t = unsigned long long;
using uint32_t = unsigned int;

namespace sge
{
	struct Hash
	{
		uint64_t value = 0;

		Hash(const void* const data, const uint32_t byteLen, const Hash accumulatedHash);
		Hash(const uint64_t value): value(value) {}
		void Generate(const void* const data, const uint32_t byteLen);
		void Accumulate(const Hash other);
		inline bool operator==(const Hash other) const
		{
			return value == other.value;
		}
		inline bool operator==(const Hash& other) const
		{
			return value == other.value;
		}
	};
}//!sge