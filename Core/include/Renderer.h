#pragma once

#include "ResourcesAbstracts.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
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

		std::vector<Resource<Shader>> shaders_ = {};
		std::vector<Resource<VertexBuffer>> vertexBuffers_ = {};
		std::vector<Resource<Texture>> textures_ = {};
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
		Handle<IndexedMesh> CreateMesh(void* data);
		Handle<Model> CreateModel(void* data);

		Handle<Shader> GetShaderForShadingMode(const ShadingMode& mode);

		void Schedule(const Handle<Model>& model, const Handle<Shader>& shader);
	};
}//!sge
