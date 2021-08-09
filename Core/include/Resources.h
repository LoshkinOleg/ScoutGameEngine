#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <type_traits>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>

#include "macros.h"

namespace sge
{
	using Hash = unsigned int;
	struct A_Resource;

	struct A_Handle
	{
		Hash hash = 0;

		bool operator==(const A_Handle& other) const;
		bool operator==(const A_Resource& resource) const;
		virtual A_Resource* operator->() const = 0;
		virtual A_Resource& operator*() const = 0;
	};

	struct A_Resource
	{
		Hash hash = 0;

		bool operator==(const A_Resource& other) const;
		bool operator==(const A_Handle& handle) const;
		virtual void Destroy() = 0;
	};

	template <typename ResourceType> concept SubResource = std::derived_from<ResourceType, A_Resource>;
	template <typename HandleType> concept SubHandle = std::derived_from<HandleType, A_Handle>;
	template<SubResource Resource, SubHandle Handle>
	class ResourceContainer
	{
	protected:
		std::map<Hash, Resource> data_ = {};
	public:
		Resource& Access(const Handle& handle)
		{
			assert(handle.hash > 0);
			assert(data_.find(handle.hash) != data_.end());
			Resource& value = data_[handle.hash];
			assert(value == handle);
			return value;
		}
		void Clear()
		{
			for (auto& pair : data_)
			{
				pair.second.Destroy();
			}
			data_.clear();
		}
		bool Exists(const Hash key)
		{
			return data_.find(key) != data_.end();
		}
		void Insert(const Resource& value)
		{
			if (Exists(value.hash))
			{
				sge_WARNING("Value is already in the map! Not inserting new element.");
			}
			data_[value.hash] = value;
		}
	};

	struct JsonData : public A_Resource
	{
		json data = json::value_t::discarded;

		void Destroy() override;
	};

	struct JsonDataHandle : public A_Handle
	{
		JsonData* operator->() const override;
		JsonData& operator*() const override;
	};

	struct KtxData : public A_Resource
	{
		gli::texture data = {};

		void Destroy() override;
	};

	struct KtxDataHandle : public A_Handle
	{
		KtxData* operator->() const override;
		KtxData& operator*() const override;
	};

	struct GltfData : public A_Resource
	{
		tinygltf::Model model = {};
		KtxDataHandle alphaMap = {};
		KtxDataHandle albedoMap = {};
		KtxDataHandle specularMap = {};
		KtxDataHandle normalMap = {};

		void Destroy() override;
	};

	struct GltfDataHandle : public A_Handle
	{
		GltfData* operator->() const override;
		GltfData& operator*() const override;
	};

	struct ShaderData : public A_Resource
	{
		std::string vertexCode = "";
		std::string fragmentCode = "";

		void Destroy() override;
	};

	struct ShaderDataHandle : public A_Handle
	{
		ShaderData* operator->() const override;
		ShaderData& operator*() const override;
	};

	struct Shader : public A_Resource
	{
		uint32_t PROGRAM = 0;
		std::map<Hash, uint32_t> uniformLocationCache = {};

		void Destroy() override;
		int32_t GetUniformLocation(const std::string_view name);
		void SetInt(const std::string_view name, const int32_t value);
		void SetFloat(const std::string_view name, const float value);
		void SetVec3(const std::string_view name, const glm::vec3 value);
		void SetMat4(const std::string_view name, const glm::mat4& value);
		void SetMat3(const std::string_view name, const glm::mat3& value);
	};

	struct ShaderHandle : public A_Handle
	{
		Shader* operator->() const override;
		Shader& operator*() const override;
	};

	struct Texture : public A_Resource
	{
		uint32_t TEX = 0;

		void Destroy() override;
	};

	struct TextureHandle : public A_Handle
	{
		Texture* operator->() const override;
		Texture& operator*() const override;
	};

	struct VertexBuffer : public A_Resource
	{
		uint32_t VBO = 0;

		void Destroy() override;
	};

	struct VertexBufferHandle : public A_Handle
	{
		VertexBuffer* operator->() const override;
		VertexBuffer& operator*() const override;
	};

	struct Mesh : public A_Resource
	{
		uint32_t VAO = 0;
		uint32_t EBO = 0;
		VertexBufferHandle positions = {};
		VertexBufferHandle normals = {};
		VertexBufferHandle tangents = {};
		VertexBufferHandle uvs = {};
		TextureHandle alphaMap = {};
		TextureHandle albedoMap = {};
		TextureHandle specularMap = {};
		TextureHandle normalMap = {};
		float shininess = 0.0f;
		uint32_t nrOfVertices = 0;
		int32_t indexType = GL_UNSIGNED_SHORT;
		bool isTransparent = false;

		void Destroy() override;
	};

	struct MeshHandle : public A_Handle
	{
		Mesh* operator->() const override;
		Mesh& operator*() const override;
	};

	struct Model : public A_Resource
	{
		std::vector<MeshHandle> meshes = {};
		glm::mat4* transformsBegin = nullptr;
		glm::mat4* transformsEnd = nullptr;
		VertexBufferHandle transformsVertexBuffer = {};
		float radius = 0.0f;

		void Destroy() override;
	};

	struct ModelHandle : public A_Handle
	{
		Model* operator->() const override;
		Model& operator*() const override;
	};

}//!sge