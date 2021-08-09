#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Engine.h"
#include "globals.h"

class Game final : public sge::I_Application
{
private:
	sge::ModelHandle model;
	sge::ShaderHandle shader;
public:
	void Init() override
	{
		auto& rm = sge::Engine::Get().GetResourceManager();
		auto& renderer = sge::Engine::Get().GetRenderer();

		auto gltfHandle = rm.LoadGltf("../data/gltfs/brickCube.gltf");
		model = renderer.CreateModel(gltfHandle, { glm::translate(sge::IDENTITY_MAT4, sge::DOWN_VEC3 * 5.0f) });
		rm.FreeGltf(gltfHandle);

		auto shaderSrcHandle = rm.LoadShader("../data/shaders/blinnPhongNormalmapped.vert", "../data/shaders/blinnPhongNormalmapped.frag");
		shader = renderer.CreateShader(shaderSrcHandle);
		rm.FreeShader(shaderSrcHandle);
	}
	void Update() override
	{
		for (glm::mat4* it = model->transformsBegin; it < model->transformsEnd; it++)
		{
			*it = glm::rotate(*it, 0.01f, glm::normalize(sge::EAST_VEC3 + sge::NORTH_VEC3));
		}

		sge::Engine::Get().GetRenderer().Schedule(model, shader, GL_TRIANGLES);
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