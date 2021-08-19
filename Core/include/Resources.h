#pragma once

#include "Hash.h"

namespace sge
{
	template <typename Type>
	struct UniqueResource
	{
		Hash hash = 0;
		Type resourceData = {};

		inline bool IsValid() const
		{
			return hash.IsValid() && resourceData.IsValid();
		}
	};

	template <typename Type>
	struct HashlessResource
	{
		Type resourceData = {};

		inline bool IsValid() const
		{
			return resourceData.IsValid();
		}
	};

	template <typename Type>
	struct UniqueResourceHandle
	{
		Hash hash = 0;
		UniqueResource<Type>* ptr = nullptr;

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
		inline bool operator==(const UniqueResourceHandle<Type>& other) const
		{
			return hash == other.hash;
		}
		inline bool operator==(const UniqueResource<Type>& resource) const
		{
			return hash == resource.hash;
		}

		inline bool IsValid() const
		{
			return hash.IsValid() && ptr != nullptr && ptr->IsValid() && hash == ptr->hash;
		}
		inline void Reset()
		{
			*this = {};
		}
	};

	template <typename Type>
	struct HashlessResourceHandle
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

		inline bool IsValid() const
		{
			return ptr != nullptr && ptr->IsValid();
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge