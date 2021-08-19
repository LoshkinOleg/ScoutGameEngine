#include <iostream>
#include <string>
#include <assert.h>

using int32 = long;
using uint32 = unsigned long;
using int64 = long long;
using uint64 = unsigned long long;
using int8 = char;
using uint8 = unsigned char;

struct StackContainer
{
	uint8* data = nullptr;
	constexpr static const uint64 end = 5;
	uint64 current = 0;

	void Init()
	{
		data = new uint8[end];
		for(uint64 i = 0; i < end; i++)
		{
			data[i] = 'x';
		}
	}
	void Destroy()
	{
		delete[] data;
	}

	uint64 Push(const uint8 value)
	{
		assert(current < end);
		data[current++] = value;
		return current - 1;
	}
	void Pop()
	{
		assert(current > 0);
		data[--current] = 'x';
	}
	uint8 Read(const uint64 position) const
	{
		assert(position <= current);
		return data[position];
	}
	void Write(const uint8 value, const uint64 position)
	{
		assert(position <= current);
		data[position] = position;
	}

	void PrintCurrentContents() const
	{
		std::cout << "Current contents: \n";
		for(uint64 i = 0; i < current; i++)
		{
			std::cout << data[i];
		}
		std::cout << std::endl;
	}

	void PrintAllContents() const
	{
		std::cout << "All contents: \n";
		for(uint64 i = 0; i < end; i++)
		{
			std::cout << data[i];
		}
		std::cout << std::endl;
	}

	void Remove(uint64& position)
	{
		assert(position >= 0 && position < current);
		data[position] = 'x';
		if(position == current - 1)
		{
			Pop();
		}
		else
		{
			// Elements need to be moved.
			memcpy(data, data + position + 1, );
		}
		position = -1;
	}
};

const char* msg = "abcde";

int main(int argv, char** args)
{
	StackContainer stack;
	stack.Init();
	uint64 handleToA = stack.Push(msg[0]);
	uint64 handleToB = stack.Push(msg[1]);
	uint64 handleToC = stack.Push(msg[2]);
	uint64 handleToD = stack.Push(msg[3]);
	uint64 handleToE = stack.Push(msg[4]);

	stack.PrintCurrentContents();

	std::cout << "===========" << std::endl;
	std::cout << "Handle to a: " << std::to_string(handleToA) << std::endl;
	std::cout << "Handle to b: " << std::to_string(handleToB) << std::endl;
	std::cout << "Handle to c: " << std::to_string(handleToC) << std::endl;
	std::cout << "Handle to d: " << std::to_string(handleToD) << std::endl;
	std::cout << "Handle to e: " << std::to_string(handleToE) << std::endl;
	std::cout << "===========" << std::endl;

	stack.Remove(handleToC);
	std::cout << "Removed c." << std::endl;

	std::cout << "===========" << std::endl;
	std::cout << "Handle to a: " << std::to_string(handleToA) << std::endl;
	std::cout << "Handle to b: " << std::to_string(handleToB) << std::endl;
	std::cout << "Handle to c: " << std::to_string(handleToC) << std::endl;
	std::cout << "Handle to d: " << std::to_string(handleToD) << std::endl;
	std::cout << "Handle to e: " << std::to_string(handleToE) << std::endl;
	std::cout << "===========" << std::endl;

	stack.PrintCurrentContents();

	stack.Destroy();
	return 0;
}