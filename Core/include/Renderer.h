#pragma once

#include "EnumsAndFlags.h"
#include "Model.h"
#include "Shader.h"

namespace sge
{
	class Renderer
	{
		friend class Engine;
		constexpr static const int32_t CLEAR_FLAGS_ = 0x4000 | 0x0100; // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
		constexpr static const size_t STATIC_VERTEX_BUFFER_POOL_SIZE_ = 64;
		constexpr static const size_t DYNAMIC_VERTEX_BUFFER_POOL_SIZE_ = 8;
		constexpr static const size_t TEXTURES_POOL_SIZE_ = 32;
		constexpr static const size_t MATERIALS_POOL_SIZE_ = 32;
		constexpr static const size_t INDEXED_MESHES_POOL_SIZE_ = 64;
		constexpr static const size_t INTERLACED_MESHES_POOL_SIZE_ = 8;
		constexpr static const size_t MODELS_POOL_SIZE_ = 8;
		constexpr static const size_t DRAW_QUEUE_SIZE_ = 16;

		class DrawCall_
		{
		public:
			UniqueResourceHandle<Model> model = {};
			ShadingMode mode = ShadingMode::INVALID;
			DrawingPrimitive primitive = DrawingPrimitive::POINTS;
		};

	public:
		UniqueResourceHandle<Shader> CreateShader(const UniqueResourceHandle<ShaderData>& handle);
		UniqueResourceHandle<VertexBuffer> CreateStaticVertexBuffer(const VertexBuffer::Definition& def);
		HashlessHandle<VertexBuffer> CreateDynamicVertexBuffer(const VertexBuffer::Definition & def);
		UniqueResourceHandle<Texture> CreateTexture(const Texture::Definition& def);
		UniqueResourceHandle<Material> CreateMaterial(const Material::Definition& def);
		UniqueResourceHandle<IndexedMesh> CreateIndexedMesh(const IndexedMesh::Definition& def);
		UniqueResourceHandle<InterlacedMesh> CreateInterlacedMesh(const InterlacedMesh::Definition& def);
		UniqueResourceHandle<Model> CreateModel(const Model::Definition& def);

		UniqueResourceHandle<Shader> GetShaderFor(const ShadingMode mode);

		void Schedule(const UniqueResourceHandle<Model>& model, const DrawingPrimitive primitive, const ShadingMode mode);

	private:
		std::vector<UniqueResource<VertexBuffer>> staticVertexBuffers_ = {};
		std::vector<HashlessResource<VertexBuffer>> dynamicVertexBuffers_ = {};
		std::vector<UniqueResource<Texture>> textures_ = {};
		std::vector<UniqueResource<Material>> materials_ = {};
		std::vector<UniqueResource<IndexedMesh>> indexedMeshes_ = {};
		std::vector<UniqueResource<InterlacedMesh>> interlacedMeshes = {};
		std::vector<UniqueResource<Model>> models_ = {};

		UniqueResource<Shader> goochShader_ = {};
		std::vector<DrawCall_> drawQueue_ = {};
		VertexBuffer modelMatricesVbo_ = {};

		void Init_();
		void Update_();
		void Shutdown_();

		// TODO: implement gl context tracking to avoid uneccessary binding calls.
		// uint32_t boundVao_ = 0, boundVbo_ = 0, boundProgram_ = 0, boundFramebuffer_ = 0;
		// void BindVao(const uint32_t VAO) const;
		// void BindVbo(const uint32_t VBO) const;
		// void BindProgram(const uint32_t PROGRAM) const;
		// void BindFramebuffer(const uint32_t FBO) const;

		// TODO: implement frustum culling.
		// static std::vector<glm::mat4> FrustumCulling_(const glm::ivec2 resolution,
		// 											  const float horizontalFullFov,
		// 											  const float nearPlane,
		// 											  const float farPlane);
		
		// TODO: implement blending.
		// static void SortFrontToBack_(std::vector<glm::mat4>& transforms);
		// static void SortBackToFront_(std::vector<glm::mat4>& transforms);
		
		// TODO: implement gizmos.
		// UniqueResource<Shader> gizmoShader_ = {};
		
		// TODO: implement non compressed and compressed textures handling.
		// UniqueResource<Shader> albedoOnlyShader_ = {};
		// UniqueResource<Shader> blinnPhongShader_ = {};
		// UniqueResource<Shader> blinnPhongNormalmappedShader_ = {};

		// TODO: implement deferred rendering and shadow pass.
		// DoubleBuffer pingPongBuffers_ = {};
		// UniqueResource<Shader> shadowPassShader_ = {};
		// UniqueResource<Shader> deferredPassShader_ = {};
		// UniqueResource<Shader> postprocessPassShader_ = {};
		// class DoubleBuffer
		// {
		// public:
		// 	Framebuffer& Ping();
		// 	Framebuffer& Pong();
		// 
		// private:
		// 	Framebuffer ping_ = {};
		// 	Framebuffer pong_ = {};
		// 	bool current_ = 0;
		// };

		// TODO: implement skybox.
		// Skybox skybox_ = {};
	};
}//!sge
