#pragma once

#include <cstdint>

namespace sge
{
	class Hash
	{
	public:
		constexpr static const uint64_t HASHING_SEED_ = 0x1337;

		uint64_t value = 0;

		Hash(const void* const data, const uint32_t byteLen, const Hash accumulatedHash);
		Hash(const uint64_t value): value(value) {}
		void Generate(const void* const data, const uint32_t byteLen);
		void Accumulate(const Hash other);
		void Accumulate(const void* const data, const uint32_t byteLen);

		bool operator==(const Hash other) const;
		bool operator==(const uint64_t other) const;

		bool operator>(const Hash other) const;
		bool operator>(const uint64_t other) const;
		bool operator<(const Hash other) const;
		bool operator<(const uint64_t other) const;

		bool operator>=(const Hash other) const;
		bool operator>=(const uint64_t other) const;
		bool operator<=(const Hash other) const;
		bool operator<=(const uint64_t other) const;

		bool operator&(const Hash other) const;
		bool operator&(const uint64_t other) const;

		bool operator&=(const Hash other) const;
		bool operator&=(const uint64_t other) const;

		bool operator&&(const bool other) const;
		bool operator&&(const Hash other) const;
		bool operator&&(const uint64_t other) const;

		bool IsValid() const;
	};

	template <typename Type>
	class HashableResource
	{
	public:
		Hash hash = 0;
		Type resourceData = {};

		bool IsValid() const;
	};

	template <typename Type>
	class HashlessResource
	{
	public:
		Type resourceData = {};

		bool IsValid() const;
	};

	template <typename Type>
	class HashableHandle
	{
	public:
		Hash hash = 0;
		HashableResource<Type>* ptr = nullptr;

		Type* operator->() const;
		Type& operator*() const;
		bool operator==(const HashableHandle<Type>& other) const;
		bool operator==(const HashableResource<Type>& resource) const;

		bool IsValid() const;
	};

	template <typename Type>
	class HashlessHandle
	{
	public:
		HashlessResource<Type>* ptr = nullptr;

		inline Type* operator->() const
		{
			return &ptr->resourceData;
		}
		inline Type& operator*() const
		{
			return ptr->resourceData;
		}

		bool IsValid() const;
	};
}//!sge