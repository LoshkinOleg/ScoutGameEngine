#include "ResourceManager.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include <xxhash.h>
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include <gli/gli.hpp>

#include "macros.h"
#include "globals.h"

namespace sge
{
	void ResourceManager::Init()
	{
		transforms_ = new glm::mat4[TRANSFORMS_POOL_SIZE_];
		currentTransformsEnd_ = transforms_;
		for (size_t i = 0; i < TRANSFORMS_POOL_SIZE_; i++)
		{
			transforms_[i] = IDENTITY_MAT4;
		}
	}
	void ResourceManager::PostInit()
	{
		if (FREE_DATA_POST_INIT_)
		{
			jsons_.Clear();
			gltfs_.Clear();
			ktxs_.Clear();
			shaderSrcs_.Clear();
		}
	}

	JsonDataHandle ResourceManager::LoadJson(const std::string_view path)
	{
		const std::string_view extension = path.substr(path.find_last_of('.'), path.size());
		assert(extension == ".json"); // Make sure we're loading a .json
		if (extension == ".gltf") { sge_ERROR("Please use LoadGltf to load composite gltf files instead."); };

		JsonData newElement;

		const std::string str = LoadFile_(path);
		newElement.data = json::parse(str);
		assert(newElement.data != json::value_t::discarded);
		newElement.hash = HashString_(str);
		assert(newElement.hash > 0);

		jsons_.Insert(newElement);
		JsonDataHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	GltfDataHandle ResourceManager::LoadGltf(const std::string_view path)
	{
		const std::string_view extension = path.substr(path.find_last_of('.'), path.size());
		assert(extension == ".glb" || extension == ".gltf"); // Make sure we're loading a gltf file.
		assert(std::filesystem::exists(path));

		GltfData newElement;

		tinygltf::TinyGLTF loader;
		std::string error, warning;
		bool success = false;

		if (extension == ".glb") // Loading a binary only.
		{
			success = loader.LoadBinaryFromFile(&newElement.model, &error, &warning, path.data());
		}
		else // Loading a composite gltf file.
		{
			success = loader.LoadASCIIFromFile(&newElement.model, &error, &warning, path.data());
		}

		if (!warning.empty())
		{
			sge_WARNING(warning);
		}
		if (!error.empty())
		{
			sge_ERROR(error);
		}
		else if (!success)
		{
			sge_ERROR("Tried loading: " << path << " but TinyGLTF loader failed to load file without providing any details.");
		}

		assert(newElement.model.images.size() == 4);
		// assert(newElement.model.images[0].as_is && newElement.model.images[1].as_is && newElement.model.images[2].as_is && newElement.model.images[3].as_is);
		assert(newElement.model.images[0].name == "alphaMap" && newElement.model.images[1].name == "albedoMap" && newElement.model.images[2].name == "specularMap" && newElement.model.images[3].name == "normalMap");
		assert(!newElement.model.images[0].uri.empty() && !newElement.model.images[1].uri.empty()&& !newElement.model.images[2].uri.empty()&& !newElement.model.images[3].uri.empty());
		newElement.alphaMap = LoadKtx(newElement.model.images[0].uri);
		newElement.albedoMap = LoadKtx(newElement.model.images[1].uri);
		newElement.specularMap = LoadKtx(newElement.model.images[2].uri);
		newElement.normalMap = LoadKtx(newElement.model.images[3].uri);

		newElement.hash = HashGltf_(newElement.model, {newElement.alphaMap, newElement.albedoMap, newElement.specularMap, newElement.normalMap });
		assert(newElement.hash > 0);

		gltfs_.Insert(newElement);
		GltfDataHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	KtxDataHandle ResourceManager::LoadKtx(const std::string_view path)
	{
		assert(path.substr(path.find_last_of('.'), path.size()) == ".ktx"); // Make sure we're loading a .ktx
		assert(std::filesystem::exists(path));
		
		KtxData newElement;

		newElement.data = gli::load(path.data());
		assert(!newElement.data.empty());
		newElement.hash = HashKtx_(newElement.data);
		assert(newElement.hash > 0);

		ktxs_.Insert(newElement);
		KtxDataHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	ShaderDataHandle ResourceManager::LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath)
	{
		assert(vertexPath.substr(vertexPath.find_last_of('.'), vertexPath.size()) == ".vert");
		assert(fragmentPath.substr(fragmentPath.find_last_of('.'), fragmentPath.size()) == ".frag");

		ShaderData newElement;

		newElement.vertexCode = LoadFile_(vertexPath);
		newElement.fragmentCode = LoadFile_(fragmentPath);
		assert(!newElement.vertexCode.empty() && !newElement.fragmentCode.empty());
		const std::string combinedHash = std::to_string(HashString_(newElement.vertexCode)) + std::to_string(HashString_(newElement.fragmentCode));
		newElement.hash = XXH32(combinedHash.c_str(), combinedHash.size(), HASHING_SEED);
		assert(newElement.hash > 0);

		shaderSrcs_.Insert(newElement);
		ShaderDataHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	void ResourceManager::FreeJson(const JsonDataHandle& handle)
	{
		// Note: this does not free up the space occupied by map! Only jsons_.Clear() resizes the map!
		jsons_.Access(handle).Destroy();
	}

	void ResourceManager::FreeGltf(GltfDataHandle handle)
	{
		gltfs_.Access(handle).Destroy();
	}

	void ResourceManager::FreeKtx(KtxDataHandle handle)
	{
		ktxs_.Access(handle).Destroy();
	}

	void ResourceManager::FreeShader(ShaderDataHandle handle)
	{
		shaderSrcs_.Access(handle).Destroy();
	}

	JsonData& ResourceManager::GetJsonData(const JsonDataHandle & handle)
	{
		return jsons_.Access(handle);
	}

	KtxData& ResourceManager::GetKtxData(const KtxDataHandle& handle)
	{
		return ktxs_.Access(handle);
	}

	GltfData& ResourceManager::GetGltfData(const GltfDataHandle& handle)
	{
		return gltfs_.Access(handle);
	}

	ShaderData& ResourceManager::GetShaderData(const ShaderDataHandle& handle)
	{
		return shaderSrcs_.Access(handle);
	}

	glm::mat4* const ResourceManager::AllocateTransforms(const std::vector<glm::mat4>& transforms)
	{
		glm::mat4* const returnVal = currentTransformsEnd_;

		const auto len = transforms.size();
		size_t i = 0;
		for (; currentTransformsEnd_ < returnVal + len; currentTransformsEnd_++)
		{
			*currentTransformsEnd_ = transforms[i++];
		}

		return returnVal;
	}

	void ResourceManager::FreeTransforms(glm::mat4* const begin, glm::mat4* const end)
	{
		for (glm::mat4* it = begin; it < end; it++)
		{
			*it = IDENTITY_MAT4;
		}

		// TODO: Fix fragmentation.
	}
	
	void ResourceManager::Shutdown()
	{
		currentTransformsEnd_ = nullptr;
		delete[] transforms_;
		transforms_ = nullptr;
		jsons_.Clear();
		gltfs_.Clear();
		ktxs_.Clear();
		shaderSrcs_.Clear();
	}

	std::string ResourceManager::LoadFile_(const std::string_view path)
	{
		assert(std::filesystem::exists(path));

		std::ifstream stream(path.data());
		return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	}

	uint32_t ResourceManager::HashString_(const std::string_view str)
	{
		assert(sizeof(char) == 1);
		return XXH32(str.data(), str.size(), HASHING_SEED);
	}

	uint32_t ResourceManager::HashGltf_(const tinygltf::Model& model, const std::array<const KtxDataHandle, 4> textures)
	{
		const uint32_t alphaMapHash = textures[0]->hash;
		const uint32_t alebdoMapHash = textures[1]->hash;
		const uint32_t specularMapHash = textures[2]->hash;
		const uint32_t normalMapHash = textures[3]->hash;

		// TODO: this doesn't work since model uses std::vectors.
		const uint32_t modelHash = XXH32(&model, sizeof(tinygltf::Model), HASHING_SEED);

		std::string accumulatedData = "";
		accumulatedData += std::to_string(modelHash);
		accumulatedData += std::to_string(alphaMapHash);
		accumulatedData += std::to_string(alebdoMapHash);
		accumulatedData += std::to_string(specularMapHash);
		accumulatedData += std::to_string(normalMapHash);

		// TODO: take into account shininess?

		return XXH32(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED);
	}

	uint32_t ResourceManager::HashKtx_(const gli::texture& ktx)
	{
		return XXH32(ktx.data(), ktx.size(), HASHING_SEED);
	}
}//!sge