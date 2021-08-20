#pragma once

namespace sge
{
	using uint32_t = unsigned int;
	class Hash;

	template<typename StaticVector>
	class StaticVectorIterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = StaticVector::value_type;
		using pointer = value_type*;
		using reference = value_type&;

		StaticHashlessVectorIterator(PointerType ptr): ptr_(ptr) {};

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }
		Iterator& operator++() { m_ptr++; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

	private:
		pointer m_ptr;
	};

	template<typename T, size_t nrOfElements>
	class StaticHashlessVector
	{
		T* begin_ = nullptr;
		uint32_t current_ = 0;

	public:
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using Iterator = StaticVectorIterator<StaticHashlessVector<T, nrOfElements>>;

		Iterator begin() { return Iterator{ begin_ }; }
		Iterator end() { return Iterator{ begin_ + current_}; }

		StaticHashlessVector(); // Note: why does vs wants to inline the definition of this?
		~StaticHashlessVector();

		void Push(const T& newElement);
		void Pop();
		void Remove(const uint32_t position);

		T& Last();
		T& At(const uint32_t position);

		size_t Size() const;
		size_t Capacity() const;
	};

	template<typename T, size_t nrOfElements>
	class StaticHashableVector
	{
		T* begin_ = nullptr;
		uint32_t current_ = 0;

	public:
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using Iterator = StaticVectorIterator<StaticHashableVector<T, nrOfElements>>;

		Iterator begin() { return Iterator{ begin_ }; }
		Iterator end() { return Iterator{ begin_ + current_ }; }

		StaticHashableVector();
		~StaticHashableVector();

		void Push(const T& newElement);
		void Pop();
		void Remove(const uint32_t position);

		T& Last();
		T& At(const uint32_t position);
		T& GetElement(const Hash& hash);

		size_t Size() const;
		size_t Capacity() const;
		bool Exists(const Hash& hash) const;
	};
}//!sge