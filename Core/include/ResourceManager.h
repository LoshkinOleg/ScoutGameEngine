#pragma once

#include "Resources.h"
#include "macros.h"

namespace sge
{
	class ResourceManager
	{
	public:
		sge_DISALLOW_COPY(ResourceManager);

		enum GltfAttributes: uint8_t
		{
			INVALID = 0,

			POSITIONS = 1 << 0,
			NORMALS = 1 << 1,
			TANGENTS = 1 << 2,
			UVS = 1 << 3,

			ALPHA_MAPS = 1 << 4,
			ALBEDO_MAPS = 1 << 5,
			NORMAL_MAPS = 1 << 6,
			SPECULAR_MAPS = 1 << 7
		};

		void Init();
		void PostInit();

		JsonDataHandle LoadJson(const std::string_view path);
		GltfDataHandle LoadGltf(const std::string_view path, const GltfAttributes dataToLoad);
		KtxDataHandle LoadKtx(const std::string_view path);
		ShaderDataHandle LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath);

		void FreeJson(const JsonDataHandle& handle);
		void FreeGltf(GltfDataHandle handle);
		void FreeKtx(KtxDataHandle handle);
		void FreeShader(ShaderDataHandle handle);

		JsonData& GetJsonData(const JsonDataHandle& handle);
		KtxData& GetKtxData(const KtxDataHandle& handle);
		GltfData& GetGltfData(const GltfDataHandle& handle);
		ShaderData& GetShaderData(const ShaderDataHandle& handle);

		glm::mat4* const AllocateTransforms(const std::vector<glm::mat4>& transforms);
		void FreeTransforms(glm::mat4* const begin, glm::mat4* const end);

		void Shutdown();

	private:

		friend class Engine;
		sge_ALLOW_CONSTRUCTION(ResourceManager);

		static std::string LoadFile_(const std::string_view path);
		static uint32_t HashString_(const std::string_view str);
		static uint32_t HashGltf_(const tinygltf::Model& model, const std::array<const KtxDataHandle, 4> textures);
		static uint32_t HashKtx_(const gli::texture& ktx);

		constexpr static const bool FREE_DATA_POST_INIT_ = true;
		constexpr static const uint32_t TRANSFORMS_POOL_SIZE_ = 256;

		ResourceContainer<JsonData, JsonDataHandle> jsons_ = {};
		ResourceContainer<GltfData, GltfDataHandle> gltfs_ = {};
		ResourceContainer<KtxData, KtxDataHandle> ktxs_ = {};
		ResourceContainer<ShaderData, ShaderDataHandle> shaderSrcs_ = {};

		glm::mat4* transforms_ = nullptr;
		glm::mat4* currentTransformsEnd_ = nullptr;
	};
}//!sge