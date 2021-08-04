#pragma once

#include <vector>
#include <utility>
#include <string_view>
#include <string>

#include <nlohmann/json.hpp>
#include <gli/gli.hpp>

#include "macros.h"

namespace tinygltf
{
	class Model;
}

namespace sge
{
	struct JsonData
	{
		uint32_t hash = 0;
		nlohmann::json json = nlohmann::json::value_t::discarded;

		void Clear();
		bool operator==(const JsonData& other) const;
	};

	struct JsonHandle
	{
		uint32_t resourceIndex = 0;
		uint32_t resourceHash = 0;

		JsonData* operator->();
		void Clear();
	};

	struct KtxData
	{
		uint32_t hash = 0;
		gli::texture texture = gli::texture();

		void Clear();
		bool operator==(const KtxData& other) const;
	};

	struct KtxHandle
	{
		uint32_t resourceIndex = 0;
		uint32_t resourceHash = 0;

		KtxData* operator->();
		void Clear();
	};

	struct GltfData
	{
		uint32_t hash = 0;
		tinygltf::Model* gltf = nullptr;

		void InitHeap();
		void Clear();
		bool operator==(const GltfData& other) const;
	};

	struct GltfHandle
	{
		uint32_t resourceIndex = 0;
		uint32_t resourceHash = 0;

		GltfData* operator->();
		void Clear();
	};

	struct ShaderData
	{
		uint32_t hash = 0;
		std::string vertexShader = "";
		std::string fragmentShader = "";

		void Clear();
		bool operator==(const ShaderData& other) const;
	};

	struct ShaderHandle
	{
		uint32_t resourceIndex = 0;
		uint32_t resourceHash = 0;

		ShaderData* operator->();
		void Clear();
	};

	class ResourceManager
	{
	public:
		sge_DISALLOW_COPY(ResourceManager);

		void Init();

		JsonData& GetJsonData(const uint32_t resourceIndex, const uint32_t resourceHash);
		JsonHandle LoadJson(const std::string_view path);
		void FreeJson(JsonHandle handle);

		GltfData& GetGltfData(const uint32_t resourceIndex, const uint32_t resourceHash);
		GltfHandle LoadGltf(const std::string_view path);
		void FreeGltf(GltfHandle handle);

		KtxData& GetKtxData(const uint32_t resourceIndex, const uint32_t resourceHash);
		KtxHandle LoadKtx(const std::string_view path);
		void FreeKtx(KtxHandle handle);

		ShaderData& GetShaderData(const uint32_t resourceIndex, const uint32_t resourceHash);
		ShaderHandle LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath);
		void FreeShader(ShaderHandle handle);

		void Shutdown();

	private:
		friend class Engine;
		sge_ALLOW_CONSTRUCTION(ResourceManager);

		std::string LoadFile_(const std::string_view path) const;
		uint32_t HashString_(const std::string_view str) const;
		uint32_t HashGltf_(const tinygltf::Model* gltf) const;
		uint32_t HashKtx_(const gli::texture& ktx) const;

		constexpr static const uint32_t SIZE_OF_RESOURCE_POOLS_ = 8;
		constexpr static const uint32_t SIZE_OF_HANDLES_POOL_ = 4 * SIZE_OF_RESOURCE_POOLS_;
		constexpr static const bool FREE_DATA_AT_INIT_ = true;

		std::vector<JsonData> jsons_ = std::vector<JsonData>(SIZE_OF_RESOURCE_POOLS_, JsonData());
		std::vector<GltfData> gltfs_ = std::vector<GltfData>(SIZE_OF_RESOURCE_POOLS_, GltfData());
		std::vector<KtxData> ktxs_ = std::vector<KtxData>(SIZE_OF_RESOURCE_POOLS_, KtxData());
		std::vector<ShaderData> shaders_ = std::vector<ShaderData>(SIZE_OF_RESOURCE_POOLS_, ShaderData());
	};
}//!sge