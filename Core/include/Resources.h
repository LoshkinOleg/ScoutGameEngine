#pragma once

#include <cstdint>

namespace sge
{
	struct I_Validateable
	{
		virtual bool IsValid() const = 0;
	};

	struct Hash: public I_Validateable
	{
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

		bool IsValid() const override;
	};

	template <typename Type>
	struct UniqueResource: public I_Validateable
	{
		Hash hash = 0;
		Type resourceData = {};

		bool IsValid() const override;
	};

	template <typename Type>
	struct HashlessResource: public I_Validateable
	{
		Type resourceData = {};

		bool IsValid() const override;
	};

	template <typename Type>
	struct UniqueResourceHandle: public I_Validateable
	{
		Hash hash = 0;
		UniqueResource<Type>* ptr = nullptr;

		Type* operator->() const;
		Type& operator*() const;
		bool operator==(const UniqueResourceHandle<Type>& other) const;
		bool operator==(const UniqueResource<Type>& resource) const;

		bool IsValid() const override;
	};

	template <typename Type>
	struct HashlessHandle: public I_Validateable
	{
		HashlessResource<Type>* ptr = nullptr;

		inline Type* operator->() const
		{
			return &ptr->resourceData;
		}
		inline Type& operator*() const
		{
			return ptr->resourceData;
		}

		bool IsValid() const override;
	};
}//!sge