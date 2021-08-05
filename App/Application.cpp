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
	sge::Texture2dHandle texture;
public:
	void Init() override
	{
		auto& rm = sge::Engine::Get().GetResourceManager();
		auto& renderer = sge::Engine::Get().GetRenderer();

		const auto vertices = std::vector<float>(sge::QUAD_2D_INDEXED.begin(), sge::QUAD_2D_INDEXED.end());
		const auto indices = std::vector<uint32_t>(sge::QUAD_INDICES.begin(), sge::QUAD_INDICES.end());
		const std::vector<uint32_t> layout = {2};
		buffer = renderer.CreateVertexBuffer(vertices, indices, layout);

		auto shaderSrcHandle = rm.LoadShader("../data/shaders/quad.vert", "../data/shaders/quad.frag");
		shader = renderer.CreateShader(shaderSrcHandle);
		rm.FreeShader(shaderSrcHandle);

		auto ktxHandle = rm.LoadKtx("../data/ktxs/smiley.ktx");
		texture = renderer.CreateTexture2d(ktxHandle);
		rm.FreeKtx(ktxHandle);

		shader->SetInt("albedo", 0);
	}
	void Update() override
	{
		sge::Engine::Get().GetRenderer().ScheduleToBeDrawn(buffer, shader, {texture}, 1, GL_TRIANGLES);
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