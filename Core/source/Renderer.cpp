#include "Renderer.h"

#include <iostream>

#include <xxhash.h>
#include <glm/gtx/quaternion.hpp>

#include "Engine.h"

namespace sge
{

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
		models_.Clear();
		meshes_.Clear();
		vertexBuffers_.Clear();
		textures_.Clear();
		shaders_.Clear();
		drawQueue_.clear();
	}
	void Renderer::Update()
	{
		// TODO: update view matrix using player input.

		// TODO: separate draw calls into two queues based on whether the mesh has transparency or not.

		glClear(CLEAR_FLAGS_);

		// TODO: make a pass on all draw calls to update only the shaders that will be used.
		for(auto& pair : shaders_.GetData())
		{
			auto& shader = pair.second;
			switch(shader.illum)
			{
				case Shader::IlluminationModel::BLINN_PHONG_NORMALMAPPED:
				{
					shader.SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix_);
					shader.SetVec3("viewPos", glm::vec3(viewMatrix_[3]));
				}break;
				case Shader::IlluminationModel::BLINN_PHONG:
				{
					shader.SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix_);
					shader.SetVec3("viewPos", glm::vec3(viewMatrix_[3]));
				}break;
				case Shader::IlluminationModel::GOOCH:
				{
					shader.SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix_);
					shader.SetVec3("viewPos", glm::vec3(viewMatrix_[3]));
				}break;
				case Shader::IlluminationModel::GIZMO:
				{
					shader.SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix_);
				}break;
				case Shader::IlluminationModel::ALBEDO_ONLY: break;
				default:
				{
					sge_ERROR("Unexpected illumination model!");
				}break;
			}
		}

		const size_t queueLen = drawQueue_.size();
		for(size_t modelIdx = 0; modelIdx < queueLen; modelIdx++)
		{
			BlinnPhongModel& model = *drawQueue_[modelIdx].model;
			Shader& shader = *drawQueue_[modelIdx].shader;
			const int32_t primitive = drawQueue_[modelIdx].primitive;

			std::vector<glm::mat4> transforms = FrustumCulling_(RESOLUTION, FULL_FOV[0], WINDOW_PROJECTION_NEAR, WINDOW_PROJECTION_FAR, model.radius, model.transformsBegin, model.transformsEnd);
			if(transforms.size() < 1) continue;

			assert(shader.PROGRAM > 0);
			glUseProgram(shader.PROGRAM);
			sge_CHECK_GL_ERROR();

			const size_t meshLen = model.meshes.size();
			for(size_t meshIdx = 0; meshIdx < meshLen; meshIdx++)
			{
				BlinnPhongMesh& mesh = *model.meshes[meshIdx];

				assert(mesh.VAO > 0);
				glBindVertexArray(mesh.VAO);
				sge_CHECK_GL_ERROR();

				switch(shader.shadingMode)
				{
					case Shader::IlluminationModel::BLINN_PHONG_NORMALMAPPED:
					{
						assert(mesh.shininess > 0.0f);
						shader.SetFloat("shininess", mesh.shininess);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, mesh.albedoMap->TEX);
						sge_CHECK_GL_ERROR();
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, mesh.specularMap->TEX);
						sge_CHECK_GL_ERROR();
						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, mesh.normalMap->TEX);
						sge_CHECK_GL_ERROR();
					}break;
					case Shader::IlluminationModel::BLINN_PHONG:
					{
						assert(mesh.shininess > 0.0f);
						shader.SetFloat("shininess", mesh.shininess);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, mesh.albedoMap->TEX);
						sge_CHECK_GL_ERROR();
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, mesh.specularMap->TEX);
						sge_CHECK_GL_ERROR();
					}break;
					case Shader::IlluminationModel::GOOCH:
					{
						shader.SetVec3("baseColor", mesh.color);
					}break;
					case Shader::IlluminationModel::GIZMO:
					{
						shader.SetVec3("baseColor", mesh.color);
					}break;
					case Shader::IlluminationModel::ALBEDO_ONLY:
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, mesh.albedoMap->TEX);
						sge_CHECK_GL_ERROR();
					}break;
					default:
					{
						sge_ERROR("Unexpected illumination model!");
					}break;
				}

				glBindBuffer(GL_ARRAY_BUFFER, model.transformsVertexBuffer->VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, transforms.size() * sizeof(glm::mat4), transforms.data());
				sge_CHECK_GL_ERROR();
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
				glEnableVertexAttribArray(7);
				glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
				sge_CHECK_GL_ERROR();
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);
				glVertexAttribDivisor(7, 1);
				sge_CHECK_GL_ERROR();

				if(mesh.indexType > 0)
				{
					glDrawElementsInstanced(primitive, mesh.nrOfVertices, mesh.indexType, 0, transforms.size());
				}
				else
				{
					glDrawArraysInstanced(primitive, 0, mesh.nrOfVertices, transforms.size());
				}
			}
		}

		// TODO: draw transparent objects.

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		drawQueue_.clear();
	}

	Shader& Renderer::GetShader(const ShaderHandle& handle)
	{
		return shaders_.Access(handle);
	}

	Texture& Renderer::GetTexture(const TextureHandle& handle)
	{
		return textures_.Access(handle);
	}

	VertexBuffer& Renderer::GetVertexBuffer(const VertexBufferHandle& handle)
	{
		return vertexBuffers_.Access(handle);
	}

	BlinnPhongMesh& Renderer::GetMesh(const BlinnPhongMeshHandle& handle)
	{
		return meshes_.Access(handle);
	}

	BlinnPhongModel& Renderer::GetModel(const BlinnPhongModelHandle& handle)
	{
		return models_.Access(handle);
	}

	ShaderHandle Renderer::CreateShader(const ShaderDataHandle& handle, const Shader::IlluminationModel illum)
	{
		uint32_t VERT = 0, FRAG = 0;
		int32_t success = false;
#ifdef _DEBUG
		int8_t errorMsg[1024];
#endif
		const auto& shaderData = *handle;
		const char* vertSrc = shaderData.vertexCode.c_str();
		const char* fragSrc = shaderData.fragmentCode.c_str();
		assert(
			(vertSrc != "" && vertSrc != "\0") &&
			(fragSrc != "" && fragSrc != "\0"));

		VERT = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(VERT, 1, &vertSrc, NULL);
		sge_CHECK_GL_ERROR();
		glCompileShader(VERT);
		sge_CHECK_GL_ERROR();
		glGetShaderiv(VERT, GL_COMPILE_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if(!success) { sge_ERROR(errorMsg); }

		FRAG = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(FRAG, 1, &fragSrc, NULL);
		sge_CHECK_GL_ERROR();
		glCompileShader(FRAG);
		sge_CHECK_GL_ERROR();
		glGetShaderiv(FRAG, GL_COMPILE_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if(!success) { sge_ERROR(errorMsg); }

		uint32_t PROGRAM = glCreateProgram();
		glAttachShader(PROGRAM, VERT);
		glAttachShader(PROGRAM, FRAG);
		sge_CHECK_GL_ERROR();
		glLinkProgram(PROGRAM);
		sge_CHECK_GL_ERROR();
		glGetProgramiv(PROGRAM, GL_LINK_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if(!success) { sge_ERROR(errorMsg); }

		glDeleteShader(VERT);
		glDeleteShader(FRAG);
		sge_CHECK_GL_ERROR();

		Shader newElement;
		newElement.PROGRAM = PROGRAM;
		newElement.hash = handle.hash;
		assert(newElement.hash > 0);
		newElement.shadingMode = illum;


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
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

		for(size_t layer = 0; layer < layers; layer++)
		{
			for(size_t face = 0; face < faces; face++)
			{
				for(size_t level = 0; level < levels; level++)
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

	Handle<VertexBuffer> Renderer::CreateVertexBuffer(const VertexBuffer::Definition& def, const Hash& accumulatedHash)
	{
		vertexBuffers_.push_back({});
		auto& vb = vertexBuffers_.back();
		vb.resourceData.Init(def);
		vb.hash.Generate(&vb.resourceData.VBO, sizeof(uint32_t));
		vb.hash.Accumulate(accumulatedHash);

		Handle<VertexBuffer> handle;
		handle.hash = vb.hash;
		handle.ptr = &vb;
		return handle;
	}

	BlinnPhongMeshHandle Renderer::CreateMesh(const MeshData_& data)
	{
		uint32_t VAO = 0, EBO = 0;
		VertexBufferHandle positions;
		VertexBufferHandle normals;
		VertexBufferHandle tangents;
		VertexBufferHandle uvs;
		TextureHandle albedoMap;
		TextureHandle specularMap;
		TextureHandle normalMap;
		const float shininess = data.shininess;
		uint32_t nrOfVertices = 0;
		int32_t indexType = 0;
		const bool isTransparent = false; // TODO: analyse alphaMap to set this.

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * data.indexType, data.indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);

		std::vector<float> positionsData;
		positionsData.insert
		(
			positionsData.end(),
			reinterpret_cast<const float*>(data.positions.data()),
			reinterpret_cast<const float*>(data.positions.data() + data.positions.size())
		);
		positions = CreateVertexBuffer(positionsData, GL_STATIC_DRAW);
		assert(positions.hash > 0);

		std::vector<float> normalsData;
		normalsData.insert
		(
			normalsData.end(),
			reinterpret_cast<const float*>(data.normals.data()),
			reinterpret_cast<const float*>(data.normals.data() + data.normals.size())
		);
		normals = CreateVertexBuffer(normalsData, GL_STATIC_DRAW);

		std::vector<float> tangentsData;
		tangentsData.insert
		(
			tangentsData.end(),
			reinterpret_cast<const float*>(data.tangents.data()),
			reinterpret_cast<const float*>(data.tangents.data() + data.tangents.size())
		);
		tangents = CreateVertexBuffer(tangentsData, GL_STATIC_DRAW);

		std::vector<float> uvsData;
		uvsData.insert
		(
			uvsData.end(),
			reinterpret_cast<const float*>(data.uvs.data()),
			reinterpret_cast<const float*>(data.uvs.data() + data.uvs.size())
		);
		uvs = CreateVertexBuffer(uvsData, GL_STATIC_DRAW);

		if(data.indexType != MeshData_::IndexType::INVALID)
		{
			nrOfVertices = data.indices.size();
		}
		else
		{
			// TODO: find a way to do this in a generic manner.
			nrOfVertices = positionsData.size() / 3;
		}
		indexType = data.indexType == MeshData_::IndexType::INVALID ? 0 : GL_UNSIGNED_SHORT; // TODO: use indexType to set the appropriate index type.

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, positions->VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		if(normalsData.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, normals->VBO);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
		if(tangentsData.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, tangents->VBO);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		}
		if(uvsData.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, uvs->VBO);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if(data.albedoMap.hash > 0)
		{
			albedoMap = CreateTexture(data.albedoMap);
		}
		if(data.specularMap.hash > 0)
		{
			specularMap = CreateTexture(data.specularMap);
		}
		if(data.normalMap.hash > 0)
		{
			normalMap = CreateTexture(data.normalMap);
		}

		BlinnPhongMesh newElement;
		newElement.positions = positions;
		assert(newElement.positions->hash > 0);
		newElement.normals = normals;
		// assert(newElement.normals->hash > 0);
		newElement.tangents = tangents;
		// assert(newElement.tangents->hash > 0);
		newElement.EBO = EBO;
		newElement.VAO = VAO;
		newElement.indexType = indexType;
		// assert(newElement.indexType == GL_UNSIGNED_SHORT);
		newElement.isTransparent = isTransparent;
		newElement.nrOfVertices = nrOfVertices;
		assert(newElement.nrOfVertices > 0);
		newElement.shininess = shininess;
		newElement.albedoMap = albedoMap;
		newElement.specularMap = specularMap;
		newElement.normalMap = normalMap;
		newElement.color = data.color;

		std::string accumulatedData = "";
		accumulatedData += std::to_string(positions->hash);
		if(normals.hash > 0)
		{
			accumulatedData += std::to_string(normals->hash);
		}
		if(tangents.hash > 0)
		{
			accumulatedData += std::to_string(tangents->hash);
		}
		if(uvs.hash > 0)
		{
			accumulatedData += std::to_string(uvs->hash);
		}
		accumulatedData += std::to_string(shininess);
		if(albedoMap.hash > 0)
		{
			accumulatedData += std::to_string(albedoMap->hash);
		}
		if(specularMap.hash > 0)
		{
			accumulatedData += std::to_string(specularMap->hash);
		}
		if(normalMap.hash > 0)
		{
			accumulatedData += std::to_string(normalMap->hash);
		}
		newElement.hash = XXH32(accumulatedData.data(), accumulatedData.size(), HASHING_SEED);
		assert(newElement.hash > 0);

		meshes_.Insert(newElement);

		BlinnPhongMeshHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	BlinnPhongModelHandle Renderer::CreateModel(const GltfDataHandle& handle, const std::vector<glm::mat4>& transforms)
	{
		BlinnPhongModel newElement;

		uint32_t hash = 0;
		std::vector<BlinnPhongMeshHandle> meshes;
		glm::mat4* transformsBegin = nullptr;
		glm::mat4* transformsEnd = nullptr;
		VertexBufferHandle transformsVBO;
		const float radius = 0.0f;

		assert(transforms.size() > 0);
		auto& rm = Engine::Get().GetResourceManager();
		transformsBegin = rm.AllocateTransforms(transforms);
		transformsEnd = transformsBegin + transforms.size();
		assert(transformsEnd > transformsBegin);

		std::vector<float> data;
		data.insert
		(
			data.end(),
			reinterpret_cast<const float*>(transforms.data()),
			reinterpret_cast<const float*>(transforms.data() + transforms.size())
		);
		transformsVBO = CreateVertexBuffer(data, GL_DYNAMIC_DRAW);
		assert(transformsVBO->hash > 0 && transformsVBO.hash > 0 && (transformsVBO->hash == transformsVBO.hash));

		std::string accumulatedData = "";

		auto meshesData = ProcessGltf_(handle);
		meshes.reserve(meshesData.size());
		for(const auto& mesh : meshesData)
		{
			assert(mesh.positions.size() > 0);
			assert(mesh.normals.size() > 0);
			assert(mesh.tangents.size() > 0);
			assert(mesh.uvs.size() > 0);
			assert(mesh.indices.size() > 0);
			assert(mesh.shininess > 0.0f);
			assert(mesh.indexType > 0);
			meshes.push_back(CreateMesh(mesh));
			accumulatedData += std::to_string(meshes.front().hash);
		}
		hash = XXH32(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED);
		assert(hash > 0);

		newElement.hash = hash;
		newElement.meshes = meshes;
		newElement.radius = radius;
		newElement.transformsBegin = transformsBegin;
		newElement.transformsEnd = transformsEnd;
		newElement.transformsVertexBuffer = transformsVBO;
		models_.Insert(newElement);

		BlinnPhongModelHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	BlinnPhongModelHandle Renderer::CreateModel(const std::vector<float>& data, const std::vector<uint32_t>& layout, const std::vector<glm::mat4>& transforms, const glm::vec3 color)
	{
		BlinnPhongModel newElement;

		std::vector<BlinnPhongMeshHandle> meshes;
		glm::mat4* transformsBegin;
		glm::mat4* transformsEnd;
		VertexBufferHandle transformsVertexBuffer;
		const float radius = 0.0f; // TODO: set this

		assert(transforms.size() > 0);
		transformsBegin = Engine::Get().GetResourceManager().AllocateTransforms(transforms);
		transformsEnd = transformsBegin + transforms.size();
		assert(transformsEnd > transformsBegin);
		std::vector<float> transformsData;
		transformsData.insert
		(
			transformsData.end(),
			reinterpret_cast<const float*>(transforms.data()),
			reinterpret_cast<const float*>(transforms.data() + transforms.size())
		);
		transformsVertexBuffer = CreateVertexBuffer(transformsData, GL_DYNAMIC_DRAW);
		assert(transformsVertexBuffer->hash > 0 && transformsVertexBuffer.hash > 0 && (transformsVertexBuffer->hash == transformsVertexBuffer.hash));

		MeshData_ meshData;
		meshData.color = color;
		meshData.indexType = MeshData_::IndexType::INVALID;
		assert(layout.size() == 1 && layout[0] == 3); // TODO: actually allow user to create own mesh with own data layout.
		meshData.positions.insert
		(
			meshData.positions.end(),
			reinterpret_cast<const glm::vec3*>(data.data()),
			reinterpret_cast<const glm::vec3*>(data.data() + data.size())
		);
		meshData.shininess = 0.0f;
		meshes.push_back(CreateMesh(meshData));

		newElement.meshes = meshes;
		newElement.radius = radius;
		newElement.transformsBegin = transformsBegin;
		newElement.transformsEnd = transformsEnd;
		newElement.transformsVertexBuffer = transformsVertexBuffer;
		std::string accumulatedData = "";
		accumulatedData += std::to_string(newElement.meshes[0].hash);
		accumulatedData += std::to_string(newElement.transformsVertexBuffer.hash);
		newElement.hash = XXH32(accumulatedData.c_str(), accumulatedData.size(), HASHING_SEED);
		models_.Insert(newElement);

		BlinnPhongModelHandle returnVal;
		returnVal.hash = newElement.hash;
		return returnVal;
	}

	void Renderer::Schedule(const BlinnPhongModelHandle& model, const ShaderHandle& shader, const int32_t primitive)
	{
		assert(model->hash > 0 && shader->hash > 0);

		DrawCall_ newElement;
		newElement.model = model;
		newElement.shader = shader;
		newElement.primitive = primitive;

		drawQueue_.push_back(newElement);
	}
}//!sge