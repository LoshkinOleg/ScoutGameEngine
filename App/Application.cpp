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
			((uint32_t)sge::ShadingMode::GOOCH);
		const auto meshDataToLoad = (sge::GltfData::GltfAttributes)
			((uint32_t)sge::GltfData::GltfAttributes::POSITIONS |
			(uint32_t)sge::GltfData::GltfAttributes::INDICES |
			(uint32_t)sge::GltfData::GltfAttributes::NORMALS);
		sge::Model::Definition modelDef = rm.GenerateDefinitionFrom(gltfHandle, meshDataToLoad, shadingModes);

		modelDef.transforms.back()[3] = glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
		modelDef.transforms.back() = glm::rotate(modelDef.transforms.back(), 3.14f, sge::EAST_VEC3);
		
		model = renderer.CreateModel(modelDef);
	}
	void Update() override
	{
		sge::Engine::Get().GetRenderer().Schedule(model, sge::Primitive::TRIANGLES, sge::ShadingMode::GOOCH);
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