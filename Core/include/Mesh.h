#pragma once

#include "VertexBuffer.h"
#include "Material.h"

namespace sge
{
	/*
	@brief: A mesh with non-interleaved data.
	*/
	struct IndexedMesh
	{
		struct Definition
		{
			std::vector<VertexBuffer::Definition> vboDefs = {};
			VertexBuffer::Definition eboDef = {};
			std::vector<Material::Definition> matDefs = {}; // At most 1 material per shading mode.

			bool IsValid() const;

			Hash ComputeHash() const;
		};

		uint32_t VAO = 0;
		Handle<VertexBuffer> indexVBO = {};
		std::vector<Handle<VertexBuffer>> vertexBuffers = {};
		std::map<const ShadingMode, const Handle<Material>> materials = {}; // at most 1 per shading mode
		uint32_t nrOfVertices = 0;
		float radius = 0.0f;

		void Update(const std::vector<std::pair<void*, uint32_t>>& dataAndByteLen) const;

		bool IsValid() const;
		inline void Reset()
		{
			*this = {};
		}

	private:
		friend class Renderer;
		friend class Model;
		void Init_(const Definition& def);
		void Draw_(const uint32_t nrOfInstances, const uint32_t primitive, const ShadingMode mode);
	};
}//!sge