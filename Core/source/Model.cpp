#include "Model.h"

#include <glad/glad.h>

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
		for(const auto& meshDef : def.meshDefs)
		{
			indexedMeshes.push_back(renderer.CreateMesh(meshDef));
		}
		transforms.Init_(def.transforms);
	}
	void Model::Draw_(const uint32_t primitive, const ShadingMode mode) const
	{
		assert(IsValid());
		assert(transforms.IsValid());
		for(const auto& mesh : indexedMeshes)
		{
			transforms.Bind(mesh->VAO, mesh->vertexBuffers.size()); // iModelMatrix always comes after all the per-vertex attributes.
			mesh->Draw_(transforms.NrOfTransforms(), primitive, mode);
		}
	}
}//!sge