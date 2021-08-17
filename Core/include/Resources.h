#pragma once

#include "Hash.h"

namespace sge
{
	template <typename Type>
	struct Resource
	{
		Hash hash = 0;
		Type resourceData = {};

		inline bool IsValid() const
		{
			// Note: not checking hash for validity since some resources aren't ment to use hashes.
			// TODO: make a hashless Resource and Handle structs.
			return resourceData.IsValid();
		}
	};

	template <typename Type>
	struct Handle
	{
		Hash hash = 0;
		Resource<Type>* ptr = nullptr;

		inline Type* operator->() const
		{
			assert(hash == ptr->hash);
			return &ptr->resourceData;
		}
		inline Type& operator*() const
		{
			assert(hash == ptr->hash);
			return ptr->resourceData;
		}
		inline bool operator==(const Handle<Type>& other) const
		{
			return hash == other.hash;
		}
		inline bool operator==(const Resource<Type>& resource) const
		{
			return hash == resource.hash;
		}

		inline bool IsValid() const
		{
			// Note: hash = 0, ptr->hash = 0 is valid for resources that are supposed to have duplicates (like transforms).
			return ptr != nullptr && ptr->IsValid() && hash == ptr->hash;
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge