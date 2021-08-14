#include "ResourceManager.h"

#include <filesystem>
#include <fstream>
#include <bit>

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include <glm/gtx/quaternion.hpp>

#include "globals.h"

namespace sge
{
	ResourceManager::TransformsPool_::TransformsPool_()
	{
		sge_MESSAGE("ResourceManager::TransformsPool_() called.");
		transforms = new glm::mat4[TRANSFORMS_POOL_SIZE];
		currentTransformsEnd = transforms;
		glm::mat4* it = transforms;
		for(glm::mat4* it = transforms; it < transforms + TRANSFORMS_POOL_SIZE; it++)
		{
			*it = IDENTITY_MAT4;
		}
	}
	ResourceManager::TransformsPool_::~TransformsPool_()
	{
		// Note: bit wierd this isn't called on shutdown... it is part of ResourceManager's resources after all.
		sge_MESSAGE("ResourceManager::~TransformsPool_() called.");
		delete[] transforms;
	}

	glm::mat4 * const ResourceManager::TransformsPool_::Allocate(const uint32_t nrOfTransforms)
	{
		currentTransformsEnd += nrOfTransforms;
		assert(currentTransformsEnd <= transforms + TRANSFORMS_POOL_SIZE);
		return transforms;
	}

	void ResourceManager::Init()
	{
		sge_MESSAGE("ResourceManager::Init() called.");
	}
	void ResourceManager::FreeAssetResources()
	{
		sge_MESSAGE("ResourceManager::FreeAssetResources() called.");
		if (FREE_DATA_POST_INIT_)
		{
			jsons_.clear();
			gltfs_.clear();
			ktxs_.clear();
			shaderSrcs_.clear();
		}
	}

	Handle<JsonData> ResourceManager::LoadJson(const std::string_view path)
	{
		const std::string_view extension = path.substr(path.find_last_of('.'), path.size());
		assert(extension == ".json"); // Make sure we're loading a .json
		if (extension == ".gltf") { sge_ERROR("Please use LoadGltf to load composite gltf files instead."); };

		jsons_.push_back(Resource<JsonData>());
		auto& newElement = jsons_.front();
		auto& newJson = newElement.resourceData;

		const std::string str = LoadFile_(path);

		newElement.hash = Hash(str.c_str(), str.length(), 0);
		newJson.data = json::parse(str);
		assert(newElement.hash.IsValid());
		assert(newJson.IsValid());

		Handle<JsonData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<GltfData> ResourceManager::LoadGltf(const std::string_view path)
	{
		assert(path.find_last_of('\\') == path.size()); // Forbid usage of backslashes in asset files.
		std::string_view fileStem, extension;
		{
			const size_t posOfLastDot = path.find_last_of('.');
			const size_t posOfLastSlash = path.find_last_of('/');
			assert(posOfLastDot != posOfLastSlash != 0);
			fileStem = path.substr(posOfLastSlash + 1, posOfLastDot);
			extension = path.substr(posOfLastDot, path.size());
			assert(extension == ".glb" || extension == ".gltf"); // Make sure we're loading a gltf file.
		}
		assert(std::filesystem::exists(path));

		gltfs_.push_back(Resource<GltfData>());
		auto& newElement = gltfs_.front();
		auto& newGltf = newElement.resourceData;

		tinygltf::TinyGLTF loader;
		std::string error, warning;
		bool success = false;

		if (extension == ".glb") // Loading a binary only.
		{
			success = loader.LoadBinaryFromFile(&newGltf.model, &error, &warning, path.data());
		}
		else // Loading a composite gltf file.
		{
			success = loader.LoadASCIIFromFile(&newGltf.model, &error, &warning, path.data());
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

		assert(newGltf.model.scenes.size() == 1); // Not handling multiple scenes.
		assert(newGltf.model.nodes.size() == newGltf.model.meshes.size()); // Handling only case where nr of nodes = nr of meshes.
		for(const auto& mesh : newGltf.model.meshes)
		{
			assert(mesh.primitives.size() == 1); // Handling only triangle base geometry.
			assert(mesh.primitives[0].attributes.size() == 4); // Expecting all data necessary for blinn-phong.
		}

		const uint32_t nrOfImages = newGltf.model.images.size();
		for(uint32_t i = 0; i < nrOfImages; i++)
		{
			const std::string_view imagePath = newGltf.model.images[i].uri;
			std::string_view imageStem, imageExtension;
			{
				assert((imagePath.find_last_of('\\') == imagePath.size()) &&
					   (imagePath.find_last_of('/') == imagePath.size())); // Forbid usage of directories. All images must be specified by name.

				const size_t posOfLastDot = path.find_last_of('.');
				const size_t posOfLastSlash = path.find_last_of('/');
				const size_t posOfLastUnderscore = path.find_last_of('_');
				assert(posOfLastDot != posOfLastSlash != 0);
				fileStem = path.substr(posOfLastSlash + 1, posOfLastDot);
				extension = path.substr(posOfLastDot, path.size());
				assert(extension == ".glb" || extension == ".gltf"); // Make sure we're loading a gltf file.
			}
			const std::string_view imageName = imagePath.substr(imagePath.find_last_of('/'), imagePath.size());
			const std::string_view extension = imagePath.substr(imagePath.find_last_of('.'), imagePath.size());
			if(extension == ".ktx")
			{
				newGltf.images.push_back(LoadKtx(sge_KTXS_PATH + fileStem.data() + imageName.data()));
				assert(newGltf.images.front()->IsValid() && newGltf.images.front().IsValid());
			}
			else if(extension == ".png")
			{
				sge_ERROR("Loading of pngs is not yet implemented!");
			}
			else if(extension == ".bmp")
			{
				sge_ERROR("Loading of bmps is not yet implemented!");
			}
			else if(extension == ".jpg" || extension == ".jpeg")
			{
				sge_ERROR("Loading of jpgs is not yet implemented!");
			}
			else
			{
				sge_ERROR("Unexpected extension encountered during loading of image from gltf!");
			}
		}
		assert(newGltf.IsValid());
		newElement.hash = Hash(path.data(), path.length(), 0);
		assert(newElement.IsValid());

		Handle<GltfData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		return handle;
	}

	Model::Definition ResourceManager::GenerateDefinitionFrom(const Handle<GltfData>& handle, const GltfAttributes relevantData, const ShadingMode requiredShadingModes)
	{
		assert(relevantData > 0); // Need at least something to load.

		Model::Definition modDef;
		modDef.transforms.push_back(IDENTITY_MAT4);
		tinygltf::Model& model = handle->model;

		const uint32_t nrOfMeshes = model.meshes.size();
		for(uint32_t meshIdx = 0; meshIdx < nrOfMeshes; meshIdx++)
		{
			modDef.meshDefs.push_back(IndexedMesh::Definition());
			auto& newMeshDef = modDef.meshDefs.front(); // TODO
			auto& newMaterialDefs = newMeshDef.matDefs; // TODO
			auto& newVboDefs = newMeshDef.vboDefs;
			auto& newEboDef = newMeshDef.eboDef;
			tinygltf::Mesh& mesh = model.meshes[meshIdx];

			assert(mesh.primitives.size() == 1 && mesh.primitives[0].attributes.size() == 4);

			auto& attributes = mesh.primitives[0].attributes;
			const auto indicesIdx = mesh.primitives[0].indices;
			const auto materialIdx = mesh.primitives[0].material;

			for(const auto& pair : attributes)
			{
				assert // Expecting the gltf to have all the data needed for normalmapped Blinn-Phong.
				(
					pair.first == "POSITION" ||
					pair.first == "NORMAL" ||
					pair.first == "TANGENT" ||
					pair.first == "TEXCOORD_0"
				);
			}

			glm::mat4 modelMatrix = (model.nodes[meshIdx].matrix.size() > 0) ?
				glm::mat4(
					model.nodes[meshIdx].matrix[0], model.nodes[meshIdx].matrix[1], model.nodes[meshIdx].matrix[2], model.nodes[meshIdx].matrix[3],
					model.nodes[meshIdx].matrix[4], model.nodes[meshIdx].matrix[5], model.nodes[meshIdx].matrix[6], model.nodes[meshIdx].matrix[7],
					model.nodes[meshIdx].matrix[8], model.nodes[meshIdx].matrix[9], model.nodes[meshIdx].matrix[10], model.nodes[meshIdx].matrix[11],
					model.nodes[meshIdx].matrix[12], model.nodes[meshIdx].matrix[13], model.nodes[meshIdx].matrix[14], model.nodes[meshIdx].matrix[15]) :
				IDENTITY_MAT4;

			if(modelMatrix == IDENTITY_MAT4)
			{
				const glm::vec3 translation = (model.nodes[meshIdx].translation.size() > 0) ?
					glm::vec3(model.nodes[meshIdx].translation[0], model.nodes[meshIdx].translation[1], model.nodes[meshIdx].translation[2]) :
					ZERO_VEC3;
				const glm::quat rotation = (model.nodes[meshIdx].rotation.size() > 0) ?
					glm::quat(model.nodes[meshIdx].rotation[0], model.nodes[meshIdx].rotation[1], model.nodes[meshIdx].rotation[2], model.nodes[meshIdx].rotation[3]) :
					IDENTITY_QUAT;
				const glm::vec3 scale = (model.nodes[meshIdx].scale.size() > 0) ?
					glm::vec3(model.nodes[meshIdx].scale[0], model.nodes[meshIdx].scale[1], model.nodes[meshIdx].scale[2]) :
					ONE_VEC3;
				modelMatrix = glm::translate(IDENTITY_MAT4, translation);
				modelMatrix = modelMatrix * glm::toMat4(rotation);
				modelMatrix = glm::scale(modelMatrix, scale);
			}

			const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
			const glm::mat3 tangentMatrix = glm::mat3(modelMatrix);

			if(relevantData & GltfAttributes::POSITIONS)
			{
				assert(relevantData & GltfAttributes::INDICES); // All gltf are indexed.

				auto& accessor = model.accessors[attributes["POSITION"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3);
				assert(model.meshes.size() == model.nodes.size()); // Note: this DOES NOT guarantee that there is a one-to-one matching between nodes and meshes, we just assume it does... Note: this really necessary now that I'm putting meshes into separate VAOs?

				std::vector<glm::vec3> positions;
				positions.insert
				(
					positions.end(),
					reinterpret_cast<glm::vec3*>(buffer.data.data() + bufferView.byteOffset),
					reinterpret_cast<glm::vec3*>(buffer.data.data() + bufferView.byteOffset + bufferView.byteLength)
				);

				for(size_t i = 0; i < positions.size(); i++)
				{
					positions[i] = (glm::vec3)(modelMatrix * glm::vec4(positions[i], 1.0f));
				}

				newVboDefs.push_back(VertexBuffer::Definition());
				auto& positionsBuffer = newVboDefs.front();
				positionsBuffer.begin = new uint8_t[bufferView.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
				positionsBuffer.byteLen = bufferView.byteLength;
				positionsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
				positionsBuffer.componentType = (ComponentType)TINYGLTF_COMPONENT_TYPE_FLOAT;
				positionsBuffer.isIndexBuffer = false;
				positionsBuffer.usage = (Mutability)GL_STATIC_DRAW;
				positionsBuffer.type = VertexBuffer::Definition::Type::POSITIONS_VEC3;
				memcpy(positionsBuffer.begin, positions.data(), bufferView.byteLength);
				assert(positionsBuffer.IsValid());
			}

			if(relevantData & GltfAttributes::INDICES)
			{
				assert(relevantData & GltfAttributes::POSITIONS); // No sense in loading indices without loading the positions.

				auto& accessor = model.accessors[indicesIdx];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT && accessor.type == TINYGLTF_TYPE_SCALAR);

				newEboDef.begin = new uint8_t[bufferView.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
				newEboDef.byteLen = bufferView.byteLength;
				newEboDef.componentsPerElement = 1;
				newEboDef.componentType = ComponentType::UINT;
				newEboDef.isIndexBuffer = true;
				newEboDef.usage = (Mutability)GL_STATIC_DRAW;
				newEboDef.type = VertexBuffer::Definition::Type::INDICES_UINT32;
				memcpy(newEboDef.begin, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
				assert(newEboDef.IsValid());
			}

			if((relevantData & GltfAttributes::NORMALS) || (relevantData & GltfAttributes::TANGENTS))
			{
				assert((relevantData & GltfAttributes::INDICES) || (relevantData & GltfAttributes::POSITIONS)); // All normals and tangents are indexed in gltf.

				auto& accessorNormals = model.accessors[attributes["NORMAL"]];
				auto& accessorTangents = model.accessors[attributes["TANGENT"]];
				auto& bufferViewNormals = model.bufferViews[accessorNormals.bufferView];
				auto& bufferViewTangents = model.bufferViews[accessorTangents.bufferView];
				auto& bufferNormals = model.buffers[bufferViewNormals.buffer];
				auto& bufferTangents = model.buffers[bufferViewTangents.buffer];

				assert(accessorNormals.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessorNormals.type == TINYGLTF_TYPE_VEC3);
				assert(accessorTangents.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessorTangents.type == TINYGLTF_TYPE_VEC4);

				std::vector<glm::vec3> normals;
				std::vector<float> tangentsVec4;
				normals.insert
				(
					normals.end(),
					reinterpret_cast<glm::vec3*>(bufferNormals.data.data() + bufferViewNormals.byteOffset),
					reinterpret_cast<glm::vec3*>(bufferNormals.data.data() + bufferViewNormals.byteOffset + bufferViewNormals.byteLength)
				);
				tangentsVec4.insert
				(
					tangentsVec4.end(),
					reinterpret_cast<float*>(bufferTangents.data.data() + bufferViewTangents.byteOffset),
					reinterpret_cast<float*>(bufferTangents.data.data() + bufferViewTangents.byteOffset + bufferViewTangents.byteLength)
				);
				assert(normals.size() * 4 == tangentsVec4.size());
				std::vector<glm::vec3> tangents = std::vector<glm::vec3>(normals.size());

				for(size_t i = 0; i < normals.size(); i++)
				{
					const glm::vec3 tangent = glm::vec3(tangentsVec4[i * 4 + 0], tangentsVec4[i * 4 + 1], tangentsVec4[i * 4 + 2]);
					assert(glm::length(tangent) > 0.99f);

					normals[i] = glm::normalize(normalMatrix * glm::normalize(normals[i]));
					tangents[i] = glm::normalize(tangentMatrix * glm::normalize(tangent));

					assert(glm::dot(normals[i], tangents[i]) < 0.0001f);
#ifdef _DEBUG
					const glm::vec3 bitangent = glm::cross(normals[i], tangents[i]);
					assert(glm::dot(normals[i], bitangent) < 0.0001f && glm::dot(tangents[i], bitangent) < 0.0001f);
#endif //!_DEBUG
				}

				if(relevantData & GltfAttributes::NORMALS)
				{
					newVboDefs.push_back(VertexBuffer::Definition());
					auto& normalsBuffer = newVboDefs.front();
					normalsBuffer.begin = new uint8_t[bufferViewNormals.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
					normalsBuffer.byteLen = bufferViewNormals.byteLength;
					normalsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
					normalsBuffer.componentType = (ComponentType)TINYGLTF_COMPONENT_TYPE_FLOAT;
					normalsBuffer.isIndexBuffer = false;
					normalsBuffer.usage = (Mutability)GL_STATIC_DRAW;
					normalsBuffer.type = VertexBuffer::Definition::Type::NORMALS;
					memcpy(normalsBuffer.begin, normals.data(), bufferViewNormals.byteLength);
					assert(normalsBuffer.IsValid());
				}
				if(relevantData & GltfAttributes::TANGENTS)
				{
					assert(relevantData & GltfAttributes::NORMALS); // Something really fishy goes on if the user asks for the tangents but NOT for the normals...

					newVboDefs.push_back(VertexBuffer::Definition());
					auto& tangentsBuffer = newVboDefs.front();
					tangentsBuffer.begin = new uint8_t[tangents.size() * sizeof(glm::vec3)]; // Note: Must be deleted in CreateVertexBuffer() method!!!
					tangentsBuffer.byteLen = tangents.size() * sizeof(glm::vec3);
					tangentsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
					tangentsBuffer.componentType = (ComponentType)TINYGLTF_COMPONENT_TYPE_FLOAT;
					tangentsBuffer.isIndexBuffer = false;
					tangentsBuffer.usage = (Mutability)GL_STATIC_DRAW;
					tangentsBuffer.type = VertexBuffer::Definition::Type::TANGENTS;
					memcpy(tangentsBuffer.begin, tangents.data(), tangents.size() * sizeof(glm::vec3));
					assert(tangentsBuffer.IsValid());
				}
			}

			if(relevantData & GltfAttributes::UVS)
			{
				assert((relevantData & GltfAttributes::INDICES) || (relevantData & GltfAttributes::POSITIONS)); // Uvs are indexed in gltf. Something wierd is going on if the user ONLY needs uvs.

				auto& accessor = model.accessors[attributes["TEXCOORD_0"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC2);

				newVboDefs.push_back(VertexBuffer::Definition());
				auto& uvsBuffer = newVboDefs.front();
				uvsBuffer.begin = new uint8_t[bufferView.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
				uvsBuffer.byteLen = bufferView.byteLength;
				uvsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
				uvsBuffer.componentType = (ComponentType)TINYGLTF_COMPONENT_TYPE_FLOAT;
				uvsBuffer.isIndexBuffer = false;
				uvsBuffer.usage = (Mutability)GL_STATIC_DRAW;
				uvsBuffer.type = VertexBuffer::Definition::Type::NORMALS;
				memcpy(uvsBuffer.begin, buffer.data.data(), bufferView.byteLength);
				assert(uvsBuffer.IsValid());
			}

			const uint32_t len = std::bit_width((uint64_t)ShadingMode::MAX_VALUE);
			for(uint32_t i = 0; i < len; i++)
			{
				// Load appropriate materials
				if(requiredShadingModes & ShadingMode::GIZMO)
				{

				}
				if(requiredShadingModes & ShadingMode::ALBEDO_ONLY)
				{

				}
				if(requiredShadingModes & ShadingMode::GOOCH)
				{

				}
				if(requiredShadingModes & ShadingMode::BLINN_PHONG)
				{

				}
				if(requiredShadingModes & ShadingMode::BLINN_PHONG_NORMALMAPPED)
				{

				}
				if(requiredShadingModes & ShadingMode::SHADOW_PASS)
				{

				}
				if(requiredShadingModes & ShadingMode::POST_PROCESS_PASS)
				{

				}
				if(requiredShadingModes & ShadingMode::DEFERRED_PASS)
				{

				}
			}
		}
	}

	Handle<KtxData> ResourceManager::LoadKtx(const std::string_view path)
	{
		std::string_view stem, extension, meshName, imageType; // Images should be named like so: <dirs to image>/<associated mesh's name>_<texture type>.<extension>
		{
			const size_t end = path.length();
			assert(path.find_last_of('\\') == path.length()); // Forbit usage of backslashes.
			const size_t lastDot = path.find_last_of('.');
			const size_t lastSlash = path.find_last_of('/');
			const size_t lastUnderscore = path.find_last_of('_');
			stem = path.substr(lastSlash + 1, lastDot - 1);
			extension = path.substr(lastDot, end);
			meshName = path.substr(lastSlash + 1, lastUnderscore - 1);
			imageType = path.substr(lastUnderscore + 1, lastDot - 1);
		}
		assert(extension == ".ktx"); // Make sure we're loading a .ktx
		assert(std::filesystem::exists(path));

		ktxs_.push_back(Resource<KtxData>());
		auto& newElement = ktxs_.front();
		auto& newKtx = newElement.resourceData;

		newKtx.data = gli::load(path.data());
		newKtx.associatedMesh = Hash(meshName.data(), meshName.length(), 0);
		if(imageType == "albedoMap")
		{
			newKtx.type = ImageType::ALBEDO_MAP;
		}
		else if(imageType == "specularMap")
		{
			newKtx.type = ImageType::SPECULAR_MAP;
		}
		else if(imageType == "normalMap")
		{
			newKtx.type = ImageType::NORMAL_MAP;
		}
		else
		{
			sge_ERROR("Unexpected texture type retireved from image's path!");
		}
		assert(newKtx.IsValid());

		newElement.hash = Hash(newKtx.data.data(), newKtx.data.size(), 0);
		assert(newElement.IsValid());

		Handle<KtxData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<ShaderData> ResourceManager::LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath, const std::string_view geometryPath)
	{
		assert(vertexPath.find_last_of('\\') == vertexPath.length() && fragmentPath.find_last_of('\\') == fragmentPath.length() && geometryPath.find_last_of('\\') == geometryPath.length());
		assert(vertexPath.substr(vertexPath.find_last_of('.'), vertexPath.length()) == ".vert");
		assert(fragmentPath.substr(fragmentPath.find_last_of('.'), fragmentPath.length()) == ".frag");
		assert(geometryPath.substr(geometryPath.find_last_of('.'), geometryPath.length()) == ".geo");

		shaderSrcs_.push_back(Resource<ShaderData>());
		auto& newElement = shaderSrcs_.front();
		auto& newShaderSrc = newElement.resourceData;

		newShaderSrc.vertexCode = LoadFile_(vertexPath);
		newShaderSrc.fragmentCode = LoadFile_(fragmentPath);
		newShaderSrc.geometryCode = LoadFile_(geometryPath);
		assert(newShaderSrc.IsValid());

		newElement.hash = Hash(vertexPath.data(), vertexPath.length(), 0);
		newElement.hash = Hash(fragmentPath.data(), fragmentPath.length(), newElement.hash);
		newElement.hash = Hash(geometryPath.data(), geometryPath.length(), newElement.hash);
		assert(newElement.IsValid());

		Handle<ShaderData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	TransformsBuffer ResourceManager::AllocateTransforms(const void* const data, const uint32_t byteLen)
	{
		TransformsBuffer buffer;
		buffer.begin = transformsPool_.Allocate(byteLen / sizeof(glm::mat4));
		buffer.end = buffer.begin + (byteLen / sizeof(glm::mat4));
		memcpy(buffer.begin, data, byteLen);
		assert(buffer.IsValid());
		return buffer;
	}
	
	void ResourceManager::Shutdown()
	{
		sge_MESSAGE("ResourceManager::Shutdown() called.");
		jsons_.clear();
		ktxs_.clear();
		shaderSrcs_.clear();
		gltfs_.clear();
	}

	std::string ResourceManager::LoadFile_(const std::string_view path)
	{
		assert(std::filesystem::exists(path));

		std::ifstream stream(path.data());
		return std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	}
}//!sge