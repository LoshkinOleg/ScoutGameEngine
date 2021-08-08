#include "Renderer.h"

#include <iostream>

#include <xxhash.h>
#include <glm/gtx/quaternion.hpp>

namespace sge
{
	std::vector<Renderer::MeshData_> Renderer::ProcessGltf_(const GltfDataHandle& handle)
	{
		std::vector<Renderer::MeshData_> returnVal;
		tinygltf::Model& model = handle->model;

		// uint32_t accumulatedVertexCount = 0;
		// uint32_t currentMeshVertexCount = 0;

		for (uint32_t meshIdx = 0; meshIdx < model.meshes.size(); meshIdx++)
		{
			tinygltf::Mesh& mesh = model.meshes[meshIdx];
			returnVal.push_back(Renderer::MeshData_());
			Renderer::MeshData_& meshData = returnVal.front();

			assert(mesh.primitives.size() == 1 && mesh.primitives[0].attributes.size() == 4);

			auto& attributes = mesh.primitives[0].attributes;
			const auto indicesIdx = mesh.primitives[0].indices;
			auto& materialIdx = mesh.primitives[0].material;

			for (const auto& pair : attributes)
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
			const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

			if (modelMatrix == IDENTITY_MAT4)
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
				modelMatrix *= glm::toMat4(rotation);
				modelMatrix = glm::scale(modelMatrix, scale);
			}

			{ // Positions.
				auto& accessor = model.accessors[attributes["POSITION"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3);
				assert(model.meshes.size() == model.nodes.size()); // Note: this DOES NOT guarantee that there is a one-to-one matching between nodes and meshes, we just assume it does... Note: this really necessary now that I'm putting meshes into separate VAOs?

				// accumulatedVertexCount += accessor.count;
				// currentMeshVertexCount = accessor.count;
				std::vector<glm::vec3> positions;
				positions.insert
				(
					positions.end(),
					reinterpret_cast<glm::vec3*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec3)),
					reinterpret_cast<glm::vec3*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec3)) + (bufferView.byteLength / sizeof(glm::vec3))
				);

				for (size_t i = 0; i < positions.size(); i++)
				{
					positions[i] = modelMatrix * glm::vec4(positions[i], 1.0f);
				}

				meshData.positions.insert
				(
					meshData.positions.end(),
					positions.begin(),
					positions.end()
				);
			}

			{ // Indices.
				auto& accessor = model.accessors[indicesIdx];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT && accessor.type == TINYGLTF_TYPE_SCALAR);
				meshData.indexType = Renderer::MeshData_::IndexType::UNSIGNED_SHORT;
				size_t currentMeshIndices = meshData.indices.end() - meshData.indices.begin();
				meshData.indices.insert
				(
					meshData.indices.end(),
					reinterpret_cast<uint16_t*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(uint16_t)),
					reinterpret_cast<uint16_t*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(uint16_t)) + (bufferView.byteLength / sizeof(uint16_t))
				);
				// for (; currentMeshIndices < meshData.indices.size(); currentMeshIndices++)
				// {
				// 	meshData.indices[currentMeshIndices] += (accumulatedVertexCount - currentMeshVertexCount);
				// }
			}

			const size_t thisMeshesNormalsStart = meshData.normals.end() - meshData.normals.begin();
			{ // Normals.
				auto& accessor = model.accessors[attributes["NORMAL"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3);

				std::vector<glm::vec3> normals;
				normals.insert
				(
					normals.end(),
					reinterpret_cast<glm::vec3*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec3)),
					reinterpret_cast<glm::vec3*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec3)) + (bufferView.byteLength / sizeof(glm::vec3))
				);

				for (size_t i = 0; i < normals.size(); i++)
				{
					normals[i] = glm::normalize(normalMatrix * glm::normalize(normals[i]));
				}

				meshData.normals.insert
				(
					meshData.normals.end(),
					normals.begin(),
					normals.end()
				);
			}

			{ // Tangents and bitangents.
				auto& accessor = model.accessors[attributes["TANGENT"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC4);
				std::vector<glm::vec4> tangentsVec4;
				std::vector<glm::vec3> normals = std::vector<glm::vec3>(thisMeshesNormalsStart == 0 ?
					meshData.normals.begin() :
					meshData.normals.begin() + thisMeshesNormalsStart, meshData.normals.end());
				tangentsVec4.insert
				(
					tangentsVec4.end(),
					reinterpret_cast<glm::vec4*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec4)),
					reinterpret_cast<glm::vec4*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec4)) + (bufferView.byteLength / sizeof(glm::vec4))
				);
				assert(normals.size() == tangentsVec4.size());

				std::vector<glm::vec3> tangents = std::vector<glm::vec3>(tangentsVec4.size(), glm::vec3(0.0f));
				std::vector<glm::vec3> bitangents = std::vector<glm::vec3>(tangentsVec4.size(), glm::vec3(0.0f));
				for (size_t i = 0; i < tangentsVec4.size(); i++)
				{
					tangents[i] = glm::normalize(normalMatrix * glm::normalize(glm::vec3(tangentsVec4[i])));
					bitangents[i] = glm::normalize(normalMatrix * glm::normalize(glm::cross(normals[i], glm::vec3(tangentsVec4[i])) * tangentsVec4[i].w));
					assert
					(
						glm::dot(normals[i], tangents[i]) == 0.0f &&
						glm::dot(normals[i], bitangents[i]) == 0.0f &&
						glm::dot(tangents[i], bitangents[i]) == 0.0f
					);
				}

				meshData.tangents.insert
				(
					meshData.tangents.end(),
					tangents.begin(),
					tangents.end()
				);
				meshData.bitangents.insert
				(
					meshData.bitangents.end(),
					bitangents.begin(),
					bitangents.end()
				);
			}

			{ // Uvs.
				auto& accessor = model.accessors[attributes["TEXCOORD_0"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC2);
				meshData.uvs.insert
				(
					meshData.uvs.end(),
					reinterpret_cast<glm::vec2*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec2)),
					reinterpret_cast<glm::vec2*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec2)) + (bufferView.byteLength / sizeof(glm::vec2))
				);
			}

			// TODO: material
		}

		return returnVal;
	}

	std::vector<glm::mat4> Renderer::FrustumCulling_(const glm::ivec2 resolution, const float horizontalFullFov, const float nearPlane, const float farPlane, const float radius, const glm::mat4* const begin, const glm::mat4* const end)
	{
		return std::vector<glm::mat4>(begin, end);
	}

	void Renderer::SortFrontToBack_(std::vector<glm::mat4>& transforms)
	{
		return;
	}

	void Renderer::SortBackToFront_(std::vector<glm::mat4>& transforms)
	{
		return;
	}

	void Renderer::Init()
	{
		glClearColor(CLEAR_COLOR_[0], CLEAR_COLOR_[1], CLEAR_COLOR_[2], CLEAR_COLOR_[3]);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	void Renderer::Shutdown()
	{
		// Note: containers call the Destroy() function on their objects before clearing the map.
		shaders_.Clear();
		vertexBuffers_.Clear();
		textures_.Clear();
		meshes_.Clear();
		models_.Clear();
		drawQueueOpaques_.clear();
		drawQueueTransparents_.clear();
	}
	void Renderer::Update()
	{
		// TODO: update view matrix using player input.

		glClear(CLEAR_FLAGS_);
		
		// TODO: make a pass on all shaders beforehand to set the common uniforms instead of bothering the gpu for every model.

		const size_t queueLen = drawQueueOpaques_.size();
		for (size_t modelIdx = 0; modelIdx < queueLen; modelIdx++)
		{
			Model& model = *drawQueueOpaques_[modelIdx].model;
			Shader& shader = *drawQueueOpaques_[modelIdx].shader;
			const int32_t primitive = drawQueueOpaques_[modelIdx].primitive;

			std::vector<glm::mat4> transforms = FrustumCulling_(RESOLUTION, FULL_FOV[0], WINDOW_PROJECTION_NEAR, WINDOW_PROJECTION_FAR, model.radius, model.transformsBegin, model.transformsEnd);
			if (transforms.size() < 1) continue;

			assert(shader.PROGRAM > 0);
			glUseProgram(shader.PROGRAM);
			shader.SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix_);
			shader.SetVec3("viewPos", glm::vec3(viewMatrix_[3]));
			sge_CHECK_GL_ERROR();

			const size_t meshLen = model.meshes.size();
			for (size_t meshIdx = 0; meshIdx < meshLen; meshIdx++)
			{
				Mesh& mesh = *model.meshes[meshIdx];
				shader.SetFloat("shininess", mesh.shininess);

				assert(mesh.VAO > 0);
				glBindVertexArray(mesh.VAO);
				sge_CHECK_GL_ERROR();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh.alphaMap->TEX);
				sge_CHECK_GL_ERROR();

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mesh.albedoMap->TEX);
				sge_CHECK_GL_ERROR();

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, mesh.specularMap->TEX);
				sge_CHECK_GL_ERROR();

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, mesh.normalMap->TEX);
				sge_CHECK_GL_ERROR();

				glBindBuffer(GL_ARRAY_BUFFER, model.transformsVertexBuffer->VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, transforms.size() * sizeof(glm::mat4), transforms.data());
				sge_CHECK_GL_ERROR();
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(7);
				glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(8);
				glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
				sge_CHECK_GL_ERROR();
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);
				glVertexAttribDivisor(7, 1);
				glVertexAttribDivisor(8, 1);
				sge_CHECK_GL_ERROR();

				glDrawElementsInstanced(primitive, mesh.nrOfVertices, mesh.indexType, 0, transforms.size());
			}
		}

		// TODO: draw transparent objects.

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		drawQueueOpaques_.clear();
		drawQueueTransparents_.clear();
	}

	Shader& Renderer::GetShader(const ShaderHandle& handle)
	{
		shaders_.Access(handle);
	}

	Texture& Renderer::GetTexture(const TextureHandle& handle)
	{
		textures_.Access(handle);
	}

	VertexBuffer& Renderer::GetVertexBuffer(const VertexBufferHandle& handle)
	{
		vertexBuffers_.Access(handle);
	}

	Mesh& Renderer::GetMesh(const MeshHandle& handle)
	{
		meshes_.Access(handle);
	}

	Model& Renderer::GetModel(const ModelHandle& handle)
	{
		models_.Access(handle);
	}

	ShaderHandle Renderer::CreateShader(const ShaderDataHandle& handle)
	{
		uint32_t VERT = 0, FRAG = 0;
		int32_t success = false;
#ifdef _DEBUG
		int8_t errorMsg[1024];
#endif
		const char* vertSrc = handle->vertexCode.data();
		const char* fragSrc = handle->fragmentCode.data();
		assert(
			(handle->vertexCode.data() != "" && handle->vertexCode.data() != "\0") &&
			(handle->fragmentCode.data() != "" && handle->fragmentCode.data() != "\0"));

		VERT = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(VERT, 1, &vertSrc, NULL);
		sge_CHECK_GL_ERROR();
		glCompileShader(VERT);
		sge_CHECK_GL_ERROR();
		glGetShaderiv(VERT, GL_COMPILE_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if (!success) { sge_ERROR(errorMsg); }

		FRAG = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(FRAG, 1, &fragSrc, NULL);
		sge_CHECK_GL_ERROR();
		glCompileShader(FRAG);
		sge_CHECK_GL_ERROR();
		glGetShaderiv(FRAG, GL_COMPILE_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if (!success) { sge_ERROR(errorMsg); }

		uint32_t PROGRAM = glCreateProgram();
		glAttachShader(PROGRAM, VERT);
		glAttachShader(PROGRAM, FRAG);
		sge_CHECK_GL_ERROR();
		glLinkProgram(PROGRAM);
		sge_CHECK_GL_ERROR();
		glGetProgramiv(PROGRAM, GL_LINK_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if (!success) { sge_ERROR(errorMsg); }

		glDeleteShader(VERT);
		glDeleteShader(FRAG);
		sge_CHECK_GL_ERROR();

		Shader newElement;
		newElement.PROGRAM = PROGRAM;
		newElement.hash = handle.hash;
		assert(newElement.hash > 0);
		shaders_.Insert(newElement);

		ShaderHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	TextureHandle Renderer::CreateTexture(const KtxDataHandle& handle)
	{
		uint32_t TEX = 0;

		gli::gl translator(gli::gl::PROFILE_KTX);
		const gli::gl::format format = translator.translate(handle->data.format(), handle->data.swizzles());
		GLenum target = translator.translate(handle->data.target());
		assert(target == GL_TEXTURE_2D);

		const GLint levels = static_cast<GLint>(handle->data.levels());
		const GLint layers = static_cast<GLint>(handle->data.layers());
		const GLint faces = static_cast<GLint>(handle->data.faces());

		glGenTextures(1, &TEX);
		glBindTexture(target, TEX);
		glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(levels - 1));
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);
		sge_CHECK_GL_ERROR();

		glm::tvec3<GLsizei> extents(handle->data.extent());

		glTexStorage2D
		(
			target,
			levels,
			format.Internal,
			extents.x,
			extents.y
		);
		sge_CHECK_GL_ERROR();

		assert(gli::is_compressed(handle->data.format()));

		for (size_t layer = 0; layer < layers; layer++)
		{
			for (size_t face = 0; face < faces; face++)
			{
				for (size_t level = 0; level < levels; level++)
				{
					extents = glm::tvec3<GLsizei>(handle->data.extent(level));
					glCompressedTexSubImage2D
					(
						target,
						static_cast<GLint>(level),
						0,
						0,
						extents.x,
						extents.y,
						format.Internal,
						static_cast<GLsizei>(handle->data.size(level)),
						handle->data.data(layer, face, level)
					);
					sge_CHECK_GL_ERROR();
				}
			}
		}

		Texture newElement;
		newElement.TEX = TEX;
		newElement.hash = handle.hash;
		assert(newElement.hash > 0);
		textures_.Insert(newElement);

		TextureHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	VertexBufferHandle Renderer::CreateVertexBuffer(const std::vector<float>& data, const int32_t usage)
	{
		uint32_t VBO = 0;

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		sge_CHECK_GL_ERROR();
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), usage);
		sge_CHECK_GL_ERROR();
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		VertexBuffer newElement;
		newElement.VBO = VBO;
		newElement.hash = XXH32(data.data(), data.size() * sizeof(float), HASHING_SEED);
		assert(newElement.hash > 0);
		vertexBuffers_.Insert(newElement);

		VertexBufferHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	MeshHandle Renderer::CreateMesh(const MeshData_& data)
	{
		uint32_t VAO = 0, EBO = 0;
		VertexBufferHandle positions;
		VertexBufferHandle normals;
		VertexBufferHandle tangents;
		VertexBufferHandle bitangents;
		VertexBufferHandle uvs;
		const float shininess = 64.0f; // TODO: actually read that from file.
		uint32_t nrOfVertices = 0;
		int32_t indexType = 0;
		const bool isTransparent = false; // TODO: analyse alphaMap to set this.

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * data.indexType, data.indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);

		const std::vector<float> positionsData = std::vector<float>(data.positions.begin(), data.positions.end());
		positions = CreateVertexBuffer(positionsData, GL_STATIC_DRAW);

		const std::vector<float> normalsData = std::vector<float>(data.normals.begin(), data.normals.end());
		normals = CreateVertexBuffer(normalsData, GL_STATIC_DRAW);

		const std::vector<float> tangentsData = std::vector<float>(data.tangents.begin(), data.tangents.end());
		tangents = CreateVertexBuffer(tangentsData, GL_STATIC_DRAW);

		const std::vector<float> bitangentsData = std::vector<float>(data.bitangents.begin(), data.bitangents.end());
		bitangents = CreateVertexBuffer(bitangentsData, GL_STATIC_DRAW);

		const std::vector<float> uvsData = std::vector<float>(data.uvs.begin(), data.uvs.end());
		uvs = CreateVertexBuffer(uvsData, GL_STATIC_DRAW);

		nrOfVertices = data.indices.size();
		indexType = GL_UNSIGNED_SHORT;

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, positions->VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, normals->VBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, tangents->VBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, bitangents->VBO);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, uvs->VBO);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		Mesh newElement;
		newElement.positions = positions;
		assert(newElement.positions->hash > 0);
		newElement.normals = normals;
		assert(newElement.normals->hash > 0);
		newElement.tangents = tangents;
		assert(newElement.tangents->hash > 0);
		newElement.bitangents = bitangents;
		assert(newElement.bitangents->hash > 0);
		newElement.EBO = EBO;
		newElement.VAO = VAO;
		newElement.indexType = indexType;
		assert(newElement.indexType == GL_UNSIGNED_SHORT);
		newElement.isTransparent = isTransparent;
		newElement.nrOfVertices = nrOfVertices;
		assert(newElement.nrOfVertices > 0);
		newElement.shininess = shininess;

		// TODO: add textures too.

		std::string accumulatedData = "";
		accumulatedData += std::to_string(positions->hash);
		accumulatedData += std::to_string(normals->hash);
		accumulatedData += std::to_string(tangents->hash);
		accumulatedData += std::to_string(bitangents->hash);
		accumulatedData += std::to_string(uvs->hash);
		newElement.hash = XXH32(accumulatedData.data(), accumulatedData.size(), HASHING_SEED);
		assert(newElement.hash > 0);

		meshes_.Insert(newElement);

		MeshHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	ModelHandle Renderer::CreateModel(const GltfDataHandle& handle)
	{
		Model newElement;
		newElement.
		ModelHandle returnVal;
	}
	void Renderer::Schedule(const ModelHandle& model, const ShaderHandle& shader, const int32_t primitive)
	{}

	void Renderer::ScheduleToBeDrawn(VertexBufferHandle bufferHandle, ShaderHandle shaderHandle, const std::vector<Texture2dHandle>& textures, const uint32_t nrOfInstances, const int32_t primitive)
	{
		assert(bufferHandle.bufferDataHash > 0);
		VertexBuffer& buffer = vertexBuffers_[bufferHandle.vertexBufferIndex];
		assert(bufferHandle.bufferDataHash == buffer.bufferDataHash);

		assert(shaderHandle.shaderSrcHash > 0);
		Shader& shader = shaders_[shaderHandle.shaderIndex];
		assert(shaderHandle.shaderSrcHash == shader.shaderSrcHash);

		std::vector<Texture2d> texs = std::vector<Texture2d>(textures.size());
		for (size_t i = 0; i < textures.size(); i++)
		{
			assert(textures[i].textureHash > 0);
			texs[i] = textures_[textures[i].textureIndex];
			assert(textures[i].textureHash == texs[i].textureHash);
		}

		drawQueue_.push_back(DrawCall(buffer, shader, texs, nrOfInstances, primitive, buffer.sizeOfIndex > 0));
	}
}//!sge