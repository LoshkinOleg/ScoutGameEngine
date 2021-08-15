#pragma once

#include <cstdint>

namespace sge
{
	struct Hash
	{
		uint64_t value = 0;

		Hash(const void* const data, const uint32_t byteLen, const Hash accumulatedHash);
		Hash(const uint64_t value): value(value) {}
		void Generate(const void* const data, const uint32_t byteLen);
		void Accumulate(const Hash other);
		void Accumulate(const void* const data, const uint32_t byteLen);
		inline bool operator==(const Hash other) const
		{
			return value == other.value;
		}
		inline bool operator==(const uint64_t other) const
		{
			return value == other;
		}
		inline bool operator>(const uint64_t other) const
		{
			return value > other;
		}
		inline bool operator>(const Hash other) const
		{
			return value > other.value;
		}
		inline bool operator<(const uint64_t other) const
		{
			return value < other;
		}
		inline bool operator<(const Hash other) const
		{
			return value < other.value;
		}
		inline bool operator>=(const uint64_t other) const
		{
			return value >= other;
		}
		inline bool operator>=(const Hash other) const
		{
			return value >= other.value;
		}
		inline bool operator<=(const uint64_t other) const
		{
			return value <= other;
		}
		inline bool operator<=(const Hash other) const
		{
			return value <= other.value;
		}
		inline bool operator&(const uint64_t other) const
		{
			return value & other;
		}
		inline bool operator&(const Hash other) const
		{
			return value & other.value;
		}
		inline bool operator&&(const bool other) const
		{
			return value && other;
		}
		inline bool IsValid() const
		{
			return bool(value);
		}
	};
}//!sge