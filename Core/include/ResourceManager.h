#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>

#include "ResourcesAbstracts.h"
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
		struct JsonData
		{
			json data = json::value_t::discarded;
		};
		struct KtxData
		{
			gli::texture data = {};
		};
		struct ShaderData
		{
			std::string vertexCode = "";
			std::string fragmentCode = "";
		};
		struct GltfData
		{
			tinygltf::Model model = {};
			Handle<KtxData> alphaMap = {};
			Handle<KtxData> albedoMap = {};
			Handle<KtxData> specularMap = {};
			Handle<KtxData> normalMap = {};
		};
		struct TransformsBuffer
		{
			glm::mat4* begin = nullptr;
			glm::mat4* end = nullptr;

			inline uint32_t NrOfTransforms() const
			{
				return end - begin;
			}
		};

		void Init();
		void FreeResources();

		Handle<JsonData> LoadJson(const std::string_view path);
		Handle<KtxData> LoadKtx(const std::string_view path);
		Handle<ShaderData> LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath);
		Handle<GltfData> LoadGltf(const std::string_view path, const GltfAttributes dataToLoad);

		void FreeJson(Handle<JsonData>& handle);
		void FreeKtx(Handle<KtxData>& handle);
		void FreeShader(Handle<ShaderData>& handle);
		void FreeGltf(Handle<GltfData>& handle);
		void FreeTransforms(Handle<TransformsBuffer>& handle);

		Handle<TransformsBuffer> const AllocateTransforms(void* data, const uint32_t byteLen);

		void Shutdown();

	private:
		struct TransformsPool_
		{
			sge_DISALLOW_COPY(TransformsPool_);

			constexpr static const uint32_t TRANSFORMS_POOL_SIZE_ = 256;
			glm::mat4* transforms = nullptr;
			glm::mat4* currentTransformsEnd = nullptr;

			TransformsPool_();
			~TransformsPool_();
			glm::mat4* const Allocate(void* data, const uint32_t byteLen);
			void FreeTransforms(glm::mat4* const begin, glm::mat4* const end);
		};

		friend class Engine;
		sge_ALLOW_CONSTRUCTION(ResourceManager);

		constexpr static const bool FREE_DATA_POST_INIT_ = true;

		std::vector<Resource<JsonData>> jsons_ = {};
		std::vector<Resource<KtxData>> ktxs_ = {};
		std::vector<Resource<ShaderData>> shaderSrcs_ = {};
		std::vector<Resource<GltfData>> gltfs_ = {};
		TransformsPool_ transforms = {};
	};
}//!sge