#include "Engine.h"

// TODO: make things private to prevent user from misuing the framework
// TODO: remove all direct pointers to vector elements, use indices instead!
// TODO: cout which element is invalid on IsValid calls

class Game final : public sge::I_Application
{
private:
	sge::UniqueResourceHandle<sge::StaticModel> model = {};
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
		sge::StaticModel::Definition modelDef = rm.GenerateDefinitionFrom(gltfHandle, meshDataToLoad, shadingModes);
		modelDef.transforms = std::vector<glm::mat4>(5, sge::IDENTITY_MAT4);
		model = renderer.CreateModel(modelDef);

		model->transforms->Translate(sge::DOWN_VEC3 * 20.0f, 0, 4);
		model->transforms->Translate(sge::EAST_VEC3 * 15.0f + sge::NORTH_VEC3 * 10.0f, 1, 1);
		model->transforms->Translate(sge::WEST_VEC3 * 15.0f + sge::NORTH_VEC3 * 10.0f, 2, 2);
		model->transforms->Translate(sge::EAST_VEC3 * 15.0f - sge::NORTH_VEC3 * 10.0f, 3, 3);
		model->transforms->Translate(sge::WEST_VEC3 * 15.0f - sge::NORTH_VEC3 * 10.0f, 4, 4);
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