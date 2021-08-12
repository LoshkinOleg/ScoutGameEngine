#include "Hash.h"

#include <string>

#include <xxHash.h>

namespace sge
{
	constexpr const uint64_t HASHING_SEED_ = 0x1337;

	void Hash::Generate(void* data, const uint32_t byteLen)
	{
		value = XXH64(data, (size_t)byteLen, HASHING_SEED_);
	}
	void Hash::Accumulate(const Hash other)
	{
		std::string accumulatedData = std::to_string(value);
		accumulatedData += std::to_string(other.value);
		value = XXH64(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED_);
	}
}//!sge