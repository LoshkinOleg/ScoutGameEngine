#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include "EnumsAndFlags.h"
#include "Containers.h"

namespace sge
{
	class JsonData: public I_Validateable
	{
	public:
		json data = json::value_t::discarded;

		bool IsValid() const override;
	};
	class UncompressedByteTexData: public I_Validateable
	{
	public:
		int width = 0, height = 0, nrOfChannels = 0;
		unsigned char* data = nullptr;
		F_TextureType type = F_TextureType::INVALID;
	
		bool IsValid() const override;
	};
	class UncompressedFloatTexData: public I_Validateable
	{
	public:
		int width = 0, height = 0, nrOfChannels = 0;
		float* data = nullptr;
		F_TextureType type = F_TextureType::INVALID;

		bool IsValid() const override;
	};
	class KtxData: public I_Validateable
	{
	public:
		gli::texture data = {};
		F_TextureType type = F_TextureType::INVALID;

		bool IsValid() const override;
	};
	class ShaderData: public I_Validateable
	{
	public:
		std::string vertexCode = "";
		std::string fragmentCode = "";
		std::string geometryCode = "";

		bool IsValid() const override;
	};
	class ObjData: public I_Validateable
	{
	public:
		tinyobj::ObjReader reader = {};
		std::vector<HashableHandle<KtxData>> compressedImages = {}; // TODO: should be handles to generic texture data rather than a specific format.
		std::vector<HashableHandle<UncompressedByteTexData>> uncompressedByteImages = {};
		std::vector<HashableHandle<UncompressedFloatTexData>> uncompressedFloatImages = {};

		bool IsValid() const override;
	};
	class GltfData: public I_Validateable
	{
	public:
		tinygltf::Model model = {};
		std::vector<HashableHandle<KtxData>> compressedImages = {}; // TODO: should be handles to generic texture data rather than a specific format.
		std::vector<HashableHandle<UncompressedByteTexData>> uncompressedByteImages = {};
		std::vector<HashableHandle<UncompressedFloatTexData>> uncompressedFloatImages = {};

		bool IsValid() const override;
	};

	class ResourceManager
	{
		friend class Engine;
		friend class Renderer;
		constexpr static const bool FREE_ASSETS_POST_INIT_ = true;
		constexpr static const size_t JSON_POOL_SIZE_ = 8;
		constexpr static const size_t KTX_POOL_SIZE_ = 8;
		constexpr static const size_t UNCOMPRESSED_BYTE_TEXTURES_POOL_SIZE_ = 8;
		constexpr static const size_t UNCOMPRESSED_FLOAT_TEXTURES_POOL_SIZE_ = 8;
		constexpr static const size_t SHADER_SRC_POOL_SIZE_ = 8;
		constexpr static const size_t GLTF_POOL_SIZE_ = 8;
		constexpr static const size_t OBJ_POOL_SIZE_ = 8;
		constexpr static const size_t TRANSFORM_BUFFER_POOL_SIZE_ = 64;
		constexpr static const size_t MODEL_MATRIX_POOL_SIZE_ = 256;
		using JsonContainer = StaticHashableVector<HashableResource<JsonData>, JSON_POOL_SIZE_>;
		using KtxContainer = StaticHashableVector<HashableResource<KtxData>, KTX_POOL_SIZE_>;
		using RawByteImageContainer = StaticHashableVector<HashableResource<UncompressedByteTexData>, UNCOMPRESSED_BYTE_TEXTURES_POOL_SIZE_>;
		using RawFloatImageContainer = StaticHashableVector<HashableResource<UncompressedFloatTexData>, UNCOMPRESSED_FLOAT_TEXTURES_POOL_SIZE_>;
		using ShaderSrcContainer = StaticHashableVector<HashableResource<ShaderData>, SHADER_SRC_POOL_SIZE_>;
		using GltfContainer = StaticHashableVector<HashableResource<GltfData>, GLTF_POOL_SIZE_>;
		using ObjContainer = StaticHashableVector<HashableResource<ObjData>, OBJ_POOL_SIZE_>;
		using TransformHandlesContainer = StaticHashlessVector<HashlessResource<TransformsBuffer>, TRANSFORM_BUFFER_POOL_SIZE_>;

	public:
		using RawByteImage = UncompressedByteTexData;
		using RawFloatImage = UncompressedFloatTexData;

		HashableHandle<JsonData> LoadJson(const std::string_view path);
		HashableHandle<KtxData> LoadKtx(const std::string_view path);
		HashableHandle<RawByteImage> LoadUncompressedByteImage(const std::string_view path);
		HashableHandle<RawFloatImage> LoadUncompressedFloatImage(const std::string_view path);
		HashableHandle<ShaderData> LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath, const std::string_view geometryPath);
		HashableHandle<GltfData> LoadGltf(const std::string_view path);

		static Model::Definition GenerateModelDefinitionFrom(const HashableHandle<GltfData>& handle, const F_3dFileAttributes relevantData, const F_ShadingMode shadingModesNeeded);
		static Model::Definition GenerateModelDefinitionFrom(const HashableHandle<ObjData>& handle, const F_3dFileAttributes relevantData, const F_ShadingMode shadingModesNeeded);

		static Texture::Definition GenerateTextureDefinitionFrom(const HashableHandle<KtxData>& handle,
																 Pair<E_SamplingMode, E_SamplingMode> samplingModes,
																 Pair<E_WrappingMode, E_WrappingMode> wrappingModes); // TODO: assert that mipmaps levels > 1
		static Texture::Definition GenerateTextureDefinitionFrom(const HashableHandle<RawByteImage>& handle,
																 Pair<E_SamplingMode, E_SamplingMode> samplingModes,
																 Pair<E_WrappingMode, E_WrappingMode> wrappingModes,
																 const bool generateMipMaps);
		static Texture::Definition GenerateTextureDefinitionFrom(const HashableHandle<RawFloatImage>& handle,
																 Pair<E_SamplingMode, E_SamplingMode> samplingModes,
																 Pair<E_WrappingMode, E_WrappingMode> wrappingModes,
																 const bool generateMipMaps);

		HashlessHandle<TransformsBuffer> CreateTransformsBuffer(const std::vector<glm::mat4>& transforms);

		uint32_t GetMaxNrOfModelMatrices() const;
		const glm::mat4& GetViewMatrix() const;

	private:
		JsonContainer jsons_ = {};
		KtxContainer ktxs_ = {};
		RawByteImageContainer byteImages_ = {};
		RawFloatImageContainer floatImages_ = {};
		ShaderSrcContainer shaderSrcs_ = {};
		GltfContainer gltfs_ = {};
		ObjContainer objs_ = {};
		TransformHandlesContainer transformsHandles_ = {};

		ModelMatrixPool modelMatrixPool_ = {};
		glm::mat4 viewMatrix_ = DEFAULT_VIEW_MATRIX;

		void Init();
		void FreeAssetResources();
		void Shutdown();

		glm::mat4* AllocateModelMatrices(const uint32_t nrOfTransforms);

		static std::string LoadFile_(const std::string_view path);
	};
}//!sge