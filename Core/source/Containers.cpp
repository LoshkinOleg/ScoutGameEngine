#include "Containers.h"

#include <assert.h>

#include "Resources.h"
#include "macros.h"

namespace sge
{
	template<typename T, size_t nrOfElements>
	StaticHashlessVector<T, nrOfElements>::StaticHashlessVector()
	{
		begin_ = new T[nrOfElements];
	}
	template<typename T, size_t nrOfElements>
	StaticHashlessVector<T, nrOfElements>::~StaticHashlessVector()
	{
		sge_WARNING("Deallocating a StaticHashlessVector, this should only happen at the end of the program.");
		delete[] begin_;
	}

	template<typename T, size_t nrOfElements>
	void StaticHashlessVector<T, nrOfElements>::Push(const T & newElement)
	{
		assert(current_ < nrOfElements);
		begin_[current_++] = newElement;
	}
	template<typename T, size_t nrOfElements>
	void StaticHashlessVector<T, nrOfElements>::Pop()
	{
		sge_ERROR("You're not supposed to remove elements from this container as it will move it's data and mess up pointers.");

		assert(current_ > 0);
		current_--;
	}
	template<typename T, size_t nrOfElements>
	void StaticHashlessVector<T, nrOfElements>::Remove(const uint32_t position)
	{
		sge_ERROR("You're not supposed to remove elements from this container as it will move it's data and mess up pointers.");

		assert(position <= current_);
		if(position == current_ - 1)
		{
			Pop();
			return;
		}
		else
		{
			memcpy(begin_ + position, begin_ + position + 1, (current_ - position - 1) * sizeof(T));
			current_--;
		}
	}
	template<typename T, size_t nrOfElements>
	T& StaticHashlessVector<T, nrOfElements>::Last()
	{
		return *(begin_ + current_);
	}
	template<typename T, size_t nrOfElements>
	T& StaticHashlessVector<T, nrOfElements>::At(const uint32_t position)
	{
		assert(position < current_);
		return *(begin_ + position);
	}
	template<typename T, size_t nrOfElements>
	size_t StaticHashlessVector<T, nrOfElements>::Size() const
	{
		return current_;
	}
	template<typename T, size_t nrOfElements>
	size_t StaticHashlessVector<T, nrOfElements>::Capacity() const
	{
		return nrOfElements;
	}

	template<typename T, size_t nrOfElements>
	StaticHashableVector<T, nrOfElements>::StaticHashableVector()
	{
		assert(nrOfElements > 0);
		begin_ = new T[nrOfElements];
	}
	template<typename T, size_t nrOfElements>
	StaticHashableVector<T, nrOfElements>::~StaticHashableVector()
	{
		sge_WARNING("Deallocating a StaticHashableVector, this should only happen at the end of the program.");
		delete[] begin_;
	}

	template<typename T, size_t nrOfElements>
	void StaticHashableVector<T, nrOfElements>::PushCopy(const T& newElement)
	{
		assert(current_ < nrOfElements);
		begin_[current_++] = newElement;
	}
	template<typename T, size_t nrOfElements>
	void StaticHashableVector<T, nrOfElements>::Pop()
	{
		sge_ERROR("You're not supposed to remove elements from this container as it will move it's data and mess up pointers.");

		assert(current_ > 0);
		current_--;
	}
	template<typename T, size_t nrOfElements>
	void StaticHashableVector<T, nrOfElements>::Remove(const uint32_t position)
	{
		sge_ERROR("You're not supposed to remove elements from this container as it will move it's data and mess up pointers.");

		assert(position <= current_);
		if(position == current_ - 1)
		{
			Pop();
			return;
		}
		else
		{
			memcpy(begin_ + position, begin_ + position + 1, (current_ - position - 1) * sizeof(T));
			current_--;
		}
	}
	template<typename T, size_t nrOfElements>
	T& StaticHashableVector<T, nrOfElements>::Last()
	{
		T& returnVal = *(begin_ + current_);
		assert(returnVal.IsValid());
		return returnVal;
	}
	template<typename T, size_t nrOfElements>
	T& StaticHashableVector<T, nrOfElements>::At(const uint32_t position)
	{
		assert(position < current_);
		T& returnVal = *(begin_ + position);
		assert(returnVal.IsValid());
		return returnVal;
	}
	template<typename T, size_t nrOfElements>
	T& StaticHashableVector<T, nrOfElements>::GetElement(const Hash& hash)
	{
		for(auto& element : *this)
		{
			if(element.hash == hash)
			{
				T& returnVal = element;
				assert(returnVal.IsValid());
				return returnVal;
			}
		}
		sge_ERROR("Element with the specified hash was not found in the list provided!");
	}
	template<typename T, size_t nrOfElements>
	size_t StaticHashableVector<T, nrOfElements>::Size() const
	{
		return current_;
	}
	template<typename T, size_t nrOfElements>
	size_t StaticHashableVector<T, nrOfElements>::Capacity() const
	{
		return nrOfElements;
	}
	template<typename T, size_t nrOfElements>
	bool StaticHashableVector<T, nrOfElements>::Exists(const Hash& hash) const
	{
		for(const auto& element : *this)
		{
			if(element.hash == hash)
			{
				return true;
			}
		}
		return false;
	}
}//!sge