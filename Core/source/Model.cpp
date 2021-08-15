#include "Model.h"

#include "Engine.h"

namespace sge
{
	bool Model::Definition::IsValid() const
	{
		bool returnVal = transforms.size() > 0;
		for(const auto& def : meshDefs)
		{
			returnVal &= def.IsValid();
		}
		return returnVal;
	}
	Hash Model::Definition::ComputeHash() const
	{
		Hash hash = 0;
		for(const auto& element : meshDefs)
		{
			hash.Accumulate(element.ComputeHash());
		}
		return hash;
	}
	bool Model::IsValid() const
	{
		bool returnVal = transforms.IsValid();
		for(const auto& mesh : indexedMeshes)
		{
			returnVal = returnVal && mesh->IsValid();
		}
		return returnVal;
	}

	void Model::Init_(const Definition& def)
	{
		auto& rm = Engine::Get().GetResourceManager();
		auto& renderer = Engine::Get().GetRenderer();
		transforms = rm.AllocateTransforms(def.transforms.data(), (uint32_t)(sizeof(glm::mat4) * def.transforms.size()));
		for(const auto& meshDef : def.meshDefs)
		{
			indexedMeshes.push_back(renderer.CreateMesh(meshDef));
		}
	}
	void Model::Draw_(const uint32_t primitive, const ShadingMode mode) const
	{
		assert(IsValid());
		for(const auto& mesh : indexedMeshes)
		{
			mesh->Draw_(transforms.NrOfTransforms(), primitive, mode);
		}
	}
}//!sge