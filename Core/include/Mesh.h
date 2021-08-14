#pragma once

#include <vector>

#include "ResourcesAbstracts.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "globals.h"
#include "macros.h"

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
			std::vector<Material::Definition> matDefs = {}; // At most 1 material per illumination model.

			bool IsValid() const;
		};

		uint32_t VAO = 0;
		Handle<VertexBuffer> indexVBO = {};
		std::vector<Handle<VertexBuffer>> vertexBuffers = {};
		Handle<Material> material = {};
		uint32_t nrOfVertices = 0;
		float radius = 0.0f;

		void Update(const std::vector<std::pair<void*, uint32_t>>& dataAndByteLen) const;
		void Draw(const Handle<Shader>& shader, const uint32_t nrOfInstances) const;

		bool IsValid() const;
		inline void Reset()
		{
			*this = {};
		}

	private:
		friend class Renderer;
		void Init_(const Definition& def);
	};
}//!sge