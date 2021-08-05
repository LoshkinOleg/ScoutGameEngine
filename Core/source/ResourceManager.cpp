#include "ResourceManager.h"

#include <assert.h>
#include <fstream>
#include <filesystem>
#include <iostream>

#include <xxhash.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
using json = nlohmann::json;
#include <tiny_gltf.h>

#include "Engine.h"

using Gltf = tinygltf::Model;

namespace sge
{
	void JsonData::Clear()
	{
		hash = 0;
		json = nlohmann::json::value_t::discarded;
	}

	bool JsonData::operator==(const JsonData& other) const
	{
		return hash == other.hash;
	}

	void JsonHandle::Clear()
	{
		resourceIndex = 0;
		resourceHash = 0;
	}

	JsonData* JsonHandle::operator->()
	{
		assert(resourceHash > 0);
		return &Engine::Get().GetResourceManager().GetJsonData(resourceIndex, resourceHash);
	}

	void GltfData::InitHeap()
	{
		gltf = new Gltf;
	}

	void GltfData::Clear()
	{
		hash = 0;
		delete gltf;
		gltf = nullptr;
	}

	bool GltfData::operator==(const GltfData& other) const
	{
		return hash == other.hash;
	}

	void GltfHandle::Clear()
	{
		resourceIndex = 0;
		resourceHash = 0;
	}

	GltfData* GltfHandle::operator->()
	{
		assert(resourceHash > 0);
		return &Engine::Get().GetResourceManager().GetGltfData(resourceIndex, resourceHash);
	}

	void ResourceManager::PostInit()
	{
		if (FREE_DATA_AT_INIT_) jsons_.resize(0);
	}

	JsonData& ResourceManager::GetJsonData(const uint32_t resourceIndex, const uint32_t resourceHash)
	{
		assert(resourceHash > 0);
		JsonData& resource = jsons_[resourceIndex];
		assert(resource.hash == resourceHash);
		return resource;
	}

	JsonHandle ResourceManager::LoadJson(const std::string_view path)
	{
		assert(path.substr(path.find_last_of('.'), path.size()) == ".json"); // Make sure we're loading a .json
		auto match = std::find(jsons_.begin(), jsons_.end(), JsonData()); // Find free resource slot.
		assert(match != jsons_.end());
		JsonData& data = *match;

		const std::string str = LoadFile_(path);
		data.json = json::parse(str);
		assert(data.json != json::value_t::discarded);
		data.hash = HashString_(str);
		
		return { (uint32_t)(match - jsons_.begin()) , data.hash };
	}

	GltfData& ResourceManager::GetGltfData(const uint32_t resourceIndex, const uint32_t resourceHash)
	{
		assert(resourceHash > 0);
		GltfData& resource = gltfs_[resourceIndex];
		assert(resource.hash == resourceHash);
		return resource;
	}

	GltfHandle ResourceManager::LoadGltf(const std::string_view path)
	{
		assert(path.substr(path.find_last_of('.'), path.size()) == ".glb"); // Make sure we're loading a .glb
		assert(std::filesystem::exists(path));
		auto match = std::find(gltfs_.begin(), gltfs_.end(), GltfData());
		assert(match != gltfs_.end());
		GltfData& data = *match;
		data.InitHeap();

		tinygltf::TinyGLTF loader;
		std::string error, warning;
		bool success = false;

		success = loader.LoadBinaryFromFile(data.gltf, &error, &warning, path.data());
		if (!warning.empty())
		{
			sge_WARNING(warning);
		}
		if (!error.empty())
		{
			sge_ERROR(error);
		}
		else if(!success)
		{
			sge_ERROR("Tried loading: " << path << " but TinyGLTF loader failed to load file without providing any details.");
		}

		data.hash = HashGltf_(data.gltf);

		return {(uint32_t)(match - gltfs_.begin()), data.hash};
	}

	void ResourceManager::FreeJson(JsonHandle handle)
	{
		handle->Clear();
		handle.Clear();
	}

	void ResourceManager::FreeGltf(GltfHandle handle)
	{
		handle->Clear();
		handle.Clear();
	}

	KtxData& ResourceManager::GetKtxData(const uint32_t resourceIndex, const uint32_t resourceHash)
	{
		assert(resourceHash > 0);
		KtxData& resource = ktxs_[resourceIndex];
		assert(resource.hash == resourceHash);
		return resource;
	}

	KtxHandle ResourceManager::LoadKtx(const std::string_view path)
	{
		assert(path.substr(path.find_last_of('.'), path.size()) == ".ktx"); // Make sure we're loading a .ktx
		assert(std::filesystem::exists(path));
		auto match = std::find(ktxs_.begin(), ktxs_.end(), KtxData());
		assert(match != ktxs_.end());
		KtxData& data = *match;

		data.texture = gli::load(path.data());
		assert(!data.texture.empty());
		data.hash = HashKtx_(data.texture);

		return { (uint32_t)(match - ktxs_.begin()), data.hash };
	}

	void ResourceManager::FreeKtx(KtxHandle handle)
	{
		handle->Clear();
		handle.Clear();
	}

	ShaderData& ResourceManager::GetShaderData(const uint32_t resourceIndex, const uint32_t resourceHash)
	{
		assert(resourceHash > 0);
		ShaderData& resource = shaderSrcs_[resourceIndex];
		assert(resource.hash == resourceHash);
		return resource;
	}

	ShaderSrcHandle ResourceManager::LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath)
	{
		assert(vertexPath.substr(vertexPath.find_last_of('.'), vertexPath.size()) == ".vert");
		assert(fragmentPath.substr(fragmentPath.find_last_of('.'), fragmentPath.size()) == ".frag");

		auto match = std::find(shaderSrcs_.begin(), shaderSrcs_.end(), ShaderData());
		assert(match != shaderSrcs_.end());
		ShaderData& data = *match;

		data.vertexShader = LoadFile_(vertexPath);
		data.fragmentShader = LoadFile_(fragmentPath);
		const std::string combinedHash = std::to_string(HashString_(data.vertexShader)) + std::to_string(HashString_(data.fragmentShader));
		data.hash = XXH32(combinedHash.c_str(), combinedHash.size(), HASHING_SEED);

		return { (uint32_t)(match - shaderSrcs_.begin()), data.hash };
	}

	void ResourceManager::FreeShader(ShaderSrcHandle handle)
	{
		handle->Clear();
		handle.Clear();
	}

	void ResourceManager::Shutdown()
	{
		for (auto& element : jsons_) element.Clear();
		for (auto& element : gltfs_) element.Clear();
		for (auto& element : ktxs_) element.Clear();
		for (auto& element : shaderSrcs_) element.Clear();
	}

	std::string ResourceManager::LoadFile_(const std::string_view path) const
	{
		assert(std::filesystem::exists(path));

		std::ifstream stream(path.data());
		return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	}

	uint32_t ResourceManager::HashString_(const std::string_view str) const
	{
		assert(sizeof(char) == 1);
		return XXH32(str.data(), str.size(), HASHING_SEED);
	}
	uint32_t ResourceManager::HashGltf_(const tinygltf::Model* gltf) const
	{
		// TODO: this only works if the model is kept loaded in memory. The same file loaded twice into memory will result in different hashes since tinygltf::Model uses std::vectors.
		return XXH32(gltf, sizeof(tinygltf::Model), HASHING_SEED);
	}
	uint32_t ResourceManager::HashKtx_(const gli::texture& ktx) const
	{
		return XXH32(ktx.data(), ktx.size(), HASHING_SEED);
	}
	void KtxData::Clear()
	{
		hash = 0;
		texture = gli::texture();
	}
	bool KtxData::operator==(const KtxData & other) const
	{
		return hash == other.hash;
	}
	int32_t KtxData::GetGlInternalFormat() const
	{
		return gli::gl(gli::gl::PROFILE_KTX).translate(texture.format(), texture.swizzles()).Internal;
	}
	int32_t KtxData::GetGlExternalFormat() const
	{
		return gli::gl(gli::gl::PROFILE_KTX).translate(texture.format(), texture.swizzles()).External;
	}
	int32_t KtxData::GetGlTarget() const
	{
		return gli::gl(gli::gl::PROFILE_KTX).translate(texture.target());
	}
	KtxData* KtxHandle::operator->()
	{
		assert(resourceHash > 0);
		return &Engine::Get().GetResourceManager().GetKtxData(resourceIndex, resourceHash);
	}
	void KtxHandle::Clear()
	{
		resourceIndex = 0;
		resourceHash = 0;
	}
	ShaderData* ShaderSrcHandle::operator->()
	{
		return &Engine::Get().GetResourceManager().GetShaderData(resourceIndex, resourceHash);
	}
	void ShaderSrcHandle::Clear()
	{
		resourceIndex = 0;
		resourceHash = 0;
	}
	void ShaderData::Clear()
	{
		hash = 0;
		vertexShader = "";
		fragmentShader = "";
	}
	bool ShaderData::operator==(const ShaderData & other) const
	{
		return hash == other.hash;
	}
}//!sge