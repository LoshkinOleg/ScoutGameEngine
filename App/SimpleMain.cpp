#include <assert.h>
#include <iostream>
#include <string>
#include <iterator>
#include <memory>
#define XXH_INLINE_ALL
#include <xxhash.h>

#include <concepts>

void* operator new(std::size_t size)
{
	assert(size > 0);
	void* returnVal = std::malloc(size);
	if(returnVal)
	{
		std::cout << "Allocating " << size << "bytes at: " << returnVal << std::endl;
		return returnVal;
	}
	else
	{
		throw std::bad_alloc{};
	}
}
void operator delete(void* ptr) noexcept
{
	std::cout << "Deallocating " << ptr << std::endl;
	std::free(ptr);
}

using Hash_T = size_t;
using Idx_T = size_t;

template<class Data_T, class Container_T>
class Handle
{
public:
	Container_T* containerPtr = nullptr;
	Idx_T idx = -1;
	Hash_T hash = 0;

	Data_T& operator*()
	{
		assert(idx < -1);
		auto resource = containerPtr->At(idx);
		assert(hash == resource->hash);
		return *resource->data;
	}
	Data_T* operator->()
	{
		assert(idx < -1);
		auto resource = containerPtr->At(idx);
		assert(hash == resource->hash);
		return resource->data;
	}
};

template<class Array>
class Iterator
{
public:
	using value_type = Array::value_type;
	using difference_type = std::ptrdiff_t;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using self = Iterator;

	pointer ptr = nullptr;

	Iterator(pointer ptr): ptr(ptr) {}
	pointer operator->()
	{
		return ptr;
	}
	reference operator*()
	{
		return *ptr;
	}
	bool operator==(const self& rhs)
	{
		return ptr == rhs.ptr;
	}
	bool operator!=(const self& rhs)
	{
		return ptr != rhs.ptr;
	}
	self operator++()
	{
		self i = *this;
		ptr++;
		return i;
	}
	self operator++(int)
	{
		ptr++;
		return *this;
	}
};

template<class Data_T, size_t nrOfElements>
class Array
{
	Data_T* data_ = nullptr; // 8 bytes
	Hash_T* hashes_ = nullptr; // 8 bytes
	Idx_T current_ = 0; // 8 bytes

public:
	// using value_type = Data_T;
	// using difference_type = std::ptrdiff_t;
	// using pointer = value_type*;
	// using const_pointer = const value_type*;
	// using reference = value_type&;
	// using iterator = Iterator<Array>;
	using Iterator_T = Iterator<Array>;
	using Handle_T = Handle<Data_T, Array<Data_T, nrOfElements>>;

	Iterator_T begin() { return Iterator_T{ data_ }; }
	Iterator_T end() { return Iterator_T{ data_ + current_ }; }

	Array()
	{
		assert(nrOfElements > 0);
		data_ = new Data_T[nrOfElements];
		hashes_ = new Hash_T[nrOfElements];
	}
	~Array()
	{
		assert(data_ && hashes_);
		delete[] data_;
		delete[] hashes_;
	}
	Array(Array&&) = delete;
	Array(Array&) = delete;
	Array& operator=(Array&&) = delete;
	Array& operator=(Array&) = delete;

	Handle_T Back()
	{
		assert(data_ && hashes_);
		assert(current_ < nrOfElements);
		Resource_T returnVal = Resource_T{ data_ + current_, hashes_ + current_ };
		return returnVal;
	}
	Handle_T Last()
	{
		assert(data_ && hashes_);
		assert(current_ > 0);
		Resource_T returnVal = Resource_T{ data_ + current_ - 1, hashes_ + current_ - 1 };
		return returnVal;
	}
	Handle_T At(const Idx_T position)
	{
		assert(data_ && hashes_);
		assert(current_ > 0);
		assert(position < current_);
		Resource_T returnVal = Resource_T{ data_ + position, hashes_ + position };
		return returnVal;
	}
	// Data_T& GetData(const Hash_T hash)
	// {
	// 	assert(data_ && hashes_);
	// 	assert(current_ > 0);
	// 	const size_t len = current_;
	// 	for(size_t i = 0; i < len; i++)
	// 	{
	// 		const Hash_T element = *(hashes_ + i);
	// 		if(hash == element)
	// 		{
	// 			return *(datas_ + i);
	// 		}
	// 	}
	// 	throw;
	// }

	// Handle_T GenerateHandle(const Idx_T idx)
	// {
	// 	assert(data_ && hashes_);
	// 	assert(current_ > 0);
	// 	assert(idx < current_);
	// 
	// 	Handle_T returnVal;
	// 	returnVal.containerPtr = this;
	// 	returnVal.hash = hashes_ + idx;
	// 	returnVal.idx = idx;
	// 	return returnVal;
	// }

	size_t Size() const
	{
		return current_;
	}
	bool Exists(const Hash_T hash) const
	{
		assert(data_ && hashes_);
		assert(current_ > 0);
		for(size_t i = 0; i < current_; i++)
		{
			const Hash_T element = *(hashes_ + i);
			if(hash == element)
			{
				return true;
			}
		}
		return false;
	}
};

class Data
{
public:
	size_t i = 0;

	bool IsValid() const
	{
		return i > 0;
	}
};

int main(int argv, char** args)
{
	Array<Data, 4> arr;
	for(size_t i = 0; i < 4; i++)
	{
		auto resource = arr.Back();
		resource.data->i = i + 1;
		resource.hash
		resource.data = Data{i + 1};
	}
}