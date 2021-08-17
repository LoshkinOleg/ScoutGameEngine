#include "ResourceManager.h"

#include <filesystem>

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include <glm/gtx/quaternion.hpp>
#include <glad/glad.h>

namespace sge
{
	bool GltfData::IsValid() const
	{
		bool returnVal = model != tinygltf::Model();
		for(const auto& image : images)
		{
			returnVal &= image->IsValid();
		}
		return returnVal;
	}
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
		jsons_.reserve(DEFAULT_JSON_POOL_SIZE_);
		ktxs_.reserve(DEFAULT_KTX_POOL_SIZE_);
		shaderSrcs_.reserve(DEFAULT_SHADER_SRC_POOL_SIZE_);
		gltfs_.reserve(DEFAULT_GLTF_POOL_SIZE_);
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
		if((jsons_.capacity() != DEFAULT_JSON_POOL_SIZE_) ||
		   (ktxs_.capacity() != DEFAULT_KTX_POOL_SIZE_) ||
		   (shaderSrcs_.capacity() != DEFAULT_SHADER_SRC_POOL_SIZE_) ||
		   (gltfs_.capacity() != DEFAULT_GLTF_POOL_SIZE_))
		{
			sge_ERROR("Trying to allocate resources post initialization! This is not yet handled!");
		}

		const std::string_view extension = path.substr(path.find_last_of('.'), path.size() - path.find_last_of('.'));
		assert(extension == ".json"); // Make sure we're loading a .json
		if (extension == ".gltf") { sge_ERROR("Please use LoadGltf to load composite gltf files instead."); };

		Resource<JsonData> newElement;
		auto& newJson = newElement.resourceData;

		const std::string str = LoadFile_(path);

		newElement.hash = Hash(str.c_str(), (uint32_t)str.length(), 0);
		newJson.data = json::parse(str);
		assert(newElement.hash.IsValid());
		assert(newJson.IsValid());
		assert(!ElementExists_<JsonData>(jsons_, newElement.hash));
		jsons_.push_back(newElement);

		Handle<JsonData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &jsons_.back();
		assert(handle.IsValid());
		return handle;
	}

	Handle<GltfData> ResourceManager::LoadGltf(const std::string_view path)
	{
		if((jsons_.capacity() != DEFAULT_JSON_POOL_SIZE_) ||
		   (ktxs_.capacity() != DEFAULT_KTX_POOL_SIZE_) ||
		   (shaderSrcs_.capacity() != DEFAULT_SHADER_SRC_POOL_SIZE_) ||
		   (gltfs_.capacity() != DEFAULT_GLTF_POOL_SIZE_))
		{
			sge_ERROR("Trying to allocate resources post initialization! This is not yet handled!");
		}

		assert(path.find_last_of('\\') == std::string::npos); // Forbid usage of backslashes in asset files.
		std::string gltfStem, gltfExtension;
		{
			const size_t posOfLastDot = path.find_last_of('.');
			const size_t posOfLastSlash = path.find_last_of('/');
			assert(posOfLastDot != -1 && posOfLastSlash != -1);
			gltfStem = path.substr(posOfLastSlash + 1, (posOfLastDot - 1) - posOfLastSlash);
			gltfExtension = path.substr(posOfLastDot, path.size() - posOfLastDot);
			assert(gltfExtension == ".glb" || gltfExtension == ".gltf"); // Make sure we're loading a gltf file.
		}
		assert(std::filesystem::exists(path));

		Resource<GltfData> newElement;
		auto& newGltf = newElement.resourceData;

		tinygltf::TinyGLTF loader;
		std::string error, warning;
		bool success = false;

		if (gltfExtension == ".glb") // Loading a binary only.
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

		const uint32_t nrOfImages = (uint32_t)newGltf.model.images.size();
		for(uint32_t i = 0; i < nrOfImages; i++)
		{
			const std::string_view imagePath = newGltf.model.images[i].uri;
			std::string_view imageExtension;
			{
				assert((imagePath.find_last_of('\\') == std::string::npos) &&
					   (imagePath.find_last_of('/') == std::string::npos)); // Forbid usage of directories. All images must be specified by name.

				const size_t posOfLastDot = imagePath.find_last_of('.');
				assert(posOfLastDot != std::string::npos);
				imageExtension = imagePath.substr(posOfLastDot, imagePath.size() - posOfLastDot);
			}
			assert(imagePath.find_last_of('.') != std::string::npos);
			if(imageExtension == ".ktx")
			{
				newGltf.images.push_back(LoadKtx(sge_KTXS_PATH + gltfStem.c_str() + "/" + imagePath.data()));
				assert(newGltf.images.back()->IsValid() && newGltf.images.back().IsValid());
			}
			else if(imageExtension == ".png")
			{
				sge_ERROR("Loading of pngs is not yet implemented!");
			}
			else if(imageExtension == ".bmp")
			{
				sge_ERROR("Loading of bmps is not yet implemented!");
			}
			else if(imageExtension == ".jpg" || imageExtension == ".jpeg")
			{
				sge_ERROR("Loading of jpgs is not yet implemented!");
			}
			else
			{
				sge_ERROR("Unexpected extension encountered during loading of image from gltf!");
			}
		}
		assert(newGltf.IsValid());
		newElement.hash = Hash(path.data(), (uint32_t)path.length(), 0);
		assert(newElement.IsValid());
		assert(!ElementExists_<GltfData>(gltfs_, newElement.hash));
		gltfs_.push_back(newElement);

		Handle<GltfData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &gltfs_.back();
		return handle;
	}

	Model::Definition ResourceManager::GenerateDefinitionFrom(const Handle<GltfData>& handle, const GltfData::GltfAttributes relevantData, const ShadingMode requiredShadingModes) const
	{
		assert((uint32_t)relevantData > 0); // Need at least something to load.

		Model::Definition modDef;
		modDef.transforms.push_back(IDENTITY_MAT4);
		tinygltf::Model& model = handle->model;

		const uint32_t nrOfMeshes = (uint32_t)model.meshes.size();
		for(uint32_t meshIdx = 0; meshIdx < nrOfMeshes; meshIdx++)
		{
			modDef.meshDefs.push_back(IndexedMesh::Definition());
			auto& newMeshDef = modDef.meshDefs.back();
			auto& newMaterialDefs = newMeshDef.matDefs;
			auto& newVboDefs = newMeshDef.vboDefs;
			auto& newEboDef = newMeshDef.eboDef;
			tinygltf::Mesh& mesh = model.meshes[meshIdx];

			assert(mesh.primitives.size() == 1 && mesh.primitives[0].attributes.size() == 4);

			auto& attributes = mesh.primitives[0].attributes;
			const auto indicesIdx = mesh.primitives[0].indices;
			const auto materialIdx = mesh.primitives[0].material;
			const std::string_view meshName = mesh.name;
			const Hash hashedMeshName = Hash(meshName.data(), (uint32_t)meshName.length(), 0);

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
					glm::vec3((float)model.nodes[meshIdx].translation[0], (float)model.nodes[meshIdx].translation[1], (float)model.nodes[meshIdx].translation[2]) :
					ZERO_VEC3;
				const glm::quat rotation = (model.nodes[meshIdx].rotation.size() > 0) ?
					glm::quat((float)model.nodes[meshIdx].rotation[0], (float)model.nodes[meshIdx].rotation[1], (float)model.nodes[meshIdx].rotation[2], (float)model.nodes[meshIdx].rotation[3]) :
					IDENTITY_QUAT;
				const glm::vec3 scale = (model.nodes[meshIdx].scale.size() > 0) ?
					glm::vec3((float)model.nodes[meshIdx].scale[0], (float)model.nodes[meshIdx].scale[1], (float)model.nodes[meshIdx].scale[2]) :
					ONE_VEC3;
				modelMatrix = glm::translate(IDENTITY_MAT4, translation);
				modelMatrix = modelMatrix * glm::toMat4(rotation);
				modelMatrix = glm::scale(modelMatrix, scale);
			}

			const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
			const glm::mat3 tangentMatrix = glm::mat3(modelMatrix);

			if((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::POSITIONS)
			{
				assert((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::INDICES); // All gltf are indexed.

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
				auto& positionsBuffer = newVboDefs.back();
				positionsBuffer.begin = new uint8_t[bufferView.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
				positionsBuffer.byteLen = (uint32_t)bufferView.byteLength;
				positionsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
				positionsBuffer.componentType = (NumberType)TINYGLTF_COMPONENT_TYPE_FLOAT;
				positionsBuffer.isIndexBuffer = false;
				positionsBuffer.mutability = (Mutability)GL_STATIC_DRAW;
				positionsBuffer.bufferContentsType = VertexBuffer::Type::POSITIONS_VEC3;
				positionsBuffer.preComputedHash = Hash(buffer.data.data() + bufferView.byteOffset, bufferView.byteLength, 0);
				memcpy(positionsBuffer.begin, positions.data(), bufferView.byteLength);
				assert(positionsBuffer.IsValid());
			}

			if((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::INDICES)
			{
				assert((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::POSITIONS); // No sense in loading indices without loading the positions.

				auto& accessor = model.accessors[indicesIdx];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.type == TINYGLTF_TYPE_SCALAR);
				const uint32_t gltfIndexNumberFormat = (uint32_t)accessor.componentType;
				switch(gltfIndexNumberFormat)
				{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					{
						newEboDef.begin = new uint8_t[bufferView.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
						newEboDef.byteLen = (uint32_t)bufferView.byteLength;
						newEboDef.componentsPerElement = 1;
						newEboDef.componentType = NumberType::UINT;
						newEboDef.isIndexBuffer = true;
						newEboDef.mutability = (Mutability)GL_STATIC_DRAW;
						newEboDef.bufferContentsType = VertexBuffer::Type::INDICES_UINT32;
						newEboDef.preComputedHash = Hash(buffer.data.data() + bufferView.byteOffset, bufferView.byteLength, 0);
						memcpy(newEboDef.begin, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
						assert(newEboDef.IsValid());
					}break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					{
						std::vector<uint16_t> inputIndices;
						inputIndices.insert(
							inputIndices.end(),
							reinterpret_cast<uint16_t*>(buffer.data.data() + bufferView.byteOffset),
							reinterpret_cast<uint16_t*>(buffer.data.data() + bufferView.byteOffset + bufferView.byteLength));
						std::vector<uint32_t> convertedIndices;
						const uint32_t nrOfIndices = bufferView.byteLength / sizeof(unsigned short);
						convertedIndices.resize((size_t)nrOfIndices);
						assert(inputIndices.size() == convertedIndices.size());
						for(uint32_t index = 0; index < nrOfIndices; index++)
						{
							convertedIndices[index] = (uint32_t)inputIndices[index];
						}

						newEboDef.begin = new uint8_t[sizeof(uint32_t) * (size_t)nrOfIndices]; // Note: Must be deleted in CreateVertexBuffer() method!!!
						newEboDef.byteLen = (uint32_t)(sizeof(uint32_t) * (size_t)nrOfIndices);
						newEboDef.componentsPerElement = 1;
						newEboDef.componentType = NumberType::UINT;
						newEboDef.isIndexBuffer = true;
						newEboDef.mutability = (Mutability)GL_STATIC_DRAW;
						newEboDef.bufferContentsType = VertexBuffer::Type::INDICES_UINT32;
						newEboDef.preComputedHash = Hash(inputIndices.data(), inputIndices.size() / sizeof(uint16_t), 0);
						memcpy(newEboDef.begin, convertedIndices.data(), (size_t)nrOfIndices * sizeof(uint32_t));
						assert(newEboDef.IsValid());
					}break;
					default:
					{
						sge_ERROR("Unexpected index number format retireved from gltf!");
					}break;
				}
			}

			if(((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::NORMALS) || ((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::TANGENTS))
			{
				assert(((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::INDICES) || ((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::POSITIONS)); // All normals and tangents are indexed in gltf.

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

				if((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::NORMALS)
				{
					newVboDefs.push_back(VertexBuffer::Definition());
					auto& normalsBuffer = newVboDefs.back();
					normalsBuffer.begin = new uint8_t[bufferViewNormals.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
					normalsBuffer.byteLen = (uint32_t)bufferViewNormals.byteLength;
					normalsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
					normalsBuffer.componentType = (NumberType)TINYGLTF_COMPONENT_TYPE_FLOAT;
					normalsBuffer.isIndexBuffer = false;
					normalsBuffer.mutability = (Mutability)GL_STATIC_DRAW;
					normalsBuffer.bufferContentsType = VertexBuffer::Type::NORMALS;
					normalsBuffer.preComputedHash = Hash(bufferNormals.data.data() + bufferViewNormals.byteOffset, bufferViewNormals.byteLength, 0);
					memcpy(normalsBuffer.begin, normals.data(), bufferViewNormals.byteLength);
					assert(normalsBuffer.IsValid());
				}
				if((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::TANGENTS)
				{
					assert((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::NORMALS); // Something really fishy goes on if the user asks for the tangents but NOT for the normals...

					newVboDefs.push_back(VertexBuffer::Definition());
					auto& tangentsBuffer = newVboDefs.back();
					tangentsBuffer.begin = new uint8_t[tangents.size() * sizeof(glm::vec3)]; // Note: Must be deleted in CreateVertexBuffer() method!!!
					tangentsBuffer.byteLen = (uint32_t)(tangents.size() * sizeof(glm::vec3));
					tangentsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
					tangentsBuffer.componentType = (NumberType)TINYGLTF_COMPONENT_TYPE_FLOAT;
					tangentsBuffer.isIndexBuffer = false;
					tangentsBuffer.mutability = (Mutability)GL_STATIC_DRAW;
					tangentsBuffer.bufferContentsType = VertexBuffer::Type::TANGENTS;
					tangentsBuffer.preComputedHash = Hash(tangents.data(), tangents.size() / sizeof(glm::vec3), 0);
					memcpy(tangentsBuffer.begin, tangents.data(), tangents.size() * sizeof(glm::vec3));
					assert(tangentsBuffer.IsValid());
				}
			}

			if((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::UVS)
			{
				assert(((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::INDICES) || ((uint32_t)relevantData & (uint32_t)GltfData::GltfAttributes::POSITIONS)); // Uvs are indexed in gltf. Something wierd is going on if the user ONLY needs uvs.

				auto& accessor = model.accessors[attributes["TEXCOORD_0"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC2);

				newVboDefs.push_back(VertexBuffer::Definition());
				auto& uvsBuffer = newVboDefs.back();
				uvsBuffer.begin = new uint8_t[bufferView.byteLength]; // Note: Must be deleted in CreateVertexBuffer() method!!!
				uvsBuffer.byteLen = (uint32_t)bufferView.byteLength;
				uvsBuffer.componentsPerElement = TINYGLTF_TYPE_VEC3;
				uvsBuffer.componentType = (NumberType)TINYGLTF_COMPONENT_TYPE_FLOAT;
				uvsBuffer.isIndexBuffer = false;
				uvsBuffer.mutability = (Mutability)GL_STATIC_DRAW;
				uvsBuffer.bufferContentsType = VertexBuffer::Type::UVS;
				uvsBuffer.preComputedHash = Hash(buffer.data.data() + bufferView.byteOffset, bufferView.byteLength, 0);
				memcpy(uvsBuffer.begin, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
				assert(uvsBuffer.IsValid());
			}

			if(requiredShadingModes != ShadingMode::INVALID)
			{
				if((uint32_t)requiredShadingModes & (uint32_t)ShadingMode::GIZMO)
				{
					newMaterialDefs.push_back(Material::Definition());
					auto& newMaterialDef = newMaterialDefs.back();
					newMaterialDef.vec3s.push_back(INVERSE_DEFAULT_COLOR);
				}
				if((uint32_t)requiredShadingModes & (uint32_t)ShadingMode::ALBEDO_ONLY)
				{
					newMaterialDefs.push_back(Material::Definition());
					auto& newMaterialDef = newMaterialDefs.back();
					newMaterialDef.shadingMode = ShadingMode::ALBEDO_ONLY;
					newMaterialDef.texDefs.push_back(Texture::Definition());
					auto& newTextureDef = newMaterialDef.texDefs.back();
					bool albedoMapFound = false;
					for(const auto& image : handle->images)
					{
						if(image->associatedMesh == hashedMeshName)
						{
							if(image->type == ImageType::ALBEDO_MAP)
							{
								albedoMapFound = true;
								newTextureDef = GenerateDefinitionFrom
										(image,
										(Texture::SamplingMode)((image->data.levels() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR),
										(Texture::SamplingMode)GL_LINEAR,
										(Texture::WrappingMode)GL_MIRRORED_REPEAT,
										(Texture::WrappingMode)GL_MIRRORED_REPEAT,
										(Mutability)GL_STATIC_DRAW,
										false);
								break;
							}
						}
					}
					if(!albedoMapFound)
					{
						sge_ERROR("Couldn't find an appropriate texture in the list of images provided!");
					}
					assert(newMaterialDef.IsValid());
				}
				if((uint32_t)requiredShadingModes & (uint32_t)ShadingMode::GOOCH)
				{
					newMaterialDefs.push_back(Material::Definition());
					auto& newMaterialDef = newMaterialDefs.back();
					newMaterialDef.vec3s.push_back(INVERSE_DEFAULT_COLOR);
					newMaterialDef.shadingMode = ShadingMode::GOOCH;
				}
				if((uint32_t)requiredShadingModes & (uint32_t)ShadingMode::BLINN_PHONG_NORMALMAPPED)
				{
					newMaterialDefs.push_back(Material::Definition());
					auto& newMaterialDef = newMaterialDefs.back();
					newMaterialDef.shadingMode = ShadingMode::BLINN_PHONG_NORMALMAPPED;
					Texture::Definition newAlbedoDef = {};
					Texture::Definition newSpecularDef = {};
					Texture::Definition newNormalmapDef = {};
					bool albedoMapFound = false;
					bool specularMapFound = false;
					bool normalMapFound = false;
					for(const auto& image : handle->images)
					{
						if(image->associatedMesh == hashedMeshName)
						{
							if(image->type == ImageType::ALBEDO_MAP)
							{
								albedoMapFound = true;
								newAlbedoDef = GenerateDefinitionFrom
								(image,
									(Texture::SamplingMode)((image->data.levels() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR),
									(Texture::SamplingMode)GL_LINEAR,
									(Texture::WrappingMode)GL_MIRRORED_REPEAT,
									(Texture::WrappingMode)GL_MIRRORED_REPEAT,
									(Mutability)GL_STATIC_DRAW,
									false);
								newMaterialDef.texDefs.push_back(newAlbedoDef);
							}
							else if(image->type == ImageType::SPECULAR_MAP)
							{
								// TODO: use single channel texture for speculars.
								specularMapFound = true;
								newSpecularDef = GenerateDefinitionFrom
								(image,
									(Texture::SamplingMode)((image->data.levels() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR),
									(Texture::SamplingMode)GL_LINEAR,
									(Texture::WrappingMode)GL_MIRRORED_REPEAT,
									(Texture::WrappingMode)GL_MIRRORED_REPEAT,
									(Mutability)GL_STATIC_DRAW,
									false);
								newMaterialDef.texDefs.push_back(newSpecularDef);
							}
							else if(image->type == ImageType::NORMAL_MAP)
							{
								normalMapFound = true;
								newNormalmapDef = GenerateDefinitionFrom
								(image,
									(Texture::SamplingMode)((image->data.levels() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR),
									(Texture::SamplingMode)GL_LINEAR,
									(Texture::WrappingMode)GL_MIRRORED_REPEAT,
									(Texture::WrappingMode)GL_MIRRORED_REPEAT,
									(Mutability)GL_STATIC_DRAW,
									false);
								newMaterialDef.texDefs.push_back(newNormalmapDef);
							}
						}
					}
					if(!albedoMapFound || !specularMapFound || !normalMapFound)
					{
						sge_ERROR("Couldn't find an appropriate texture in the list of images provided!");
					}
					newMaterialDef.floats.push_back(64.0f); // TODO: load shininess from gltf.
					assert(newMaterialDef.IsValid());
				}
			}
			assert(newMeshDef.IsValid());
		}
		return modDef;
	}

	Texture::Definition ResourceManager::GenerateDefinitionFrom
		(const Handle<KtxData>& handle,
		 const Texture::SamplingMode minifyingMode,
		 const Texture::SamplingMode magnifyingMode,
		 const Texture::WrappingMode onS,
		 const Texture::WrappingMode onT,
		 const Mutability mutability,
		 const bool generateMipMaps) const
	{
		assert(handle->IsValid());
		if(generateMipMaps)
		{
			sge_ERROR("Implement this before using it!");
		}

		const auto& image = handle->data;
		const uint32_t nrOfMipLevels = (uint32_t)image.levels();
		Texture::Definition newTextureDef;

		newTextureDef.datas.resize(nrOfMipLevels);
		for(uint32_t level = 0; level < nrOfMipLevels; level++)
		{
			// TODO: handle cubemaps
			newTextureDef.datas[level] = new uint8_t[image.size(level)];
			memcpy(newTextureDef.datas[level], image.data(0, 0, level), image.size(level));
			newTextureDef.byteLens.push_back(image.size(level));
			newTextureDef.widths.push_back(image.extent(level).x);
			newTextureDef.heights.push_back(image.extent(level).y);
		}
		newTextureDef.format = Texture::Format::RGBA_B8; // TODO: add support for more formats.
		newTextureDef.generateMipMaps = false;
		newTextureDef.minifyingMode = minifyingMode;
		newTextureDef.magnifyingMode = magnifyingMode;
		newTextureDef.mipLevels = nrOfMipLevels - 1; // Note: using 0 for a single mipmap level for simpler conditionals.
		newTextureDef.mutability = mutability;
		newTextureDef.onS = onS;
		newTextureDef.onT = onT;
		newTextureDef.preComputedHash = Hash(image.data(0,0,0), image.size(0), 0);
		switch(image.format())
		{
			case gli::texture::format_type::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16:
			{
				newTextureDef.compression = Texture::Compression::ASTC_RGBA_4x4;
			}break;
			case gli::texture::format_type::FORMAT_RGBA_ETC2_UNORM_BLOCK16:
			{
				newTextureDef.compression = Texture::Compression::ETC2;
			}break;
			case gli::texture::format_type::FORMAT_RGB_ETC_UNORM_BLOCK8:
			{
				newTextureDef.compression = Texture::Compression::ETC1;
			}break;
			default:
			{
				sge_ERROR("Unexpected format retrieved from gli::texture!");
			}break;
		}
		assert(newTextureDef.IsValid());
		return newTextureDef;
	}

	Handle<KtxData> ResourceManager::LoadKtx(const std::string_view path)
	{
		if((jsons_.capacity() != DEFAULT_JSON_POOL_SIZE_) ||
		   (ktxs_.capacity() != DEFAULT_KTX_POOL_SIZE_) ||
		   (shaderSrcs_.capacity() != DEFAULT_SHADER_SRC_POOL_SIZE_) ||
		   (gltfs_.capacity() != DEFAULT_GLTF_POOL_SIZE_))
		{
			sge_ERROR("Trying to allocate resources post initialization! This is not yet handled!");
		}

		std::string stem, extension, meshName, imageType; // Images should be named like so: <dirs to image>/<associated mesh's name>_<texture type>.<extension>
		{
			const size_t end = path.length();
			assert(path.find_last_of('\\') == std::string::npos); // Forbit usage of backslashes.
			const size_t lastDot = path.find_last_of('.');
			const size_t lastSlash = path.find_last_of('/');
			const size_t lastUnderscore = path.find_last_of('_');
			stem = path.substr(lastSlash + 1, (lastDot - 1) - lastSlash);
			extension = path.substr(lastDot, end - lastDot);
			meshName = path.substr(lastSlash + 1, lastUnderscore - (lastSlash + 1));
			imageType = path.substr(lastUnderscore + 1, lastDot - (lastUnderscore + 1));
		}
		assert(extension == ".ktx"); // Make sure we're loading a .ktx
		assert(std::filesystem::exists(path));


		Resource<KtxData> newElement;
		auto& newKtx = newElement.resourceData;

		newKtx.data = gli::load(path.data());
		newElement.hash = Hash(newKtx.data.data(), (uint32_t)newKtx.data.size(), 0);
		newElement.hash.Accumulate(imageType.c_str(), imageType.length());
		Hash associatedMesh = Hash(meshName.data(), (uint32_t)meshName.length(), 0);
		newKtx.associatedMesh = associatedMesh;
		newElement.hash.Accumulate(associatedMesh);

		if(!ElementExists_<KtxData>(ktxs_, newElement.hash))
		{
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

			assert(newElement.IsValid());
			ktxs_.push_back(newElement);

			Handle<KtxData> handle;
			handle.hash = newElement.hash;
			handle.ptr = &ktxs_.back();
			assert(handle.IsValid());
			return handle;
		}
		else
		{
			sge_WARNING("Element already in a resource manager's list! Returning handle to existing element.");
			auto& existingElement = GetElement_<KtxData>(ktxs_, newElement.hash);
			Handle<KtxData> handle;
			handle.hash = existingElement.hash;
			handle.ptr = &existingElement;
			assert(handle.IsValid());
			return handle;
		}
	}

	Handle<ShaderData> ResourceManager::LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath, const std::string_view geometryPath)
	{
		if((jsons_.capacity() != DEFAULT_JSON_POOL_SIZE_) ||
		   (ktxs_.capacity() != DEFAULT_KTX_POOL_SIZE_) ||
		   (shaderSrcs_.capacity() != DEFAULT_SHADER_SRC_POOL_SIZE_) ||
		   (gltfs_.capacity() != DEFAULT_GLTF_POOL_SIZE_))
		{
			sge_ERROR("Trying to allocate resources post initialization! This is not yet handled!");
		}

		assert(vertexPath.find_last_of('\\') == std::string::npos && fragmentPath.find_last_of('\\') == std::string::npos && geometryPath.find_last_of('\\') == std::string::npos);
		assert(vertexPath.substr(vertexPath.find_last_of('.'), vertexPath.size() - vertexPath.find_last_of('.')) == ".vert");
		assert(fragmentPath.substr(fragmentPath.find_last_of('.'), fragmentPath.size() - fragmentPath.find_last_of('.')) == ".frag");
		const bool usingGeometryShader = geometryPath.length() > 0;
		if(usingGeometryShader)
		{
			assert(geometryPath.substr(geometryPath.find_last_of('.'), geometryPath.size() - geometryPath.find_last_of('.')) == ".geo");
		}

		Hash hash = 0;
		hash.Accumulate(vertexPath.data(), (uint32_t)vertexPath.length());
		hash.Accumulate(fragmentPath.data(), (uint32_t)fragmentPath.length());
		if(usingGeometryShader)
		{
			hash.Accumulate(geometryPath.data(), (uint32_t)geometryPath.length());
		}
		assert(!ElementExists_<ShaderData>(shaderSrcs_, hash));

		shaderSrcs_.push_back(Resource<ShaderData>());
		auto& newElement = shaderSrcs_.back();
		auto& newShaderSrc = newElement.resourceData;

		newShaderSrc.vertexCode = LoadFile_(vertexPath);
		newShaderSrc.fragmentCode = LoadFile_(fragmentPath);
		if(usingGeometryShader)
		{
			newShaderSrc.geometryCode = LoadFile_(geometryPath);
		}
		assert(newShaderSrc.IsValid());

		newElement.hash = hash;
		assert(newElement.IsValid());

		Handle<ShaderData> handle;
		handle.hash = newElement.hash;
		handle.ptr = &shaderSrcs_.back();
		assert(handle.IsValid());
		return handle;
	}

	glm::mat4* ResourceManager::AllocateTransforms(const void* const data, const uint32_t byteLen)
	{
		return transformsPool_.Allocate(byteLen / sizeof(glm::mat4));
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