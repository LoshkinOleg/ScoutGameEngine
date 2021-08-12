#include <concepts>
#include <array>
#include <assert.h>
#include <vector>

using uint = unsigned int;

struct TextureData
{
	uint TEX = 0;
};

enum BufferDataMutability
{
	STATIC = 1,
	DYNAMIC = 2
};

struct BufferData
{
	BufferDataMutability mutability = BufferDataMutability::STATIC;
	uint VBO = 0;
};

enum RenderMode
{
	TEXTURE_ONLY,
	GOOCH,
	BLINN_PHONG
};

struct MeshData
{
	RenderMode renderMode = RenderMode::BLINN_PHONG;
	std::vector<BufferData> buffers = {};
};

int main(int argv, char** args)
{
	return 0;
}