#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>

#include "EnumsAndFlags.h"
#include "Resources.h"
#include "Transform.h"
#include "Model.h"

// TODO: create a custom container to use instead of the std::vector.

namespace sge
{
	class JsonData: public I_Validateable
	{
	public:
		json data = json::value_t::discarded;

		bool IsValid() const override;
	};
	// TODO: implement import of png and jpg textures.
	// class UncompressedTextureData: public I_Validateable
	// {
	// public:
	// 	// std_image data = nullptr;
	// 	TextureType type = TextureType::INVALID;
	// 
	// 	bool IsValid() const override;
	// };
	class KtxData: public I_Validateable
	{
	public:
		gli::texture data = {};
		TextureType type = TextureType::INVALID;

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
	class GltfData: public I_Validateable
	{
	public:
		tinygltf::Model model = {};
		std::vector<UniqueResourceHandle<KtxData>> compressedImages = {}; // TODO: should be handles to generic texture data rather than a specific format.
		// std::vector<UniqueResourceHandle<UncompressedTextureData>> uncompressedImages = {};

		bool IsValid() const override;
	};

	class ResourceManager
	{
		friend class Engine;
		constexpr static const bool FREE_ASSETS_POST_INIT_ = true;
		constexpr static const size_t JSON_POOL_SIZE_ = 32;
		constexpr static const size_t KTX_POOL_SIZE_ = 64;
		constexpr static const size_t SHADER_SRC_POOL_SIZE_ = 16;
		constexpr static const size_t GLTF_POOL_SIZE_ = 16;
		constexpr static const size_t TRANSFORM_BUFFER_POOL_SIZE_ = 64;
		constexpr static const size_t MODEL_MATRIX_POOL_SIZE_ = 256;

	public:
		UniqueResourceHandle<JsonData> LoadJson(const std::string_view path);
		UniqueResourceHandle<KtxData> LoadKtx(const std::string_view path);
		// UniqueResourceHandle<UncompressedTextureData> LoadUncompressedImage(const std::string_view path);
		UniqueResourceHandle<ShaderData> LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath, const std::string_view geometryPath);
		UniqueResourceHandle<GltfData> LoadGltf(const std::string_view path);

		static Model::Definition GenerateDefinitionFrom(const UniqueResourceHandle<GltfData>& handle, const GltfAttributes relevantData, const ShadingMode shadingModesNeeded);
		static Texture::Definition GenerateDefinitionFrom(const UniqueResourceHandle<KtxData>& handle,
												   const Texture::SamplingMode minifyingMode,
												   const Texture::SamplingMode magnifyingMode,
												   const Texture::WrappingMode onS,
												   const Texture::WrappingMode onT,
												   const Mutability mutability,
												   const bool generateMipMaps);
		// static Texture::Definition GenerateDefinitionFrom(const UniqueResourceHandle<UncompressedTextureData>& handle,
		// 												  const Texture::SamplingMode minifyingMode,
		// 												  const Texture::SamplingMode magnifyingMode,
		// 												  const Texture::WrappingMode onS,
		// 												  const Texture::WrappingMode onT,
		// 												  const Mutability mutability,
		// 												  const bool generateMipMaps);

		HashlessHandle<TransformsBuffer> CreateTransformsBuffer(const std::vector<glm::mat4>& transforms);
		glm::mat4* AllocateModelMatrices(const uint32_t nrOfTransforms);
		glm::mat4* GetModelMatricesBegin();
		uint32_t GetMaxNrOfModelMatrices() const;

		const glm::mat4& GetViewMatrix() const;

	private:
		std::vector<UniqueResource<JsonData>> jsons_ = {};
		std::vector<UniqueResource<KtxData>> ktxs_ = {};
		// std::vector<UniqueResource<UncompressedTextureData>> uncompressedImages_ = {};
		std::vector<UniqueResource<ShaderData>> shaderSrcs_ = {};
		std::vector<UniqueResource<GltfData>> gltfs_ = {};
		std::vector<HashlessResource<TransformsBuffer>> transformBuffers_ = {};

		ModelMatrixPool modelMatrixPool_ = {};
		glm::mat4 viewMatrix_ = DEFAULT_VIEW_MATRIX;

		void Init_();
		void FreeAssetResources_();
		void Shutdown_();

		static std::string LoadFile_(const std::string_view path);
		// TODO: make own map container that checks that stuff automatically.
		template<typename Type> static bool ElementExists_(const std::vector<UniqueResource<Type>>& list, const Hash hash);
		template<typename Type> static UniqueResource<Type>& GetElement_(std::vector<UniqueResource<Type>>& list, const Hash hash);
	};
}//!sge