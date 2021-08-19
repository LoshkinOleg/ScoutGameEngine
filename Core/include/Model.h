#pragma once

#include "Mesh.h"
#include "Transform.h"

namespace sge
{
	struct Model
	{
		struct Definition
		{
			std::vector<glm::mat4> transforms = {};
			std::vector<IndexedMesh::Definition> meshDefs = {};

			bool IsValid() const;

			Hash ComputeHash() const;
		};

		HashlessResourceHandle<TransformsBuffer> transforms = {};
		std::vector<UniqueResourceHandle<IndexedMesh>> indexedMeshes = {};

		bool IsValid() const;
		inline void Reset()
		{
			*this = {};
		}

	private:
		friend class Renderer;
		void Init_(const Definition& def);
		void Draw_(const uint32_t primitive, const ShadingMode mode) const;
	};
}//!sge