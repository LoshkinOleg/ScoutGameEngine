#include "Mesh.h"

#include "Engine.h"

namespace sge
{
	bool IndexedMesh::Definition::IsValid() const
	{
		bool returnVal = false;
		returnVal = returnVal && vboDefs.size();
		returnVal = returnVal && illum;
		returnVal = returnVal && eboDef.IsValid();
		returnVal = returnVal && eboDef.componentsPerElement == 1;
		switch(illum)
		{
			case sge::ALBEDO_ONLY:
			{
				returnVal = returnVal && texDefs.size() == 1;
			}break;
			case sge::GOOCH: break;
			case sge::GIZMO: break;
			case sge::BLINN_PHONG:
			{
				returnVal = returnVal && texDefs.size() == 2;
				returnVal = returnVal && shininess;
			}break;
			case sge::BLINN_PHONG_NORMALMAPPED:
			{
				returnVal = returnVal && texDefs.size() == 3;
				returnVal = returnVal && shininess;
			}break;
			default:
			{
				returnVal = false;
			}break;
		}
		return returnVal;
	}

	void IndexedMesh::Init(const Definition& def)
	{
		assert(def.IsValid());
		auto& renderer = Engine::Get().GetRenderer();
		const uint32_t nrOfVbos = def.vboDefs.size();

		shininess = def.shininess;
		color = def.color;
		
		// Note: we're assuming the first buffer is always the one with positions.
		const VertexBuffer::Definition& vbdef = def.vboDefs[0];
		assert(vbdef.IsValid() && (vbdef.componentsPerElement == 2 || vbdef.componentsPerElement == 3) && vbdef.componentType == GL_FLOAT && !vbdef.isIndexBuffer && vbdef.usage == GL_STATIC_DRAW);
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
		assert(def.eboDef.componentType == GL_UNSIGNED_INT);
		nrOfVertices = def.eboDef.byteLen / sizeof(uint32_t);

		vertexBuffers.resize(nrOfVbos); // One is for the indices.
		uint32_t indexBufferIdx = 0;
		for(uint32_t i = 0; i < nrOfVbos; i++)
		{
			assert(def.vboDefs[i].IsValid());
			assert(def.vboDefs[i].componentType == GL_FLOAT); // Not handling any other type of vertex data yet.
			vertexBuffers[i] = renderer.CreateVertexBuffer(def.vboDefs[i], 0);
			sge_CHECK_GL_ERROR();
		}

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		indexVBO = renderer.CreateVertexBuffer(def.eboDef, 0);
		sge_CHECK_GL_ERROR();

		const uint32_t nrOfTextures = def.texDefs.size();
		for(uint32_t i = 0; i < nrOfTextures; i++)
		{
			textures[i] = renderer.CreateTexture(def.texDefs[i], 0);
			sge_CHECK_GL_ERROR();
		}

		for(uint32_t i = 0; i < vertexBuffers.size(); i++)
		{
			sge_CHECK_GL_ERROR();
			vertexBuffers[i]->Bind();
			glEnableVertexAttribArray((GLuint)i);
			glVertexAttribPointer((GLuint)i, (GLint)vertexBuffers[i]->componentsPerElement, GL_FLOAT, GL_FALSE, (GLsizei)vertexBuffers[i]->Stride(), 0);
			if(vertexBuffers[i]->usage == GL_DYNAMIC_DRAW)
			{
				glVertexAttribDivisor((GLuint)i, 1); // Note: only supports per-mesh instancing.
			}
			sge_CHECK_GL_ERROR();
		}
	}
	void IndexedMesh::Update(const std::vector<std::pair<void*, uint32_t>>& dataAndByteLen) const
	{
		uint32_t idx = 0;
		const uint32_t len = vertexBuffers.size();
		for(uint32_t i = 0; i < len; i++)
		{
			const auto& buffer = *vertexBuffers[i];
			if(buffer.usage == GL_DYNAMIC_DRAW)
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
	void IndexedMesh::Draw(const Handle<Shader>& shader, const uint32_t nrOfInstances, const glm::mat4& viewMatrix, const bool updateLightingUniforms) const
	{
		assert(shader->illum == illum);
		shader->Bind();
		switch(illum) // Should get updated for every mesh.
		{
			case sge::ALBEDO_ONLY: break;
			case sge::GOOCH:
			{
				shader->SetVec3("color", color);
			}break;
			case sge::GIZMO:
			{
				shader->SetVec3("color", color);
			}break;
			case sge::BLINN_PHONG:
			{
				shader->SetFloat("shininess", shininess);
			}break;
			case sge::BLINN_PHONG_NORMALMAPPED:
			{
				shader->SetFloat("shininess", shininess);
			}break;
			default:
			{
				sge_ERROR("Unexpected illum value!");
			}break;
		}
		if(updateLightingUniforms) // Should be updated once per frame.
		{
			switch(illum)
			{
				case sge::ALBEDO_ONLY: break;
				case sge::GOOCH:; break;
				case sge::GIZMO:; break;
				case sge::BLINN_PHONG:
				{
					const glm::mat4 cameraMatrix = WINDOW_PROJECTION * viewMatrix;
					shader->SetMat4("cameraMatrix", cameraMatrix);
					shader->SetVec3("viewPos", glm::vec3(viewMatrix[3]));
				}break;
				case sge::BLINN_PHONG_NORMALMAPPED:
				{
					const glm::mat4 cameraMatrix = WINDOW_PROJECTION * viewMatrix;
					shader->SetMat4("cameraMatrix", cameraMatrix);
					shader->SetVec3("viewPos", glm::vec3(viewMatrix[3]));
				}break;
				default:
				{
					sge_ERROR("Unexpected illum value!");
				}break;
			}
		}
		glBindVertexArray(VAO);
		if(nrOfInstances)
		{
			glDrawElementsInstanced((GLenum)shader->primitive, (GLsizei)nrOfVertices, (GLenum)indexVBO->componentType, 0, (GLsizei)nrOfInstances);
		}
		else
		{
			glDrawElements((GLenum)shader->primitive, (GLsizei)nrOfVertices, (GLenum)indexVBO->componentType, 0);
		}
	}
	void IndexedMesh::Destroy()
	{
		if(IsValid())
		{
			for(auto& texture : textures)
			{
				texture->Destroy();
			}
			for(auto& buffer : vertexBuffers)
			{
				buffer->Destroy();
			}
			indexVBO->Destroy();
			Reset();
		}
		else
		{
			sge_WARNING("Trying to delete an invalid IndexedMesh!");
		}
	}
	bool IndexedMesh::IsValid() const
	{
		bool returnVal = false;
		returnVal = returnVal && VAO;
		returnVal = returnVal && indexVBO.IsValid();
		returnVal = returnVal && indexVBO->IsValid();
		for(const auto& handle : vertexBuffers)
		{
			returnVal = returnVal && handle.IsValid();
			returnVal = returnVal && handle->IsValid();
		}
		for(const auto& handle : textures)
		{
			returnVal = returnVal && handle.IsValid();
			returnVal = returnVal && handle->IsValid();
		}
		returnVal = returnVal && nrOfVertices;
		switch(illum)
		{
			case sge::ALBEDO_ONLY:
			{
				returnVal = returnVal && textures.size() == 1;
			}break;
			case sge::GOOCH: break;
			case sge::BLINN_PHONG:
			{
				returnVal = returnVal && textures.size() == 2;
				returnVal = returnVal && shininess;
			}break;
			case sge::BLINN_PHONG_NORMALMAPPED:
			{
				returnVal = returnVal && textures.size() == 3;
				returnVal = returnVal && shininess;
			}break;
			default:
			{
				returnVal = false;
			}break;
		}

		return returnVal;
	}
}//!sge