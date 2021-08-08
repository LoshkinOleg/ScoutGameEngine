#include "ResourceManager.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <filesystem>

#include <xxhash.h>

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

		assert(newElement.model != tinygltf::Model());
		newElement.hash = HashGltf_(newElement.model);
		assert(newElement.hash > 0);

		// TODO: load textures as well.

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

	glm::mat4* const ResourceManager::AllocateTransforms(const uint32_t nrOfTransforms)
	{
		glm::mat4* const returnVal = currentTransformsEnd_;
		currentTransformsEnd_ += nrOfTransforms;
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

	uint32_t ResourceManager::HashGltf_(const tinygltf::Model& model)
	{
		// TODO: this only works if the model is kept loaded in memory. The same file loaded twice into memory will result in different hashes since tinygltf::Model uses std::vectors.
		return XXH32(&model, sizeof(tinygltf::Model), HASHING_SEED);
	}

	uint32_t ResourceManager::HashKtx_(const gli::texture& ktx)
	{
		return XXH32(ktx.data(), ktx.size(), HASHING_SEED);
	}
}//!sge