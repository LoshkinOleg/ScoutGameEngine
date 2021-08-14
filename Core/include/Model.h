#pragma once

#include <vector>
#include <utility>

#include "Mesh.h"
#include "Shader.h"
#include "Engine.h"

namespace sge
{
	struct Model
	{
		struct Definition
		{
			std::vector<glm::mat4> transforms = {};
			std::vector<IndexedMesh::Definition> meshDefs = {};

			bool IsValid() const
			{
				bool returnVal = transforms.size() > 0;
				for(const auto& def : meshDefs)
				{
					returnVal &= def.IsValid();
				}
				return returnVal;
			}
		};

		Handle<TransformsBuffer> transforms = {};
		std::vector<Handle<IndexedMesh>> indexedMeshes = {};

		void Init(const Definition& def)
		{

		}
		void Update() const
		{

		}
		void Draw(const Handle<Shader>& shader) const
		{
			assert(IsValid());
			for(const auto& mesh : indexedMeshes)
			{
				mesh->Draw(shader, transforms->NrOfTransforms());
			}
		}

		bool IsValid() const
		{
			bool returnVal = transforms->IsValid();
			for(const auto& mesh : indexedMeshes)
			{
				returnVal = returnVal && mesh->IsValid();
			}
			return returnVal;
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge