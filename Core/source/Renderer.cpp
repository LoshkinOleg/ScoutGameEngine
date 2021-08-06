#include "Renderer.h"

#include <iostream>

#include <glad/glad.h>
#include <xxhash.h>

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

	void VertexBuffer::Init(const uint32_t VAO, const std::vector<uint32_t>& VBOs, const uint32_t hash, const uint32_t vertexCount)
	{
		this->VAO = VAO;
		this->VBOs = std::vector<uint32_t>(VBOs);
		bufferDataHash = hash;
		this->vertexCount = vertexCount;
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
	void VertexBuffer::Render(Shader& shader, const std::vector<Texture2d>& textures, const uint32_t nrOfInstances, const int32_t primitive) const
	{
		shader.Bind();
		for (uint32_t i = 0; i < textures.size(); i++)
		{
			textures[i].Bind(i);
		}
		sge_CHECK_GL_ERROR();
		glBindVertexArray(VAO);
		glDrawArrays(primitive, 0, vertexCount);
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
			drawCall.buffer.Render(drawCall.shader, drawCall.textures, drawCall.nrOfInstances, drawCall.primitive);
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
		int8_t errorMsg[1024];
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
		// TODO: implement this.
		return VertexBufferHandle();
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

		vertexBuffer.Init(VAO, { VBO }, hash, (uint32_t)vertices.size() / stride);

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

		drawQueue_.push_back(DrawCall_(buffer, shader, texs, nrOfInstances, primitive));
	}

	Shader* ShaderHandle::operator->()
	{
		return &Engine::Get().GetRenderer().GetShader(shaderIndex, shaderSrcHash);
	}

	uint32_t textureIndex = 0;
	uint32_t textureHash = 0;

	Texture2d* Texture2dHandle::operator->()
	{
		return &Engine::Get().GetRenderer().GetTexture(textureIndex, textureHash);
	}

	VertexBuffer* VertexBufferHandle::operator->()
	{
		return &Engine::Get().GetRenderer().GetVertexBuffer(vertexBufferIndex, bufferDataHash);
	}
}//!sge