#include "Resources.h"

#include <assert.h>
#include <string>

#include <xxhash.h>

namespace sge
{
	Hash::Hash(const void* const data, const uint32_t byteLen, const Hash accumulatedHash)
	{
		assert(data && byteLen);
		Generate(data, byteLen);
		Accumulate(accumulatedHash);
	}
	bool Hash::operator==(const Hash other) const
	{
		return value == other.value;
	}
	bool Hash::operator==(const uint64_t other) const
	{
		return value == other;
	}
	bool Hash::operator>(const uint64_t other) const
	{
		return value > other;
	}
	bool Hash::operator>(const Hash other) const
	{
		return value > other.value;
	}
	bool Hash::operator<(const uint64_t other) const
	{
		return value < other;
	}
	bool Hash::operator<(const Hash other) const
	{
		return value < other.value;
	}
	bool Hash::operator>=(const uint64_t other) const
	{
		return value >= other;
	}
	bool Hash::operator>=(const Hash other) const
	{
		return value >= other.value;
	}
	bool Hash::operator<=(const uint64_t other) const
	{
		return value <= other;
	}
	bool Hash::operator<=(const Hash other) const
	{
		return value <= other.value;
	}
	bool Hash::operator&(const uint64_t other) const
	{
		return (bool)value & (bool)other;
	}
	bool Hash::operator&=(const Hash other) const
	{
		return (bool)value & (bool)other.value;
	}
	bool Hash::operator&=(const uint64_t other) const
	{
		return (bool)value & (bool)other;
	}
	bool Hash::operator&(const Hash other) const
	{
		return (bool)value & (bool)other.value;
	}
	bool Hash::operator&&(const bool other) const
	{
		return (bool)value && other;
	}
	bool Hash::operator&&(const Hash other) const
	{
		return (bool)value && (bool)other.value;
	}
	bool Hash::operator&&(const uint64_t other) const
	{
		return (bool)value && (bool)other;
	}
	bool Hash::IsValid() const
	{
		const bool returnVal = value > 0;
		assert(returnVal);
		return returnVal;
	}
	void Hash::Generate(const void* const data, const uint32_t byteLen)
	{
		assert(data && byteLen);
		value = XXH64(data, (size_t)byteLen, HASHING_SEED_);
	}
	void Hash::Accumulate(const Hash other)
	{
		assert(other.IsValid());
		std::string accumulatedData = std::to_string(value);
		accumulatedData += std::to_string(other.value);
		value = XXH64(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED_);
	}
	void Hash::Accumulate(const void* const data, const uint32_t byteLen)
	{
		assert(data && byteLen);
		std::string accumulatedData = std::to_string(value);
		accumulatedData += std::to_string(XXH64(data, (size_t)byteLen, HASHING_SEED_));
		value = XXH64(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED_);
	}

	template<typename Type>
	bool UniqueResource<Type>::IsValid() const
	{
		bool returnVal = hash.IsValid();
		returnVal &= resourceData.IsValid();
		assert(returnVal);
		return returnVal;
	}

	template<typename Type>
	bool HashlessResource<Type>::IsValid() const
	{
		const bool returnVal = resourceData.IsValid());
		assert(returnVal);
		return returnVal;
	}

	template<typename Type>
	Type* UniqueResourceHandle<Type>::operator->() const
	{
		assert(hash == ptr->hash);
		return &ptr->resourceData;
	}
	template<typename Type>
	Type& UniqueResourceHandle<Type>::operator*() const
	{
		assert(hash == ptr->hash);
		return ptr->resourceData;
	}
	template<typename Type>
	bool UniqueResourceHandle<Type>::operator==(const UniqueResourceHandle<Type>& other) const
	{
		return hash == other.hash;
	}
	template<typename Type>
	bool UniqueResourceHandle<Type>::operator==(const UniqueResource<Type>& resource) const
	{
		return hash == resource.hash;
	}
	template<typename Type>
	bool UniqueResourceHandle<Type>::IsValid() const
	{
		bool returnVal = hash.IsValid();
		returnVal &= ptr->IsValid();
		assert(returnVal);
		returnVal &= ptr != nullptr;
		assert(returnVal);
		returnVal &= hash == ptr->hash;
		assert(returnVal);
		return returnVal;
	}

	template<typename Type>
	bool HashlessHandle<Type>::IsValid() const
	{
		return ptr != nullptr && ptr->IsValid();

		bool returnVal = ptr != nullptr;
		assert(returnVal);
		returnVal &= ptr->IsValid();
		assert(returnVal);
		return returnVal;
	}
}//!sge