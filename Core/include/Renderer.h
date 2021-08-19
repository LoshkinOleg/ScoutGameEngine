#pragma once

#include "Shader.h"
#include "ResourceManager.h"

namespace sge
{
	class Renderer
	{
		friend class Engine;
		sge_ALLOW_CONSTRUCTION(Renderer);

		constexpr static const int32_t CLEAR_FLAGS_ = 0x00004000 | 0x00000100;
		constexpr static const size_t VERTEX_BUFFER_POOL_SIZE_ = 64;
		constexpr static const size_t TEXTURES_POOL_SIZE_ = 32;
		constexpr static const size_t MATERIALS_POOL_SIZE_ = 32;
		constexpr static const size_t INDEXED_MESHES_POOL_SIZE_ = 64;
		constexpr static const size_t MODELS_POOL_SIZE_ = 8;

		struct DrawCall_
		{
			UniqueResourceHandle<Model> model = {};
			ShadingMode mode = {};
			int32_t primitive = 0;
		};

		// std::vector<Resource<Shader>> shaders_ = {};
		UniqueResource<Shader> gizmoShader_ = {};
		UniqueResource<Shader> goochShader_ = {};
		UniqueResource<Shader> albedoOnlyShader_ = {};
		UniqueResource<Shader> blinnPhongShader_ = {};
		UniqueResource<Shader> blinnPhongNormalmappedShader_ = {};
		// Resource<Shader> shadowPassShader_ = {};
		// Resource<Shader> deferredPassShader_ = {};
		// Resource<Shader> postprocessPassShader_ = {};

		std::vector<UniqueResource<VertexBuffer>> vertexBuffers_ = {};
		std::vector<UniqueResource<Texture>> textures_ = {};
		std::vector<UniqueResource<Material>> materials_ = {};
		std::vector<UniqueResource<IndexedMesh>> indexedMeshes_ = {};
		std::vector<UniqueResource<Model>> models_ = {};

		std::vector<DrawCall_> drawQueue_ = {};

		glm::mat4 viewMatrix_ = DEFAULT_VIEW_MATRIX;

		uint32_t modelMatricesVBO_ = 0;

		// TODO: implement this
		static std::vector<glm::mat4> FrustumCulling_(const glm::ivec2 resolution,
													  const float horizontalFullFov,
													  const float nearPlane,
													  const float farPlane);
		static void SortFrontToBack_(std::vector<glm::mat4>& transforms);
		static void SortBackToFront_(std::vector<glm::mat4>& transforms);

	public:
		sge_DISALLOW_COPY(Renderer);

		void Init();
		void Shutdown();
		void Update();

		UniqueResourceHandle<Shader> CreateShader(const UniqueResourceHandle<ShaderData>& handle);
		UniqueResourceHandle<VertexBuffer> CreateVertexBuffer(const VertexBuffer::Definition& def);
		UniqueResourceHandle<Texture> CreateTexture(const Texture::Definition& def);
		UniqueResourceHandle<Material> CreateMaterial(const Material::Definition& def);
		UniqueResourceHandle<IndexedMesh> CreateMesh(const IndexedMesh::Definition& def);
		UniqueResourceHandle<Model> CreateModel(const Model::Definition& def);

		UniqueResourceHandle<Shader> GetShaderForShadingMode(const ShadingMode mode);
		void BindModelMatricesVbo() const;

		void Schedule(const UniqueResourceHandle<Model>& model, const Primitive primitive, const ShadingMode mode);
	};
}//!sge
