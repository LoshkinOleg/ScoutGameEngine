#pragma once

#include <vector>
#include <utility>

#include "ResourceManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "globals.h"

namespace sge
{
	struct Model
	{
		Handle<ResourceManager::TransformsBuffer> transforms = {};
		// std::vector<std::pair<Handle<IndexedMesh>, Handle<Shader>>> indexedMeshes = {};

		// TODO: address the issue of having multiple shaders for different meshes and not updating them all excessively

		void Init()
		{

		}
		void Update() const
		{

		}
		void Draw(const glm::mat4& viewMatrix) const
		{
			indexedMeshes[0]->Draw(shader, transforms->NrOfTransforms(), viewMatrix, true);
			const uint32_t len = indexedMeshes.size();
			for(uint32_t i = 1; i < len; i++)
			{
				indexedMeshes[i]->Draw(shader, transforms->NrOfTransforms(), viewMatrix, false);
			}
		}
		void Destroy()
		{

		}

		bool IsValid() const
		{

		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge