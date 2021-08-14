#pragma once

#include "Hash.h"

namespace sge
{
	template <typename Type>
	struct Resource
	{
		Hash hash = 0;
		Type resourceData = {};

		bool IsValid() const
		{
			return hash.IsValid() && resourceData.IsValid();
		}
	};

	template <typename Type>
	struct Handle
	{
		Hash hash = 0;
		Resource<Type>* ptr = nullptr;

		Type* operator->() const
		{
			assert(hash == ptr->hash);
			return &ptr->resourceData;
		}
		Type& operator*() const
		{
			assert(hash == ptr->hash);
			return ptr->resourceData;
		}
		bool operator==(const Handle<Type>& other) const
		{
			return hash == other.hash;
		}
		bool operator==(const Resource<Type>& resource) const
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