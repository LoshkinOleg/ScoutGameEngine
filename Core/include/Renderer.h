#pragma once

#include "Model.h"

namespace sge
{
	class Renderer
	{
		friend class Engine;
		sge_ALLOW_CONSTRUCTION(Renderer);

		constexpr static const int32_t CLEAR_FLAGS_ = 0x00004000 | 0x00000100;

		struct DrawCall_
		{
			Handle<Model> model = {};
			ShadingMode mode = {};
			int32_t primitive = GL_TRIANGLES; // 0 is GL_POINTS so might as well give it a default value.
		};

		// std::vector<Resource<Shader>> shaders_ = {};
		Resource<Shader> gizmoShader_ = {};
		Resource<Shader> goochShader_ = {};
		Resource<Shader> albedoOnlyShader_ = {};
		Resource<Shader> blinnPhongShader_ = {};
		Resource<Shader> blinnPhongNormalmappedShader_ = {};
		Resource<Shader> shadowPassShader_ = {};
		Resource<Shader> deferredPassShader_ = {};
		Resource<Shader> postprocessPassShader_ = {};

		std::vector<Resource<VertexBuffer>> vertexBuffers_ = {};
		std::vector<Resource<Texture>> textures_ = {};
		std::vector<Resource<Material>> materials_ = {};
		std::vector<Resource<IndexedMesh>> indexedMeshes_ = {};
		std::vector<Resource<Model>> models_ = {};

		std::vector<DrawCall_> drawQueue_ = {};

		glm::mat4 viewMatrix_ = DEFAULT_VIEW_MATRIX;

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

		Handle<Shader> CreateShader(const Handle<ShaderData>& handle);
		Handle<VertexBuffer> CreateVertexBuffer(const VertexBuffer::Definition& def);
		Handle<Texture> CreateTexture(const Texture::Definition& def);
		Handle<Material> CreateMaterial(const Material::Definition& def);
		Handle<IndexedMesh> CreateMesh(const IndexedMesh::Definition& def);
		Handle<Model> CreateModel(const Model::Definition& def);

		Handle<Shader> GetShaderForShadingMode(const ShadingMode mode);

		void Schedule(const Handle<Model>& model, const uint32_t primitive, const ShadingMode mode);
	};
}//!sge
