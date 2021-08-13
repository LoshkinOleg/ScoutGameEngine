#pragma once

#include <vector>

#include "ResourcesAbstracts.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Shader.h"
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
			std::vector<Texture::Definition> texDefs = {};
			VertexBuffer::Definition eboDef = {};
			IllumMode illum = IllumMode::INVALID;
			glm::vec3 color = WHITE;
			float shininess = 0.0f;

			bool IsValid() const;
		};

		uint32_t VAO = 0;
		Handle<VertexBuffer> indexVBO = {};
		std::vector<Handle<VertexBuffer>> vertexBuffers = {};
		std::vector<Handle<Texture>> textures = {};
		uint32_t nrOfVertices = 0;
		float radius = 0.0f;
		IllumMode illum = IllumMode::INVALID;
		glm::vec3 color = WHITE;
		float shininess = 0.0f;

		void Init(const Definition& def);
		void Update(const std::vector<std::pair<void*, uint32_t>>& dataAndByteLen) const;
		void Draw(const Handle<Shader>& shader, const uint32_t nrOfInstances, const glm::mat4& viewMatrix, const bool updateLightingUniforms) const;
		void Destroy();

		bool IsValid() const;
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge