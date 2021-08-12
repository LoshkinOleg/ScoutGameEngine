#include "Resources.h"

#include <iostream>

#include <xxhash.h>

#include "Engine.h"
#include "macros.h"

namespace sge
{
	void sge::JsonData::Destroy()
	{
		data = json::value_t::discarded; // Has RAII.
	}
	void sge::KtxData::Destroy()
	{
		data = {}; // Should have RAII.
	}
	void sge::GltfData::Destroy()
	{
		model = {}; // Should have RAII.
		alphaMap = {};
		albedoMap = {};
		specularMap = {};
		normalMap = {};
	}
	void sge::ShaderData::Destroy()
	{
		vertexCode = std::string_view();
		fragmentCode = std::string_view();
	}
	void sge::BlinnPhongMesh::Destroy()
	{
		isTransparent = false;
		indexType = GL_UNSIGNED_SHORT;
		nrOfVertices = 0;
		shininess = 0.0f;
		color = WHITE;
		// normalMap->Destroy();
		// specularMap->Destroy();
		// albedoMap->Destroy();
		// alphaMap->Destroy();
		// uvs->Destroy();
		// bitangents->Destroy();
		// tangents->Destroy();
		// normals->Destroy();
		// positions->Destroy();
		// if (VAO > 0)
		// {
		// 	glDeleteVertexArrays(1, &VAO);
		// }
		VAO = 0;
		EBO = 0;
	}
	void sge::BlinnPhongModel::Destroy()
	{
		radius = 0.0f;
		// transformsVertexBuffer->Destroy();
		Engine::Get().GetResourceManager().FreeTransforms(transformsBegin, transformsEnd);
		// for (auto& mesh : meshes)
		// {
		// 	mesh->Destroy();
		// }
	}
}//!sge