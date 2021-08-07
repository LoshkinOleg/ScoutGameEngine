#include "Renderer.h"

#include <iostream>

// #include <glad/glad.h>
#include <xxhash.h>
#include <tiny_gltf.h>
#include <glm/gtx/quaternion.hpp>

#include "ResourceManager.h"
#include "Engine.h"

namespace sge
{
	void Shader::Init(const uint32_t program, const uint32_t hash)
	{
		PROGRAM = program;
		shaderSrcHash = hash;
		uniformLocationCache.clear();
	}
	void Shader::Destroy()
	{
		glDeleteProgram(PROGRAM);
		sge_CHECK_GL_ERROR();
		PROGRAM = 0;
		shaderSrcHash = 0;
		uniformLocationCache.clear();
	}

	int32_t Shader::GetUniformLocation(const std::string_view name)
	{
		const uint32_t hash = XXH32(name.data(), name.size(), HASHING_SEED);
		const auto match = uniformLocationCache.find(hash);
		if (match != uniformLocationCache.end()) // Name of uniform already known, use it.
		{
			return match->second;
		}
		else
		{
			const int location = glGetUniformLocation(PROGRAM, name.data());
			sge_CHECK_GL_ERROR();
			if (location < 0) { sge_ERROR("Trying to get the location of a non existent uniform!"); }
			uniformLocationCache.insert({ hash, location }); // Add the new entry.
			return location;
		}
	}

	void Shader::SetInt(const std::string_view name, const int32_t value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform1i(location, value);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetVec3(const std::string_view name, const glm::vec3 value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform3fv(location, 1, &value[0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetMat4(const std::string_view name, const glm::mat4 & value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetMat3(const std::string_view name, const glm::mat3& value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::Bind() const
	{
		glUseProgram(PROGRAM);
		sge_CHECK_GL_ERROR();
	}

	void Texture2d::Init(const uint32_t tex, const uint32_t hash)
	{
		TEX = tex;
		textureHash = hash;
	}
	void Texture2d::Destroy()
	{
		glDeleteTextures(1, &TEX);
		TEX = 0;
		textureHash = 0;
	}

	void Texture2d::Bind(const uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, TEX);
		sge_CHECK_GL_ERROR();
	}

	void VertexBuffer::Init(const uint32_t VAO, const std::vector<uint32_t>& VBOs, const uint32_t hash, const uint32_t vertexCount, const uint32_t sizeOfIndex)
	{
		this->VAO = VAO;
		this->VBOs = std::vector<uint32_t>(VBOs);
		bufferDataHash = hash;
		this->vertexCount = vertexCount;
		this->sizeOfIndex = sizeOfIndex;
	}
	void VertexBuffer::Destroy()
	{
		glDeleteBuffers((GLsizei)VBOs.size(), VBOs.data());
		glDeleteVertexArrays(1, &VAO);
		sge_CHECK_GL_ERROR();
		VAO = 0;
		VBOs.clear();
		bufferDataHash = 0;
	}
	void VertexBuffer::Render(Shader& shader, const std::vector<Texture2d>& textures, const uint32_t nrOfInstances, const int32_t primitive, const bool indexedData) const
	{
		shader.Bind();
		for (uint32_t i = 0; i < textures.size(); i++)
		{
			textures[i].Bind(i);
		}
		sge_CHECK_GL_ERROR();
		glBindVertexArray(VAO);
		if (indexedData)
		{
			glDrawElementsInstanced(primitive, vertexCount, sizeOfIndex, 0, nrOfInstances);
		}
		else
		{
			glDrawArraysInstanced(primitive, 0, vertexCount, nrOfInstances);
		}
		sge_CHECK_GL_ERROR();
	}

	void Renderer::Init()
	{
		glClearColor(CLEAR_COLOR_[0], CLEAR_COLOR_[1], CLEAR_COLOR_[2], CLEAR_COLOR_[3]);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}
	void Renderer::Shutdown()
	{
		for (auto& shader : shaders_)
		{
			shader.Destroy();
		}
		for (auto& texture : textures_)
		{
			texture.Destroy();
		}
		for (auto& vertexBuffer : vertexBuffers_)
		{
			vertexBuffer.Destroy();
		}
	}
	void Renderer::Update()
	{
		glClear(CLEAR_FLAGS_);

		for (uint32_t i = 0; i < drawQueue_.size(); i++)
		{
			const auto drawCall = drawQueue_[i];
			drawCall.buffer.Render(drawCall.shader, drawCall.textures, drawCall.nrOfInstances, drawCall.primitive, drawCall.indexedData);
		}
		drawQueue_.clear();
	}

	Shader& Renderer::GetShader(const uint32_t shaderIndex, const uint32_t shaderSrcHash)
	{
		assert(shaderSrcHash > 0);
		Shader& shader = shaders_[shaderIndex];
		assert(shaderSrcHash == shader.shaderSrcHash);
		return shader;
	}
	Texture2d& Renderer::GetTexture(const uint32_t textureIndex, const uint32_t textureHash)
	{
		assert(textureHash > 0);
		Texture2d& texture = textures_[textureIndex];
		assert(textureHash == texture.textureHash);
		return texture;
	}
	VertexBuffer& Renderer::GetVertexBuffer(const uint32_t vertexBufferIndex, const uint32_t bufferDataHash)
	{
		assert(bufferDataHash > 0);
		VertexBuffer& buffer = vertexBuffers_[vertexBufferIndex];
		assert(bufferDataHash == buffer.bufferDataHash);
		return buffer;
	}

	ShaderHandle Renderer::CreateShader(ShaderSrcHandle& handle)
	{
		uint32_t VERT = 0, FRAG = 0;
		int32_t success = false;
#ifdef _DEBUG
		int8_t errorMsg[1024];
#endif
		const char* vertSrc = handle->vertexShader.c_str();
		const char* fragSrc = handle->fragmentShader.c_str();
		shaders_.push_back(Shader());
		Shader& shader = shaders_.front();

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

		shader.Init(PROGRAM, handle.resourceHash);

		return { (uint32_t)(shaders_.size() - 1) , handle.resourceHash };
	}
	Texture2dHandle Renderer::CreateTexture2d(KtxHandle& handle)
	{
		uint32_t TEX = 0;
		textures_.push_back(Texture2d());
		Texture2d& texture = textures_.front();

		gli::gl translator(gli::gl::PROFILE_KTX);
		const gli::gl::format format = translator.translate(handle->texture.format(), handle->texture.swizzles());
		GLenum target = translator.translate(handle->texture.target());
		assert(target == GL_TEXTURE_2D);

		const GLint levels = static_cast<GLint>(handle->texture.levels());
		const GLint layers = static_cast<GLint>(handle->texture.layers());
		const GLint faces = static_cast<GLint>(handle->texture.faces());

		glGenTextures(1, &TEX);
		glBindTexture(target, TEX);
		glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(levels - 1));
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, format.Swizzles[0]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, format.Swizzles[1]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, format.Swizzles[2]);
		glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, format.Swizzles[3]);
		sge_CHECK_GL_ERROR();

		glm::tvec3<GLsizei> extents(handle->texture.extent());

		glTexStorage2D
		(
			target,
			levels,
			format.Internal,
			extents.x,
			extents.y
		);
		sge_CHECK_GL_ERROR();

		assert(gli::is_compressed(handle->texture.format()));

		for (size_t layer = 0; layer < layers; layer++)
		{
			for (size_t face = 0; face < faces; face++)
			{
				for (size_t level = 0; level < levels; level++)
				{
					extents = glm::tvec3<GLsizei>(handle->texture.extent(level));
					glCompressedTexSubImage2D
					(
						target,
						static_cast<GLint>(level),
						0,
						0,
						extents.x,
						extents.y,
						format.Internal,
						static_cast<GLsizei>(handle->texture.size(level)),
						handle->texture.data(layer, face, level)
					);
					sge_CHECK_GL_ERROR();
				}
			}
		}

		texture.Init(TEX, handle.resourceHash);

		return { (uint32_t)(textures_.size() - 1) , handle.resourceHash };
	}
	VertexBufferHandle Renderer::CreateVertexBuffer(GltfHandle& handle)
	{
		const Mesh_ mesh = ProcessGltf_(handle);

		uint32_t VAO = 0, EBO = 0;
		std::vector<uint32_t> VBOs = std::vector<uint32_t>(5, 0);
		const size_t floatStride = ((4 * 3) + (1 * 2)); // pos, norm, tan, bitan = 3 floats, uvs = 2 floats   = 56 bytes or 14 floats

		// TODO: check here we're not loading data that's already on the GPU using the hash.

		assert
		(
			mesh.positions.size() == mesh.normals.size() &&
			mesh.positions.size() == mesh.tangents.size() &&
			mesh.positions.size() == mesh.bitangents.size() &&
			mesh.positions.size() == mesh.uvs.size()
		);
		
		vertexBuffers_.push_back(VertexBuffer());
		VertexBuffer& vertexBuffer = vertexBuffers_.front();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(5, VBOs.data());
		glBindVertexArray(VAO);
		sge_CHECK_GL_ERROR();

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, mesh.positions.size() * sizeof(glm::vec3), mesh.positions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(glm::vec3), mesh.normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
		glBufferData(GL_ARRAY_BUFFER, mesh.tangents.size() * sizeof(glm::vec3), mesh.tangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
		glBufferData(GL_ARRAY_BUFFER, mesh.bitangents.size() * sizeof(glm::vec3), mesh.bitangents.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]);
		glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(glm::vec2), mesh.uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

		sge_CHECK_GL_ERROR();

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint16_t), mesh.indices.data(), GL_STATIC_DRAW);

		std::string accumulatedData = "";
		accumulatedData = std::to_string(XXH32(mesh.positions.data(), sizeof(glm::vec3) * mesh.positions.size(), HASHING_SEED));
		accumulatedData = std::to_string(XXH32(mesh.normals.data(), sizeof(glm::vec3) * mesh.normals.size(), HASHING_SEED));
		accumulatedData = std::to_string(XXH32(mesh.tangents.data(), sizeof(glm::vec3) * mesh.tangents.size(), HASHING_SEED));
		accumulatedData = std::to_string(XXH32(mesh.bitangents.data(), sizeof(glm::vec3) * mesh.bitangents.size(), HASHING_SEED));
		accumulatedData = std::to_string(XXH32(mesh.uvs.data(), sizeof(glm::vec2) * mesh.uvs.size(), HASHING_SEED));
		const uint32_t hash = XXH32(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED);

		vertexBuffer.Init(VAO, VBOs, hash, (uint32_t)mesh.indices.size(), GL_UNSIGNED_SHORT);

		return { (uint32_t)(vertexBuffers_.size() - 1) , hash };
	}

	Renderer::Mesh_ Renderer::ProcessGltf_(GltfHandle& handle) const
	{
		Mesh_ returnVal = {};
		tinygltf::Model& model = *handle->gltf;

		uint32_t accumulatedVertexCount = 0;
		uint32_t currentMeshVertexCount = 0;

		for (uint32_t meshIdx = 0; meshIdx < model.meshes.size(); meshIdx++)
		{
			tinygltf::Mesh& mesh = model.meshes[meshIdx];
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

			{ // Positions.
				auto& accessor = model.accessors[attributes["POSITION"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3);
				assert(model.meshes.size() == model.nodes.size()); // Note: this DOES NOT guarantee that there is a one-to-one matching between nodes and meshes, we just assume it does...

				accumulatedVertexCount += accessor.count;
				currentMeshVertexCount = accessor.count;
				std::vector<glm::vec3> positions;
				positions.insert
				(
					positions.end(),
					reinterpret_cast<glm::vec3*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec3)),
					reinterpret_cast<glm::vec3*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec3)) + (bufferView.byteLength / sizeof(glm::vec3))
				);

				glm::mat4 modelMatrix = (model.nodes[meshIdx].matrix.size() > 0) ?
					glm::mat4(
						model.nodes[meshIdx].matrix[0], model.nodes[meshIdx].matrix[1], model.nodes[meshIdx].matrix[2], model.nodes[meshIdx].matrix[3],
						model.nodes[meshIdx].matrix[4], model.nodes[meshIdx].matrix[5], model.nodes[meshIdx].matrix[6], model.nodes[meshIdx].matrix[7], 
						model.nodes[meshIdx].matrix[8], model.nodes[meshIdx].matrix[9], model.nodes[meshIdx].matrix[10], model.nodes[meshIdx].matrix[11], 
						model.nodes[meshIdx].matrix[12], model.nodes[meshIdx].matrix[13], model.nodes[meshIdx].matrix[14], model.nodes[meshIdx].matrix[15]) :
					IDENTITY_MAT4;
				
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

				for (size_t i = 0; i < positions.size(); i++)
				{
					positions[i] = modelMatrix * glm::vec4(positions[i], 1.0f);
				}

				returnVal.positions.insert
				(
					returnVal.positions.end(),
					positions.begin(),
					positions.end()
				);
			}

			{ // Indices.
				auto& accessor = model.accessors[indicesIdx];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT && accessor.type == TINYGLTF_TYPE_SCALAR);
				size_t currentMeshIndices = returnVal.indices.end() - returnVal.indices.begin();
				returnVal.indices.insert
				(
					returnVal.indices.end(),
					reinterpret_cast<uint16_t*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(uint16_t)),
					reinterpret_cast<uint16_t*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(uint16_t)) + (bufferView.byteLength / sizeof(uint16_t))
				);
				for (; currentMeshIndices < returnVal.indices.size(); currentMeshIndices++)
				{
					returnVal.indices[currentMeshIndices] += (accumulatedVertexCount - currentMeshVertexCount);
				}
			}

			const size_t thisMeshesNormalsStart = returnVal.normals.end() - returnVal.normals.begin();
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

				glm::mat4 modelMatrix = (model.nodes[meshIdx].matrix.size() > 0) ?
					glm::mat4(
						model.nodes[meshIdx].matrix[0], model.nodes[meshIdx].matrix[1], model.nodes[meshIdx].matrix[2], model.nodes[meshIdx].matrix[3],
						model.nodes[meshIdx].matrix[4], model.nodes[meshIdx].matrix[5], model.nodes[meshIdx].matrix[6], model.nodes[meshIdx].matrix[7],
						model.nodes[meshIdx].matrix[8], model.nodes[meshIdx].matrix[9], model.nodes[meshIdx].matrix[10], model.nodes[meshIdx].matrix[11],
						model.nodes[meshIdx].matrix[12], model.nodes[meshIdx].matrix[13], model.nodes[meshIdx].matrix[14], model.nodes[meshIdx].matrix[15]) :
					IDENTITY_MAT4;

				if (modelMatrix == IDENTITY_MAT4)
				{
					const glm::quat rotation = (model.nodes[meshIdx].rotation.size() > 0) ?
						glm::quat(model.nodes[meshIdx].rotation[0], model.nodes[meshIdx].rotation[1], model.nodes[meshIdx].rotation[2], model.nodes[meshIdx].rotation[3]) :
						IDENTITY_QUAT;
					const glm::vec3 scale = (model.nodes[meshIdx].scale.size() > 0) ?
						glm::vec3(model.nodes[meshIdx].scale[0], model.nodes[meshIdx].scale[1], model.nodes[meshIdx].scale[2]) :
						ONE_VEC3;
					modelMatrix *= glm::toMat4(rotation);
					modelMatrix = glm::scale(modelMatrix, scale);
				}

				const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

				for (size_t i = 0; i < normals.size(); i++)
				{
					normals[i] = normalMatrix * normals[i];
				}

				returnVal.normals.insert
				(
					returnVal.normals.end(),
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
				std::vector<glm::vec3> normals = std::vector<glm::vec3>(thisMeshesNormalsStart == 0 ? returnVal.normals.begin() : returnVal.normals.begin() + thisMeshesNormalsStart, returnVal.normals.end());
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
					tangents[i] = glm::vec3(tangentsVec4[i]);
					bitangents[i] = glm::cross(normals[i], glm::vec3(tangentsVec4[i])) * tangentsVec4[i].w;
				}

				// Note: I have a sneaking suspicion that the tangents and bitangents need to be multiplied by a matrix to take into account the mesh'es transform.

				returnVal.tangents.insert
				(
					returnVal.tangents.end(),
					tangents.begin(),
					tangents.end()
				);
				returnVal.bitangents.insert
				(
					returnVal.bitangents.end(),
					bitangents.begin(),
					bitangents.end()
				);
			}

			{ // Uvs.
				auto& accessor = model.accessors[attributes["TEXCOORD_0"]];
				auto& bufferView = model.bufferViews[accessor.bufferView];
				auto& buffer = model.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC2);
				returnVal.uvs.insert
				(
					returnVal.uvs.end(),
					reinterpret_cast<glm::vec2*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec2)),
					reinterpret_cast<glm::vec2*>(buffer.data.data()) + (bufferView.byteOffset / sizeof(glm::vec2)) + (bufferView.byteLength / sizeof(glm::vec2))
				);
			}

			// TODO: material
		}

		return returnVal;
	}

	VertexBufferHandle Renderer::CreateVertexBuffer(const std::vector<float>& vertices, const std::vector<uint32_t>& layout)
	{
		uint32_t VAO = 0, VBO = 0;
		const uint32_t stride = std::accumulate(layout.begin(), layout.end(), 0);

		assert(vertices.size() % stride == 0);
		for (const auto& element : layout)
		{
			assert(element < 5);
		}

		const uint32_t hash = XXH32(vertices.data(), vertices.size() * sizeof(float), HASHING_SEED);

		// TODO: check here we're not loading data that's already on the GPU using the hash.

		vertexBuffers_.push_back(VertexBuffer());
		VertexBuffer& vertexBuffer = vertexBuffers_.front();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		sge_CHECK_GL_ERROR();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		sge_CHECK_GL_ERROR();

		const uint32_t layoutSize = (uint32_t)layout.size();
		uint32_t accumulatedOffset = 0;
		for (uint32_t i = 0; i < layoutSize; i++)
		{
			glVertexAttribPointer(i, layout[i], GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(uint64_t)accumulatedOffset);
			glEnableVertexAttribArray(i);
			sge_CHECK_GL_ERROR();
			accumulatedOffset += sizeof(float) * layout[i];
		}
		sge_CHECK_GL_ERROR();

		vertexBuffer.Init(VAO, { VBO }, hash, (uint32_t)vertices.size() / stride, 0);

		return { (uint32_t)(vertexBuffers_.size() - 1) , hash };
	}

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

		drawQueue_.push_back(DrawCall_(buffer, shader, texs, nrOfInstances, primitive, buffer.sizeOfIndex > 0));
	}

	Shader* ShaderHandle::operator->()
	{
		return &Engine::Get().GetRenderer().GetShader(shaderIndex, shaderSrcHash);
	}

	Texture2d* Texture2dHandle::operator->()
	{
		return &Engine::Get().GetRenderer().GetTexture(textureIndex, textureHash);
	}

	VertexBuffer* VertexBufferHandle::operator->()
	{
		return &Engine::Get().GetRenderer().GetVertexBuffer(vertexBufferIndex, bufferDataHash);
	}

	VertexBufferHandle Renderer::OLD_CreateVertexBuffer_(GltfHandle& handle)
	{
		// Code adapted from https://github.com/syoyo/tinygltf/blob/master/examples/glview/glview.cc .
		// Back magic gltf fuckery.

		const auto& model = *handle->gltf;

		vertexBuffers_.push_back(VertexBuffer());
		VertexBuffer& vertexBuffer = vertexBuffers_.front();
		uint32_t VAO = 0, EBO = 0;
		std::vector<uint32_t> VBOs = std::vector<uint32_t>(model.bufferViews.size() - 1, 0); // -1 since one buffer is for the indices.
		std::vector<uint32_t> hashes;
		uint32_t nrOfVertices = 0;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		sge_CHECK_GL_ERROR();

		std::function<uint64_t(const int32_t type)> sizeOfComponent = [](const int32_t type)
		{
			switch (type)
			{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				case TINYGLTF_COMPONENT_TYPE_BYTE:
					return sizeof(char);
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				case TINYGLTF_COMPONENT_TYPE_SHORT:
					return sizeof(short);
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				case TINYGLTF_COMPONENT_TYPE_INT:
					return sizeof(int);
				case TINYGLTF_COMPONENT_TYPE_FLOAT:
					return sizeof(float);
				case TINYGLTF_COMPONENT_TYPE_DOUBLE:
					return sizeof(double);
				default:
					return uint64_t(0);
			}
		};

		for (size_t bufferViewIdx = 0; bufferViewIdx < model.bufferViews.size(); bufferViewIdx++)
		{
			const tinygltf::BufferView& view = model.bufferViews[bufferViewIdx];

			int32_t sparseAccessor = -1;
			for (size_t accessorIdx = 0; accessorIdx < model.accessors.size(); accessorIdx++)
			{
				const auto& accessor = model.accessors[accessorIdx];
				if (accessor.bufferView == bufferViewIdx && accessor.sparse.isSparse)
				{
					sparseAccessor = accessorIdx;
				}
			}
			if (view.target == GL_ELEMENT_ARRAY_BUFFER)
			{
				const tinygltf::Buffer& buffer = model.buffers[view.buffer];
				glGenBuffers(1, &EBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				sge_CHECK_GL_ERROR();
				if (sparseAccessor < 0) // Regular accessor.
				{
					hashes.push_back(XXH32(&buffer.data.at(0), view.byteLength, HASHING_SEED));
					nrOfVertices = view.byteLength / sizeof(uint16_t);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, view.byteLength, &buffer.data.at(0) + view.byteOffset, GL_STATIC_DRAW);
					sge_CHECK_GL_ERROR();
				}
				else // Sparse accessor.
				{
					sge_ERROR("Handling of sparse accessors isn't implemented!");
				}
			}
			else
			{
				assert(view.target == GL_ARRAY_BUFFER);
				const tinygltf::Buffer& buffer = model.buffers[view.buffer];
				glGenBuffers(1, &VBOs[bufferViewIdx]);
				glBindBuffer(GL_ARRAY_BUFFER, VBOs[bufferViewIdx]);
				sge_CHECK_GL_ERROR();
				if (sparseAccessor < 0) // Regular accessor.
				{
					hashes.push_back(XXH32(&buffer.data.at(0), view.byteLength, HASHING_SEED));
					glBufferData(GL_ARRAY_BUFFER, view.byteLength, &buffer.data.at(0) + view.byteOffset, GL_STATIC_DRAW);
					sge_CHECK_GL_ERROR();
					static size_t lastBufferIndex;
					if (bufferViewIdx == 0)
					{
						lastBufferIndex = 0;
					}
					else
					{
						assert(lastBufferIndex == bufferViewIdx - 1);
					}
					glEnableVertexAttribArray(bufferViewIdx);
					glVertexAttribPointer(bufferViewIdx, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
					sge_CHECK_GL_ERROR();
				}
				else // Sparse accessor.
				{
					sge_ERROR("Handling of sparse accessors isn't implemented!");
				}
			}
		}

		std::string accumulatedHashes = "";
		for (size_t i = 0; i < hashes.size(); i++)
		{
			accumulatedHashes += std::to_string(hashes[i]);
		}

		// Note: apparently GL_UNSIGNED_SHORTs are used.
		vertexBuffer.Init(VAO, VBOs, XXH32(accumulatedHashes.c_str(), accumulatedHashes.size(), HASHING_SEED), nrOfVertices, GL_UNSIGNED_SHORT);
		sge_CHECK_GL_ERROR();

		return { (uint32_t)(vertexBuffers_.size() - 1) , vertexBuffer.bufferDataHash };
	}
}//!sge