#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>

#include "Model.h"
#include "macros.h"

namespace sge
{
	struct JsonData
	{
		json data = json::value_t::discarded;

		inline bool IsValid() const
		{
			return data != json::value_t::discarded;
		}
	};
	struct KtxData
	{
		gli::texture data = {};
		ImageType type = ImageType::INVALID;
		Hash associatedMesh = 0;

		inline bool IsValid() const
		{
			return !data.empty() && (uint32_t)type && associatedMesh.value;
		}
	};
	struct ShaderData
	{
		std::string vertexCode = "";
		std::string fragmentCode = "";
		std::string geometryCode = "";

		inline bool IsValid() const
		{
			return !vertexCode.empty() && vertexCode != "\0" && !fragmentCode.empty() && fragmentCode != "\0" && geometryCode != "\0";
		}
	};
	struct GltfData
	{
		enum class GltfAttributes: uint8_t
		{
			INVALID = 0,

			POSITIONS = 1 << 0,
			NORMALS = 1 << 1,
			TANGENTS = 1 << 2,
			UVS = 1 << 3,

			INDICES = 1 << 4,

			EVERYTHING = POSITIONS | NORMALS | TANGENTS | UVS,
			MAX_VALUE = INDICES
		};

		tinygltf::Model model = {};
		std::vector<Handle<KtxData>> images = {};

		bool IsValid() const;
	};

	class ResourceManager
	{
	public:
		sge_DISALLOW_COPY(ResourceManager);

		void Init();
		void FreeAssetResources();

		Handle<JsonData> LoadJson(const std::string_view path);
		Handle<KtxData> LoadKtx(const std::string_view path);
		Handle<ShaderData> LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath, const std::string_view geometryPath);
		Handle<GltfData> LoadGltf(const std::string_view path);

		Model::Definition GenerateDefinitionFrom(const Handle<GltfData>& handle, const GltfData::GltfAttributes relevantData, const ShadingMode shadingModesNeeded) const;
		Texture::Definition GenerateDefinitionFrom(const Handle<KtxData>& handle,
												   const Texture::SamplingMode minifyingMode,
												   const Texture::SamplingMode magnifyingMode,
												   const Texture::WrappingMode onS,
												   const Texture::WrappingMode onT,
												   const Mutability mutability,
												   const bool generateMipMaps) const;

		TransformsBuffer AllocateTransforms(const void* const data, const uint32_t byteLen);

		void Shutdown();

	private:
		struct TransformsPool_
		{
			sge_DISALLOW_COPY(TransformsPool_);

			constexpr static const uint32_t TRANSFORMS_POOL_SIZE = 256;
			glm::mat4* transforms = nullptr;
			glm::mat4* currentTransformsEnd = nullptr;

			TransformsPool_();
			~TransformsPool_();
			glm::mat4* const Allocate(const uint32_t nrOfTransforms);
		};

		friend class Engine;
		sge_ALLOW_CONSTRUCTION(ResourceManager);

		constexpr static const bool FREE_DATA_POST_INIT_ = true;

		std::vector<Resource<JsonData>> jsons_ = {};
		std::vector<Resource<KtxData>> ktxs_ = {};
		std::vector<Resource<ShaderData>> shaderSrcs_ = {};
		std::vector<Resource<GltfData>> gltfs_ = {};
		TransformsPool_ transformsPool_ = {};

		static std::string LoadFile_(const std::string_view path);
	};
}//!sge