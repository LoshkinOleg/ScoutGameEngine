#pragma once

#include "Mesh.h"
#include "Transform.h"

namespace sge
{
	class Model: public I_Validateable
	{
		friend class Renderer;

	public:
		class Definition: public I_Validateable
		{
		public:
			std::vector<glm::mat4> transforms = {};
			std::vector<IndexedMesh::Definition> indexedMeshDefs = {};
			std::vector<InterlacedMesh::Definition> interlacedMeshDefs = {};

			Hash ComputeHash() const;

			bool IsValid() const override;
		};

		HashlessHandle<TransformsBuffer> transforms = {};
		std::vector<UniqueResourceHandle<IndexedMesh>> indexedMeshes = {};
		std::vector<UniqueResourceHandle<IndexedMesh>> interlacedMeshes = {};

		bool IsValid() const override;

	private:
		void Init_(const Definition& def);
		void Draw_(const uint32_t primitive, const ShadingMode mode) const;
	};
}//!sge