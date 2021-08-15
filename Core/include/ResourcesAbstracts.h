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
			return hash.IsValid() && resourceData.IsValid();
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
			return hash && ptr && ptr->IsValid();
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge