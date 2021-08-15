#include "Engine.h"

class Game final : public sge::I_Application
{
private:
	sge::Handle<sge::Model> model = {};
public:
	void Init() override
	{
		auto& rm = sge::Engine::Get().GetResourceManager();
		auto& renderer = sge::Engine::Get().GetRenderer();

		auto gltfHandle = rm.LoadGltf("../data/gltf/complexScene.gltf");
		const auto shadingModes = (sge::ShadingMode)
			((uint32_t)sge::ShadingMode::ALBEDO_ONLY |
			 (uint32_t)sge::ShadingMode::BLINN_PHONG_NORMALMAPPED |
			 (uint32_t)sge::ShadingMode::GOOCH);
		model = renderer.CreateModel(rm.GenerateDefinitionFrom(gltfHandle, sge::GltfData::GltfAttributes::EVERYTHING, shadingModes));
	}
	void Update() override
	{
		sge::Engine::Get().GetRenderer().Schedule(model, sge::Primitive::TRIANGLES, sge::ShadingMode::BLINN_PHONG_NORMALMAPPED);
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