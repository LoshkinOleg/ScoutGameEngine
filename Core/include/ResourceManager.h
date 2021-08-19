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
		Hash associatedMesh = 0; // TODO: shouldn't be here. Makes no sense for an image to have a reference to a mesh.

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
			return !vertexCode.empty() && !fragmentCode.empty();
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

			EVERYTHING = POSITIONS | NORMALS | TANGENTS | UVS | INDICES,
			MAX_VALUE = INDICES
		};

		tinygltf::Model model = {};
		std::vector<UniqueResourceHandle<KtxData>> images = {};

		bool IsValid() const;
	};

	class ResourceManager
	{
	public:
		sge_DISALLOW_COPY(ResourceManager);

		void Init();
		void FreeAssetResources();

		UniqueResourceHandle<JsonData> LoadJson(const std::string_view path);
		UniqueResourceHandle<KtxData> LoadKtx(const std::string_view path);
		UniqueResourceHandle<ShaderData> LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath, const std::string_view geometryPath);
		UniqueResourceHandle<GltfData> LoadGltf(const std::string_view path);

		Model::Definition GenerateDefinitionFrom(const UniqueResourceHandle<GltfData>& handle, const GltfData::GltfAttributes relevantData, const ShadingMode shadingModesNeeded) const;
		Texture::Definition GenerateDefinitionFrom(const UniqueResourceHandle<KtxData>& handle,
												   const Texture::SamplingMode minifyingMode,
												   const Texture::SamplingMode magnifyingMode,
												   const Texture::WrappingMode onS,
												   const Texture::WrappingMode onT,
												   const Mutability mutability,
												   const bool generateMipMaps) const;

		HashlessResourceHandle<TransformsBuffer> CreateTransformsBuffer(const std::vector<glm::mat4>& transforms);
		glm::mat4* AllocateModelMatrices(const uint32_t nrOfTransforms);
		glm::mat4* GetModelMatricesBegin();
		uint32_t GetMaxNrOfModelMatrices() const;

		void Shutdown();

	private:

		friend class Engine;
		sge_ALLOW_CONSTRUCTION(ResourceManager);

		constexpr static const bool FREE_ASSETS_POST_INIT_ = true;
		constexpr static const size_t JSON_POOL_SIZE_ = 32;
		constexpr static const size_t KTX_POOL_SIZE_ = 64;
		constexpr static const size_t SHADER_SRC_POOL_SIZE_ = 16;
		constexpr static const size_t GLTF_POOL_SIZE_ = 16;
		constexpr static const size_t TRANSFORM_BUFFER_POOL_SIZE_ = 64;
		constexpr static const size_t MODEL_MATRIX_POOL_SIZE_ = 256;

		// WARNING: these are free to relocate as they wish since it's using a standard allocator! Reserve the memory on init to avoid that behaviour!
		std::vector<UniqueResource<JsonData>> jsons_ = {};
		std::vector<UniqueResource<KtxData>> ktxs_ = {};
		std::vector<UniqueResource<ShaderData>> shaderSrcs_ = {};
		std::vector<UniqueResource<GltfData>> gltfs_ = {};
		std::vector<HashlessResource<TransformsBuffer>> transformBuffers_ = {};
		ModelMatrixPool modelMatrixPool_ = {};

		static std::string LoadFile_(const std::string_view path);
		template<typename Type>
		static bool ElementExists_(const std::vector<UniqueResource<Type>>& list, const Hash hash)
		{
			for(const auto& element : list)
			{
				if(element.hash == hash)
				{
					return true;
				}
			}
			return false;
		}
		template<typename Type>
		static UniqueResource<Type>& GetElement_(std::vector<UniqueResource<Type>>& list, const Hash hash)
		{
			for(auto& element : list)
			{
				if(element.hash == hash)
				{
					return element;
				}
			}
			sge_ERROR("Element with the specified hash was not found in the list provided!");
		}
	};
}//!sge