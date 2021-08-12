#pragma once

#include <concepts>

#include "Model.h"
#include "Mesh.h"
#include "Shader.h"


#include "Hash.h"
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
			Handle<Model> model = {};
			Handle<Shader> shader = {};
			int32_t primitive = GL_TRIANGLES;
		};

		struct GltfMeshData_
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
			IndexType indexType = IndexType::INVALID;
		};

		std::vector<Resource<Shader>> shaders_ = {};
		std::vector<Resource<VertexBuffer>> vertexBuffers_ = {};
		std::vector<Resource<Texture>> textures_ = {};
		std::vector<Resource<Mesh>> meshes_ = {};
		std::vector<Resource<Model>> models_ = {};

		std::vector<DrawCall_> drawQueue_ = {};

		glm::mat4 viewMatrix_ = DEFAULT_VIEW_MATRIX;

		static std::vector<GltfMeshData_> ProcessGltf_(const GltfDataHandle& handle);
		static std::vector<glm::mat4> FrustumCulling_(const glm::ivec2 resolution, const float horizontalFullFov, const float nearPlane, const float farPlane, const float radius, const glm::mat4* const begin, const glm::mat4* const end);
		static void SortFrontToBack_(std::vector<glm::mat4>& transforms);
		static void SortBackToFront_(std::vector<glm::mat4>& transforms);

	public:
		sge_DISALLOW_COPY(Renderer);

		void Init();
		void Shutdown();
		void Update();

		Handle<Shader> CreateShader(const Handle<ShaderData>& handle, const Shader::IlluminationModel illum);
		Handle<Texture> CreateTexture(const KtxDataHandle& handle);
		Handle<VertexBuffer> CreateVertexBuffer(const VertexBuffer::Definition& def, const Hash& accumulatedHash);
		Handle<Mesh> CreateMesh(const MeshData_& data);
		Handle<Model> CreateModel(const std::vector<float>& data, const std::vector<uint32_t>& layout, const std::vector<glm::mat4>& transforms, const glm::vec3 color);

		Handle<Model> ModelFromGltf(const GltfDataHandle& handle, const std::vector<glm::mat4>& transforms);

		void Schedule(const Handle<Model>& model, const Handle<Shader>& shader);
	};
}//!sge
