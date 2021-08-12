#pragma once

using uint64_t = unsigned long long;
using uint32_t = unsigned int;

namespace sge
{
	struct Hash
	{
		uint64_t value = 0;

		void Generate(void* data, const uint32_t byteLen);
		void Accumulate(const Hash other);
	};
}//!sge