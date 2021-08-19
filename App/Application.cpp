#include "Engine.h"

// TODO: make things private to prevent user from misuing the framework
// TODO: remove all direct pointers to vector elements, use indices instead!
// TODO: cout which element is invalid on IsValid calls

class Game final : public sge::I_Application
{
private:
	sge::UniqueResourceHandle<sge::Model> model = {};
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
		modelDef.transforms = std::vector<glm::mat4>(3, sge::IDENTITY_MAT4);
		model = renderer.CreateModel(modelDef);

		model->transforms->Translate(sge::DOWN_VEC3 * 20.0f + sge::SOUTH_VEC3 * 13.0f, 0, 0);
		model->transforms->Translate(sge::DOWN_VEC3 * 20.0f + sge::EAST_VEC3 * 15.0f + sge::NORTH_VEC3 * 10.0f, 1, 1);
		model->transforms->Translate(sge::DOWN_VEC3 * 20.0f + sge::WEST_VEC3 * 15.0f + sge::NORTH_VEC3 * 10.0f, 2, 2);
	}
	void Update() override
	{
		const float timer = sge::Engine::Get().GetCurrentTimer();

		model->transforms->Rotate(0.01f, sge::NORTH_VEC3, 0, 0);
		model->transforms->Rotate(0.01f, sge::EAST_VEC3, 1, 1);
		model->transforms->Rotate(0.01f, sge::UP_VEC3, 2, 2);
		model->indexedMeshes[0]->materials[sge::ShadingMode::GOOCH]->vec3s[0] = glm::vec3(glm::cos(timer), glm::sin(timer), glm::cos(timer));
		model->indexedMeshes[1]->materials[sge::ShadingMode::GOOCH]->vec3s[0] = glm::vec3(glm::cos(timer + sge::PI), glm::sin(timer), glm::cos(timer));
		model->indexedMeshes[2]->materials[sge::ShadingMode::GOOCH]->vec3s[0] = glm::vec3(glm::cos(timer), glm::sin(timer +sge::PI), glm::cos(timer));
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