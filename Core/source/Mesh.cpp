#include "Mesh.h"

#include <glad/glad.h>

#include "Engine.h"
#include "Transform.h"

namespace sge
{
	bool IndexedMesh::Definition::IsValid() const
	{
		bool returnVal = vboDefs.size();
		returnVal &= eboDef.IsValid();
		returnVal &= eboDef.componentsPerElement == 1;
		for(const auto& matDef : matDefs)
		{
			returnVal &= matDef.IsValid();
		}
		return returnVal;
	}
	Hash IndexedMesh::Definition::ComputeHash() const
	{
		Hash hash = 0;
		for(const auto& element : vboDefs)
		{
			hash.Accumulate(element.preComputedHash);
		}
		hash.Accumulate(eboDef.begin, eboDef.byteLen);
		for(const auto& element : matDefs)
		{
			hash.Accumulate(element.ComputeHash());
		}
		return hash;
	}
	void IndexedMesh::Init_(const Definition& def)
	{
		assert(def.IsValid());
		auto& renderer = Engine::Get().GetRenderer();
		const uint32_t nrOfVbos = (uint32_t)def.vboDefs.size();
		
		// Note: we're assuming the first buffer is always the one with positions.
		const VertexBuffer::Definition& vbdef = def.vboDefs[0];
		assert(vbdef.IsValid() && (vbdef.componentsPerElement == 2 || vbdef.componentsPerElement == 3) && vbdef.componentType == (NumberType)GL_FLOAT && !vbdef.isIndexBuffer && vbdef.mutability == (Mutability)GL_STATIC_DRAW);
		const uint32_t componentsPerElement = vbdef.componentsPerElement;

		float largestRadius = 0.0f;
		if(componentsPerElement == 3)
		{
			std::vector<glm::vec3> positions;
			positions.insert
			(
				positions.end(),
				reinterpret_cast<glm::vec3*>(vbdef.begin),
				reinterpret_cast<glm::vec3*>(vbdef.begin + vbdef.byteLen)
			);

			glm::vec3 centroid = ZERO_VEC3;
			for(const auto& pos : positions)
			{
				centroid += pos;
			}
			centroid /= positions.size();

			for(const auto& pos : positions)
			{
				const float newRadius = glm::length(pos - centroid);
				largestRadius = newRadius > largestRadius ? newRadius : largestRadius;
			}
		}
		else
		{
			std::vector<glm::vec2> positions;
			positions.insert
			(
				positions.end(),
				reinterpret_cast<glm::vec2*>(vbdef.begin),
				reinterpret_cast<glm::vec2*>(vbdef.begin + vbdef.byteLen)
			);

			glm::vec2 centroid = ZERO_VEC3;
			for(const auto& pos : positions)
			{
				centroid += pos;
			}
			centroid /= positions.size();

			for(const auto& pos : positions)
			{
				const float newRadius = glm::length(pos - centroid);
				largestRadius = newRadius > largestRadius ? newRadius : largestRadius;
			}
		}
		radius = largestRadius;

		// Not handling any type other type of index yet.
		assert(def.eboDef.componentType == (NumberType)GL_UNSIGNED_INT);
		nrOfVertices = def.eboDef.byteLen / sizeof(uint32_t);

		vertexBuffers.resize(nrOfVbos); // One is for the indices.
		uint32_t indexBufferIdx = 0;
		for(uint32_t i = 0; i < nrOfVbos; i++)
		{
			assert(def.vboDefs[i].IsValid());
			assert(def.vboDefs[i].componentType == (NumberType)GL_FLOAT); // Not handling any other type of vertex data yet.
			vertexBuffers[i] = renderer.CreateVertexBuffer(def.vboDefs[i]);
			assert(vertexBuffers[i].IsValid());
			sge_CHECK_GL_ERROR();
		}

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		indexVBO = renderer.CreateVertexBuffer(def.eboDef);
		sge_CHECK_GL_ERROR();

		const uint32_t nrOfMaterials = (uint32_t)def.matDefs.size();
		for(uint32_t i = 0; i < nrOfMaterials; i++)
		{
			materials.insert({ def.matDefs[i].shadingMode, renderer.CreateMaterial(def.matDefs[i]) });
			sge_CHECK_GL_ERROR();
		}

		// Set up VAO ptrs.
		for(uint32_t i = 0; i < vertexBuffers.size(); i++)
		{
			sge_CHECK_GL_ERROR();
			assert(vertexBuffers[i].IsValid());
			vertexBuffers[i]->Bind();
			glEnableVertexAttribArray((GLuint)i);
			glVertexAttribPointer((GLuint)i, (GLint)vertexBuffers[i]->componentsPerElement, GL_FLOAT, GL_FALSE, (GLsizei)vertexBuffers[i]->Stride(), 0);
			if(vertexBuffers[i]->mutability == (Mutability)GL_DYNAMIC_DRAW)
			{
				glVertexAttribDivisor((GLuint)i, 1); // Note: only supports per-mesh instancing.
			}
			sge_CHECK_GL_ERROR();
		}
	}

	void IndexedMesh::Update(const std::vector<std::pair<void*, uint32_t>>& dataAndByteLen) const
	{
		sge_ERROR("Not yet implemented!");

		uint32_t idx = 0;
		const uint32_t len = (uint32_t)vertexBuffers.size();
		for(uint32_t i = 0; i < len; i++)
		{
			const auto& buffer = *vertexBuffers[i];
			if(buffer.mutability == (Mutability)GL_DYNAMIC_DRAW)
			{
				const auto& pair = dataAndByteLen[idx++];
				buffer.Update(pair.first, pair.second);

				sge_CHECK_GL_ERROR();
				glBindVertexArray(VAO);
				glEnableVertexAttribArray((GLuint)i);
				glVertexAttribPointer((GLuint)i, (GLint)buffer.componentsPerElement, GL_FLOAT, GL_FALSE, (GLsizei)buffer.Stride(), 0);
				glVertexAttribDivisor((GLuint)i, 1);
				sge_CHECK_GL_ERROR();
			}
		}
	}

	void IndexedMesh::Draw_(const HashlessResourceHandle<TransformsBuffer>& transforms, const uint32_t primitive, const ShadingMode mode)
	{
		auto& renderer = Engine::Get().GetRenderer();
		glBindVertexArray(VAO);
		renderer.BindModelMatricesVbo();

		const uint32_t matrixLocation = (uint32_t)vertexBuffers.size(); // iModelMatrix comes after all the other vertex attributes.
		glEnableVertexAttribArray((unsigned int)matrixLocation);
		glVertexAttribPointer((unsigned int)matrixLocation, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray((unsigned int)matrixLocation + 1);
		glVertexAttribPointer((unsigned int)matrixLocation + 1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
		glEnableVertexAttribArray((unsigned int)matrixLocation + 2);
		glVertexAttribPointer((unsigned int)matrixLocation + 2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)));
		glEnableVertexAttribArray((unsigned int)matrixLocation + 3);
		glVertexAttribPointer((unsigned int)matrixLocation + 3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)));
		glVertexAttribDivisor((unsigned int)matrixLocation, 1);
		glVertexAttribDivisor((unsigned int)matrixLocation + 1, 1);
		glVertexAttribDivisor((unsigned int)matrixLocation + 2, 1);
		glVertexAttribDivisor((unsigned int)matrixLocation + 3, 1);

		const uint32_t nrOfInstances = transforms->GetNrOfTransforms();
		materials[mode]->Bind();
		if(nrOfInstances)
		{
			glDrawElementsInstanced((GLenum)primitive, (GLsizei)nrOfVertices, (GLenum)indexVBO->componentType, 0, (GLsizei)nrOfInstances);
		}
		else
		{
			glDrawElements((GLenum)primitive, (GLsizei)nrOfVertices, (GLenum)indexVBO->componentType, 0);
		}
	}

	bool IndexedMesh::IsValid() const
	{
		bool returnVal = VAO;
		returnVal &= indexVBO.IsValid();
		returnVal &= indexVBO->IsValid();
		for(const auto& handle : vertexBuffers)
		{
			returnVal &= handle.IsValid();
			returnVal &= handle->IsValid();
		}
		for(const auto& pair : materials)
		{
			returnVal &= pair.second.IsValid();
			returnVal &= pair.second->IsValid();
		}
		returnVal &= (bool)nrOfVertices;
		return returnVal;
	}
}//!sge