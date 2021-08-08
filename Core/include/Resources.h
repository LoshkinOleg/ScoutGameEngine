#pragma once

#include <vector>
#include <map>
#include <type_traits>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>

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

	template <typename ResourceType, typename HandleType>
	class A_ResourceContainer
	{
	protected:
		std::map<Hash, ResourceType> data_ = {};
	public:
		virtual ResourceType& Access(const HandleType& handle) = 0;
		virtual void Clear() = 0;
		virtual bool Exists(const Hash key) = 0;
		virtual void Insert(const ResourceType& value) = 0;
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

	class JsonDataContainer : public A_ResourceContainer<JsonData, JsonDataHandle>
	{
	public:
		JsonData& Access(const JsonDataHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const JsonData& value) override;
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

	class KtxDataContainer : public A_ResourceContainer<KtxData, KtxDataHandle>
	{
	public:
		KtxData& Access(const KtxDataHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const KtxData& value) override;
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

	class GltfDataContainer : public A_ResourceContainer<GltfData, GltfDataHandle>
	{
	public:
		GltfData& Access(const GltfDataHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const GltfData& value) override;
	};

	struct ShaderData : public A_Resource
	{
		std::string_view vertexCode = std::string_view();
		std::string_view fragmentCode = std::string_view();

		void Destroy() override;
	};

	struct ShaderDataHandle : public A_Handle
	{
		ShaderData* operator->() const override;
		ShaderData& operator*() const override;
	};

	class ShaderDataContainer : public A_ResourceContainer<ShaderData, ShaderDataHandle>
	{
	public:
		ShaderData& Access(const ShaderDataHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const ShaderData& value) override;
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

	class ShaderContainer : public A_ResourceContainer<Shader, ShaderHandle>
	{
	public:
		Shader& Access(const ShaderHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const Shader& value) override;
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

	class TextureContainer : public A_ResourceContainer<Texture, TextureHandle>
	{
	public:
		Texture& Access(const TextureHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const Texture& value) override;
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

	class VertexBufferContainer : public A_ResourceContainer<VertexBuffer, VertexBufferHandle>
	{
	public:
		VertexBuffer& Access(const VertexBufferHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const VertexBuffer& value) override;
	};

	struct Mesh : public A_Resource
	{
		uint32_t VAO = 0;
		uint32_t EBO = 0;
		VertexBufferHandle positions = {};
		VertexBufferHandle normals = {};
		VertexBufferHandle tangents = {};
		VertexBufferHandle bitangents = {};
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

	class MeshContainer : public A_ResourceContainer<Mesh, MeshHandle>
	{
	public:
		Mesh& Access(const MeshHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const Mesh& value) override;
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

	class ModelContainer : public A_ResourceContainer<Model, ModelHandle>
	{
	public:
		Model& Access(const ModelHandle& handle) override;
		void Clear() override;
		bool Exists(const Hash key) override;
		void Insert(const Model& value) override;
	};

}//!sge