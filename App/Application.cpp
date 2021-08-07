#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Engine.h"
#include "globals.h"

class Game final : public sge::I_Application
{
private:
	sge::VertexBufferHandle buffer;
	sge::ShaderHandle shader;
	glm::mat4 model = glm::translate(sge::IDENTITY_MAT4, sge::DOWN_VEC3 * 10.0f);
public:
	void Init() override
	{
		auto& rm = sge::Engine::Get().GetResourceManager();
		auto& renderer = sge::Engine::Get().GetRenderer();

		auto gltfHandle = rm.LoadGltf("../data/gltfs/complexScene.glb");
		buffer = renderer.CreateVertexBuffer(gltfHandle);
		rm.FreeGltf(gltfHandle);

		auto shaderSrcHandle = rm.LoadShader("../data/shaders/gooch.vert", "../data/shaders/gooch.frag");
		shader = renderer.CreateShader(shaderSrcHandle);
		rm.FreeShader(shaderSrcHandle);

		shader->SetMat4("cameraMatrix", sge::WINDOW_PROJECTION * sge::DEFAULT_VIEW_MATRIX);
		shader->SetVec3("viewPos", sge::ZERO_VEC3);
		shader->SetVec3("baseColor", sge::WHITE * 0.5f);
	}
	void Update() override
	{
		model = glm::rotate(model, 0.01f, sge::NORTH_VEC3);
		shader->SetMat4("modelMatrix", model);
		shader->SetMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		sge::Engine::Get().GetRenderer().ScheduleToBeDrawn(buffer, shader, {}, 1, GL_TRIANGLES);
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