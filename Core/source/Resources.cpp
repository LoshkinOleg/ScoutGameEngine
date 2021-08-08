#include "Resources.h"

#include <iostream>

#include <xxhash.h>

#include "Engine.h"
#include "macros.h"

namespace sge
{
	JsonData* sge::JsonDataHandle::operator->() const
	{
		return &Engine::Get().GetResourceManager().GetJsonData(*this);
	}
	JsonData& JsonDataHandle::operator*() const
	{
		return Engine::Get().GetResourceManager().GetJsonData(*this);
	}
	KtxData* sge::KtxDataHandle::operator->() const
	{
		return &Engine::Get().GetResourceManager().GetKtxData(*this);
	}
	KtxData& KtxDataHandle::operator*() const
	{
		return Engine::Get().GetResourceManager().GetKtxData(*this);
	}
	GltfData* sge::GltfDataHandle::operator->() const
	{
		return &Engine::Get().GetResourceManager().GetGltfData(*this);
	}
	GltfData& GltfDataHandle::operator*() const
	{
		return Engine::Get().GetResourceManager().GetGltfData(*this);
	}
	ShaderData* sge::ShaderDataHandle::operator->() const
	{
		return &Engine::Get().GetResourceManager().GetShaderData(*this);
	}
	ShaderData& ShaderDataHandle::operator*() const
	{
		return Engine::Get().GetResourceManager().GetShaderData(*this);
	}
	Shader* sge::ShaderHandle::operator->() const
	{
		return &Engine::Get().GetRenderer().GetShader(*this);
	}
	Shader& ShaderHandle::operator*() const
	{
		return Engine::Get().GetRenderer().GetShader(*this);
	}
	Texture* sge::TextureHandle::operator->() const
	{
		return &Engine::Get().GetRenderer().GetTexture(*this);
	}
	Texture& TextureHandle::operator*() const
	{
		return Engine::Get().GetRenderer().GetTexture(*this);
	}
	VertexBuffer* sge::VertexBufferHandle::operator->() const
	{
		return &Engine::Get().GetRenderer().GetVertexBuffer(*this);
	}
	VertexBuffer& VertexBufferHandle::operator*() const
	{
		return Engine::Get().GetRenderer().GetVertexBuffer(*this);
	}
	Mesh* sge::MeshHandle::operator->() const
	{
		return &Engine::Get().GetRenderer().GetMesh(*this);
	}
	Mesh& MeshHandle::operator*() const
	{
		return Engine::Get().GetRenderer().GetMesh(*this);
	}
	Model* sge::ModelHandle::operator->() const
	{
		return &Engine::Get().GetRenderer().GetModel(*this);
	}

	Model& ModelHandle::operator*() const
	{
		return Engine::Get().GetRenderer().GetModel(*this);
	}

	JsonData& sge::JsonDataContainer::Access(const JsonDataHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		JsonData& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	KtxData& sge::KtxDataContainer::Access(const KtxDataHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		KtxData& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	GltfData& sge::GltfDataContainer::Access(const GltfDataHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		GltfData& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	ShaderData& sge::ShaderDataContainer::Access(const ShaderDataHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		ShaderData& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	Shader& sge::ShaderContainer::Access(const ShaderHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		Shader& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	Texture& sge::TextureContainer::Access(const TextureHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		Texture& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	VertexBuffer& sge::VertexBufferContainer::Access(const VertexBufferHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		VertexBuffer& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	Mesh& sge::MeshContainer::Access(const MeshHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		Mesh& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}
	Model& sge::ModelContainer::Access(const ModelHandle& handle)
	{
		assert(handle.hash > 0);
		assert(data_.find(handle.hash) != data_.end());
		Model& value = data_[handle.hash];
		assert(value == handle);
		return value;
	}

	void JsonDataContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool JsonDataContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void JsonDataContainer::Insert(const JsonData& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void KtxDataContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool KtxDataContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void KtxDataContainer::Insert(const KtxData& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void GltfDataContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool GltfDataContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void GltfDataContainer::Insert(const GltfData& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void ShaderDataContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool ShaderDataContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void ShaderDataContainer::Insert(const ShaderData& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void ShaderContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool ShaderContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void ShaderContainer::Insert(const Shader& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void TextureContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool TextureContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void TextureContainer::Insert(const Texture& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void VertexBufferContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool VertexBufferContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void VertexBufferContainer::Insert(const VertexBuffer& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void MeshContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool MeshContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void MeshContainer::Insert(const Mesh& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}
	void ModelContainer::Clear()
	{
		for (auto& pair : data_)
		{
			pair.second.Destroy();
		}
		data_.clear();
	}
	bool ModelContainer::Exists(const Hash key)
	{
		return data_.find(key) != data_.end();
	}
	void ModelContainer::Insert(const Model& value)
	{
		assert(!Exists(value.hash));
		data_[value.hash] = value;
	}


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
	void sge::Shader::Destroy()
	{
		if (PROGRAM > 0)
		{
			glDeleteProgram(PROGRAM);
			sge_CHECK_GL_ERROR();
		}
		PROGRAM = 0;
		uniformLocationCache.clear();
	}
	void sge::Texture::Destroy()
	{
		if (TEX > 0)
		{
			glDeleteTextures(1, &TEX);
		}
		TEX = 0;
	}
	void VertexBuffer::Destroy()
	{
		if (VBO > 0)
		{
			glDeleteBuffers(1, &VBO);
			sge_CHECK_GL_ERROR();
		}
		VBO = 0;
	}
	void sge::Mesh::Destroy()
	{
		isTransparent = false;
		indexType = GL_UNSIGNED_SHORT;
		nrOfVertices = 0;
		shininess = 0.0f;
		normalMap->Destroy();
		specularMap->Destroy();
		albedoMap->Destroy();
		alphaMap->Destroy();
		uvs->Destroy();
		bitangents->Destroy();
		tangents->Destroy();
		normals->Destroy();
		positions->Destroy();
		if (VAO > 0)
		{
			glDeleteVertexArrays(1, &VAO);
		}
		VAO = 0;
		EBO = 0;
	}
	void sge::Model::Destroy()
	{
		radius = 0.0f;
		transformsVertexBuffer->Destroy();
		Engine::Get().GetResourceManager().FreeTransforms(transformsBegin, transformsEnd);
		for (auto& mesh : meshes)
		{
			mesh->Destroy();
		}
	}

	int32_t Shader::GetUniformLocation(const std::string_view name)
	{
		const uint32_t hash = XXH32(name.data(), name.size(), HASHING_SEED);
		const auto match = uniformLocationCache.find(hash);
		if (match != uniformLocationCache.end()) // Name of uniform already known, use it.
		{
			return match->second;
		}
		else
		{
			const int location = glGetUniformLocation(PROGRAM, name.data());
			sge_CHECK_GL_ERROR();
			if (location < 0) { sge_ERROR("Trying to get the location of a non existent uniform!"); }
			uniformLocationCache.insert({ hash, location }); // Add the new entry.
			return location;
		}
	}
	void Shader::SetInt(const std::string_view name, const int32_t value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform1i(location, value);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetFloat(const std::string_view name, const float value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform1f(location, value);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetVec3(const std::string_view name, const glm::vec3 value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform3fv(location, 1, &value[0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetMat4(const std::string_view name, const glm::mat4& value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetMat3(const std::string_view name, const glm::mat3& value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
		sge_CHECK_GL_ERROR();
	}
}//!sge