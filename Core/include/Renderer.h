#pragma once

#include <vector>
#include <map>
#include <string_view>

#include "globals.h"
#include "macros.h"

namespace sge
{
	struct ShaderSrcHandle;
	struct KtxHandle;
	struct GltfHandle;

	struct Shader
	{
		using Hash = uint32_t;

		uint32_t PROGRAM = 0;
		uint32_t shaderSrcHash = 0;
		std::map<Hash, uint32_t> uniformLocationCache = {};

		void Init(const uint32_t program, const uint32_t hash);
		void Destroy();

		void Bind() const;

		int32_t GetUniformLocation(const std::string_view name);
		void SetInt(const std::string_view name, const int32_t value);
	};

	struct ShaderHandle
	{
		uint32_t shaderIndex = 0;
		uint32_t shaderSrcHash = 0;

		Shader* operator->();
	};

	struct Texture2d
	{
		uint32_t TEX = 0;
		uint32_t textureHash = 0;

		void Init(const uint32_t tex, const uint32_t hash);
		void Destroy();

		void Bind(const uint32_t textureUnit) const;
	};

	struct Texture2dHandle
	{
		uint32_t textureIndex = 0;
		uint32_t textureHash = 0;

		Texture2d* operator->();
	};

	struct VertexBuffer
	{
		uint32_t VAO = 0, EBO = 0;
		std::vector<uint32_t> VBOs = {};
		uint32_t bufferDataHash = 0;
		uint32_t vertexCount = 0;

		void Init(const uint32_t VAO, const uint32_t EBO, const std::vector<uint32_t>& VBOs, const uint32_t hash, const uint32_t vertexCount);
		void Destroy();
		void Render(Shader& shader, const std::vector<Texture2d>& textures, const uint32_t nrOfInstances, const int32_t primitive) const;
	};

	struct VertexBufferHandle
	{
		uint32_t vertexBufferIndex = 0;
		uint32_t bufferDataHash = 0;

		VertexBuffer* operator->();
	};

	class Renderer
	{
		friend class Engine;
		sge_ALLOW_CONSTRUCTION(Renderer);

		struct DrawCall_
		{
			DrawCall_() = delete;
			~DrawCall_() = default;
			DrawCall_(const VertexBuffer& buffer, Shader& shader, const std::vector<Texture2d>& textures, const uint32_t nrOfInstances, const int32_t primitive) : buffer(buffer), shader(shader), textures(textures), nrOfInstances(nrOfInstances), primitive(primitive) {}
			
			const VertexBuffer& buffer;
			Shader& shader;
			const std::vector<Texture2d> textures;  // TODO: switch for a non heap type of container.
			const uint32_t nrOfInstances;
			const int32_t primitive;
		};

		constexpr static const int32_t CLEAR_FLAGS_ = GL_COLOR_BUFFER_BIT;
		constexpr static const float CLEAR_COLOR_[4] = { 0.3f, 0.0f, 0.3f, 1.0f };

		std::vector<Shader> shaders_ = {}; // TODO: use hashes to avoid duplicate resources
		std::vector<Texture2d> textures_ = {}; // TODO: use hashes to avoid duplicate resources
		std::vector<VertexBuffer> vertexBuffers_ = {}; // TODO: use hashes to avoid duplicate resources
		std::vector<DrawCall_> drawQueue_ = {}; // TODO: use a fixed size container

	public:
		sge_DISALLOW_COPY(Renderer);

		void Init();
		void Shutdown();
		void Update();

		Shader& GetShader(const uint32_t shaderIndex, const uint32_t shaderSrcHash);
		Texture2d& GetTexture(const uint32_t textureIndex, const uint32_t textureHash);
		VertexBuffer& GetVertexBuffer(const uint32_t vertexBufferIndex, const uint32_t bufferDataHash);

		ShaderHandle CreateShader(ShaderSrcHandle& handle);
		Texture2dHandle CreateTexture2d(KtxHandle& handle);
		VertexBufferHandle CreateVertexBuffer(GltfHandle& handle);
		VertexBufferHandle CreateVertexBuffer(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const std::vector<uint32_t>& layout);

		void ScheduleToBeDrawn(VertexBufferHandle bufferHandle, ShaderHandle shaderHandle, const std::vector<Texture2dHandle>& textures, const uint32_t nrOfInstances, const int32_t primitive);
	};
}//!sge
