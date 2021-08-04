#include <iostream>
#include <string>

#include <glm/glm.hpp>

#include "Engine.h"
#include "macros.h"

class Game final : public sge::I_Application
{
public:
	void Init() override
	{
		auto& rm = sge::Engine::Get().GetResourceManager();

		auto handle = rm.LoadShader("../data/shaders/floor.vert", "../data/shaders/floor.frag");
		sge_MESSAGE(handle->vertexShader);
		rm.FreeShader(handle);
	}
	void Update() override
	{

	}
	void Shutdown() override
	{
		
	}
};

int main(int argv, char** args)
{
	Game game;
	sge::Engine& engine = sge::Engine::Get();
	engine.Run(&game);
	return EXIT_SUCCESS;
}