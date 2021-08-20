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

		bool IsValid() const override;

	private:
		HashlessHandle<TransformsBuffer> transforms_ = {};
		std::vector<HashableHandle<IndexedMesh>> indexedMeshes_ = {};
		std::vector<HashableHandle<InterlacedMesh>> interlacedMeshes_ = {};

		void Init_(const Definition& def);
		void Draw_(const ShadingMode mode) const;
	};
}//!sge