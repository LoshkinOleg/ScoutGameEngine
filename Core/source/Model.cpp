#include "Model.h"

#include <glad/glad.h>

#include "Engine.h"

namespace sge
{
	bool StaticModel::Definition::IsValid() const
	{
		bool returnVal = transforms.size() > 0;
		assert(returnVal);
		for(const auto& def : meshDefs)
		{
			returnVal &= def.IsValid();
			assert(returnVal);
		}
		return returnVal;
	}
	bool DynamicModel::Definition::IsValid() const
	{
		bool returnVal = transform != glm::mat4(0.0f);
		assert(returnVal);
		returnVal &= meshDef.IsValid();
		assert(returnVal);
		return returnVal;
	}
	Hash StaticModel::Definition::ComputeHash() const
	{
		Hash hash = 0;
		for(const auto& element : meshDefs)
		{
			hash.Accumulate(element.ComputeHash());
		}
		return hash;
	}
	bool StaticModel::IsValid() const
	{
		bool returnVal = transforms.IsValid();
		assert(returnVal);
		for(const auto& mesh : indexedMeshes)
		{
			returnVal = returnVal && mesh->IsValid();
			assert(returnVal);
		}
		return returnVal;
	}
	bool DynamicModel::IsValid() const
	{
		bool returnVal = transform.IsValid();
		assert(returnVal);
		returnVal &= mesh->IsValid();
		assert(returnVal);
		return returnVal;
	}

	void StaticModel::Init_(const Definition& def)
	{
		auto& rm = Engine::Get().GetResourceManager();
		auto& renderer = Engine::Get().GetRenderer();
		for(const auto& meshDef : def.meshDefs)
		{
			indexedMeshes.push_back(renderer.CreateIndexedMesh(meshDef));
		}
		transforms = rm.CreateTransformsBuffer(def.transforms);
	}
	void StaticModel::Draw_(const uint32_t primitive, const ShadingMode mode) const
	{
		assert(IsValid());
		assert(transforms.IsValid());
		for(const auto& mesh : indexedMeshes)
		{
			mesh->Draw_(transforms, primitive, mode);
		}
	}

	void DynamicModel::Init_(const Definition& def)
	{
		auto& rm = Engine::Get().GetResourceManager();
		auto& renderer = Engine::Get().GetRenderer();
		mesh = renderer.CreateInterlacedMesh(def.meshDef);
		transform = rm.CreateTransformsBuffer(std::vector<glm::mat4>(1, def.transform));
	}
	void DynamicModel::Draw_(const uint32_t primitive, const ShadingMode mode) const
	{
		assert(IsValid());
		assert(transform.IsValid());
		mesh->Draw_(transform, primitive, mode);
	}
}//!sge