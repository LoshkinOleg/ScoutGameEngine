#pragma once

#include <concepts>

#include "Resources.h"
#include "macros.h"
#include "globals.h"

namespace sge
{
	class Renderer
	{
		friend class Engine;
		sge_ALLOW_CONSTRUCTION(Renderer);

		constexpr static const int32_t CLEAR_FLAGS_ = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		constexpr static const float CLEAR_COLOR_[4] = { 0.3f, 0.0f, 0.3f, 1.0f };

		struct DrawCall_
		{
			ModelHandle model = {};
			ShaderHandle shader = {};
			int32_t primitive = GL_TRIANGLES;
		};

		struct MeshData_
		{
			// Value matches the size of the type in bytes.
			enum IndexType : uint32_t
			{
				INVALID = 0,
				UNSIGNED_BYTE = 1,
				SIGNED_BYTE = 1,
				UNSIGNED_SHORT = 2,
				SIGNED_SHORT = 2,
				UNSIGNED_INT = 4,
				SIGNED_INT = 4
			};

			std::vector<glm::vec3> positions = {};
			std::vector<glm::vec3> normals = {};
			std::vector<glm::vec3> tangents = {};
			std::vector<glm::vec2> uvs = {};
			std::vector<uint16_t> indices = {};
			KtxDataHandle albedoMap = {};
			KtxDataHandle specularMap = {};
			KtxDataHandle normalMap = {};
			float shininess = 0.0f;
			glm::vec3 color = WHITE;
			IndexType indexType = IndexType::INVALID;
		};

		ResourceContainer<Shader, ShaderHandle> shaders_ = {};
		ResourceContainer<VertexBuffer, VertexBufferHandle> vertexBuffers_ = {};
		ResourceContainer<Texture, TextureHandle> textures_ = {};
		ResourceContainer<Mesh, MeshHandle> meshes_ = {};
		ResourceContainer<Model, ModelHandle> models_ = {};

		std::vector<DrawCall_> drawQueue_ = {};

		glm::mat4 viewMatrix_ = DEFAULT_VIEW_MATRIX;

		static std::vector<MeshData_> ProcessGltf_(const GltfDataHandle& handle);
		static std::vector<glm::mat4> FrustumCulling_(const glm::ivec2 resolution, const float horizontalFullFov, const float nearPlane, const float farPlane, const float radius, const glm::mat4* const begin, const glm::mat4* const end);
		static void SortFrontToBack_(std::vector<glm::mat4>& transforms);
		static void SortBackToFront_(std::vector<glm::mat4>& transforms);

	public:
		sge_DISALLOW_COPY(Renderer);

		void Init();
		void Shutdown();
		void Update();

		Shader& GetShader(const ShaderHandle& handle);
		Texture& GetTexture(const TextureHandle& handle);
		VertexBuffer& GetVertexBuffer(const VertexBufferHandle& handle);
		Mesh& GetMesh(const MeshHandle& handle);
		Model& GetModel(const ModelHandle& handle);

		ShaderHandle CreateShader(const ShaderDataHandle& handle, const Shader::IlluminationModel illum);
		TextureHandle CreateTexture(const KtxDataHandle& handle);
		VertexBufferHandle CreateVertexBuffer(const std::vector<float>& data, const int32_t usage);
		MeshHandle CreateMesh(const MeshData_& data);
		ModelHandle CreateModel(const GltfDataHandle& handle, const std::vector<glm::mat4>& transforms);
		ModelHandle CreateModel(const std::vector<float>& data, const std::vector<uint32_t>& layout, const std::vector<glm::mat4>& transforms, const glm::vec3 color);

		void Schedule(const ModelHandle& model, const ShaderHandle& shader, const int32_t primitive);
	};
}//!sge
