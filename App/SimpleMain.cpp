#include <string>
#include <string_view>
#include <assert.h>

int main(int argv, char** args)
{
	auto empty = "";
	auto null = "\0";
	assert(empty == null);

	return 0;
}