#include "Renderer.h"

#include <glad/glad.h>

#include "Engine.h"

namespace sge
{
	void Renderer::Init()
	{
		glClearColor(DEFAULT_COLOR.r, DEFAULT_COLOR.g, DEFAULT_COLOR.b, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		const std::string GIZMO_SHADER_PATH[2] = { sge_SHADERS_PATH + "gizmo.vert", sge_SHADERS_PATH + "gizmo.frag"};
		const std::string GOOCH_SHADER_PATH[2] = { sge_SHADERS_PATH + "gooch.vert", sge_SHADERS_PATH + "gooch.frag"};
		const std::string ALBEDO_ONLY_SHADER_PATH[2] = { sge_SHADERS_PATH + "albedoOnly.vert", sge_SHADERS_PATH + "albedoOnly.frag"};
		const std::string BLINN_PHONG_SHADER_PATH[2] = { sge_SHADERS_PATH + "blinnPhong.vert", sge_SHADERS_PATH + "blinnPhong.frag"};
		const std::string BLINN_PHONG_NORMALMAPPED_SHADER_PATH[2] = { sge_SHADERS_PATH + "blinnPhongNormalmapped.vert", sge_SHADERS_PATH + "blinnPhongNormalmapped.frag"};
		// const std::string SHADOW_PASS_SHADER_PATH[2] = { sge_SHADERS_PATH + "shadowPass.vert", sge_SHADERS_PATH + "shadowPass.frag"};
		// const std::string DEFERRED_PASS_SHADER_PATH[2] = { sge_SHADERS_PATH + "deferredPass.vert", sge_SHADERS_PATH + "deferredPass.frag"};
		// const std::string POSTPROCESS_PASS_SHADER_PATH[2] = { sge_SHADERS_PATH + "postprocessPass.vert", sge_SHADERS_PATH + "postprocessPass.frag"};

		auto& rm = Engine::Get().GetResourceManager();
		auto gizmoSrcHandle = rm.LoadShader(GIZMO_SHADER_PATH[0], GIZMO_SHADER_PATH[1], "");
		auto goochSrcHandle = rm.LoadShader(GOOCH_SHADER_PATH[0], GOOCH_SHADER_PATH[1], "");
		auto albedoOnlySrcHandle = rm.LoadShader(ALBEDO_ONLY_SHADER_PATH[0], ALBEDO_ONLY_SHADER_PATH[1], "");
		auto blinnPhongSrcHandle = rm.LoadShader(BLINN_PHONG_SHADER_PATH[0], BLINN_PHONG_SHADER_PATH[1], "");
		auto blinnPhongNormalmappedSrcHandle = rm.LoadShader(BLINN_PHONG_NORMALMAPPED_SHADER_PATH[0], BLINN_PHONG_NORMALMAPPED_SHADER_PATH[1], "");
		// auto shadowPassSrcHandle = rm.LoadShader(SHADOW_PASS_SHADER_PATH[0], SHADOW_PASS_SHADER_PATH[1], "");
		// auto deferredPassSrcHandle = rm.LoadShader(DEFERRED_PASS_SHADER_PATH[0], DEFERRED_PASS_SHADER_PATH[1], "");
		// auto postprocessPassSrcHandle = rm.LoadShader(POSTPROCESS_PASS_SHADER_PATH[0], POSTPROCESS_PASS_SHADER_PATH[1], "");
		assert(gizmoSrcHandle.IsValid());
		assert(goochSrcHandle.IsValid());
		assert(albedoOnlySrcHandle.IsValid());
		assert(blinnPhongSrcHandle.IsValid());
		assert(blinnPhongNormalmappedSrcHandle.IsValid());
		// assert(deferredPassSrcHandle.IsValid());
		// assert(deferredPassSrcHandle.IsValid());
		// assert(postprocessPassSrcHandle.IsValid());

		gizmoShader_.hash = gizmoSrcHandle.hash;
		gizmoShader_.resourceData.Init_(gizmoSrcHandle->vertexCode, gizmoSrcHandle->fragmentCode, gizmoSrcHandle->geometryCode, ShadingMode::GIZMO);
		goochShader_.hash = goochSrcHandle.hash;
		goochShader_.resourceData.Init_(goochSrcHandle->vertexCode, goochSrcHandle->fragmentCode, goochSrcHandle->geometryCode, ShadingMode::GOOCH);
		albedoOnlyShader_.hash = albedoOnlySrcHandle.hash;
		albedoOnlyShader_.resourceData.Init_(albedoOnlySrcHandle->vertexCode, albedoOnlySrcHandle->fragmentCode, albedoOnlySrcHandle->geometryCode, ShadingMode::ALBEDO_ONLY);
		blinnPhongShader_.hash = blinnPhongSrcHandle.hash;
		blinnPhongShader_.resourceData.Init_(blinnPhongSrcHandle->vertexCode, blinnPhongSrcHandle->fragmentCode, blinnPhongSrcHandle->geometryCode, ShadingMode::BLINN_PHONG);
		blinnPhongNormalmappedShader_.hash = blinnPhongNormalmappedSrcHandle.hash;
		blinnPhongNormalmappedShader_.resourceData.Init_(blinnPhongNormalmappedSrcHandle->vertexCode, blinnPhongNormalmappedSrcHandle->fragmentCode, blinnPhongNormalmappedSrcHandle->geometryCode, ShadingMode::BLINN_PHONG_NORMALMAPPED);
		// shadowPassShader_.hash = shadowPassSrcHandle.hash;
		// shadowPassShader_.resourceData.Init_(shadowPassSrcHandle->vertexCode, shadowPassSrcHandle->fragmentCode, shadowPassSrcHandle->geometryCode, ShadingMode::SHADOW_PASS);
		// deferredPassShader_.hash = deferredPassSrcHandle.hash;
		// deferredPassShader_.resourceData.Init_(deferredPassSrcHandle->vertexCode, deferredPassSrcHandle->fragmentCode, deferredPassSrcHandle->geometryCode, ShadingMode::DEFERRED_PASS);
		// postprocessPassShader_.hash = postprocessPassSrcHandle.hash;
		// postprocessPassShader_.resourceData.Init_(postprocessPassSrcHandle->vertexCode, postprocessPassSrcHandle->fragmentCode, postprocessPassSrcHandle->geometryCode, ShadingMode::POST_PROCESS_PASS);

		uniqueVertexBuffers_.reserve(STATIC_VERTEX_BUFFER_POOL_SIZE_);
		dynamicVertexBuffers_.reserve(DYNAMIC_VERTEX_BUFFER_POOL_SIZE_);
		textures_.reserve(TEXTURES_POOL_SIZE_);
		materials_.reserve(MATERIALS_POOL_SIZE_);
		indexedMeshes_.reserve(INDEXED_MESHES_POOL_SIZE_);
		models_.reserve(MODELS_POOL_SIZE_);

		// Create buffer for all model matrices.
		glGenBuffers(1, &modelMatricesVBO_);
		glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(rm.GetMaxNrOfModelMatrices() * sizeof(glm::mat4)), rm.GetModelMatricesBegin(), GL_DYNAMIC_DRAW);
	}
	void Renderer::Shutdown()
	{
		for(auto& vbResource : uniqueVertexBuffers_)
		{
			auto& vb = vbResource.resourceData;
			vb.Destroy_();
		}
		for(auto& texResource : textures_)
		{
			auto& tex = texResource.resourceData;
			tex.Destroy_();
		}
		// for(auto& shaderResource : shaders_)
		// {
		// 	auto& shader = shaderResource.resourceData;
		// 	shader.Destroy_();
		// }
		gizmoShader_.resourceData.Destroy_();
		goochShader_.resourceData.Destroy_();
		albedoOnlyShader_.resourceData.Destroy_();
		blinnPhongShader_.resourceData.Destroy_();
		blinnPhongNormalmappedShader_.resourceData.Destroy_();
		// shadowPassShader_.resourceData.Destroy_();
		// deferredPassShader_.resourceData.Destroy_();
		// postprocessPassShader_.resourceData.Destroy_();

		glDeleteBuffers(1, &modelMatricesVBO_);

		models_.clear();
		indexedMeshes_.clear();
		uniqueVertexBuffers_.clear();
		dynamicVertexBuffers_.clear();
		textures_.clear();
		materials_.clear();
		// shaders_.clear();
		drawQueue_.clear();
	}
	void Renderer::Update()
	{
		glClear(CLEAR_FLAGS_);

		// Upload transforms to gpu.
		auto& rm = Engine::Get().GetResourceManager();
		glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(rm.GetMaxNrOfModelMatrices() * sizeof(glm::mat4)), rm.GetModelMatricesBegin());

		// TODO: update view matrix using player input.

		// TODO: separate draw calls into two queues based on whether the mesh has transparency or not.

		gizmoShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::GIZMO);
		goochShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::GOOCH);
		albedoOnlyShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::ALBEDO_ONLY);
		blinnPhongShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::BLINN_PHONG);
		blinnPhongNormalmappedShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::BLINN_PHONG_NORMALMAPPED);
		// shadowPassShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::SHADOW_PASS);
		// deferredPassShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::DEFERRED_PASS);
		// postprocessPassShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::POST_PROCESS_PASS);

		const uint32_t len = (uint32_t)drawQueue_.size();
		for(uint32_t i = 0; i < len; i++)
		{
			const DrawCall_& drawCall = drawQueue_[i];

			// TODO: add frustum culling.
			drawCall.model->Draw_(drawCall.primitive, drawCall.mode);
		}

		// TODO: draw transparent objects.

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		drawQueue_.clear();
	}

	UniqueResourceHandle<Shader> Renderer::CreateShader(const UniqueResourceHandle<ShaderData>& handle)
	{
		sge_ERROR("You're not supposed to create your own shaders just yet.");
		return {};
	}

	UniqueResourceHandle<VertexBuffer> Renderer::CreateStaticVertexBuffer(const VertexBuffer::Definition & def)
	{
		assert(def.mutability == Mutability::STATIC);
		assert(uniqueVertexBuffers_.size() + 1 < STATIC_VERTEX_BUFFER_POOL_SIZE_);

		uniqueVertexBuffers_.push_back(UniqueResource<VertexBuffer>());
		auto& newElement = uniqueVertexBuffers_.back();
		auto& newValue = newElement.resourceData;
		newElement.hash = Hash(def.begin, def.byteLen, 0);
		newValue.Init_(def);
		UniqueResourceHandle<VertexBuffer> handle;
		handle.hash = newElement.hash;
		handle.ptr = &uniqueVertexBuffers_.back();
		assert(newElement.IsValid());
		assert(newValue.IsValid());
		assert(handle.IsValid());
		return handle;
	}

	HashlessHandle<VertexBuffer> Renderer::CreateDynamicVertexBuffer(const VertexBuffer::Definition & def)
	{
		assert(def.mutability == Mutability::DYNAMIC);
		assert(dynamicVertexBuffers_.size() + 1 < DYNAMIC_VERTEX_BUFFER_POOL_SIZE_);

		HashlessResource<VertexBuffer> newElement;
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		assert(newValue.IsValid());
		assert(newElement.IsValid());
		dynamicVertexBuffers_.push_back(newElement);

		HashlessHandle<VertexBuffer> handle;
		handle.ptr = &dynamicVertexBuffers_.back();
		assert(handle.IsValid());

		return handle;
	}

	UniqueResourceHandle<Texture> Renderer::CreateTexture(const Texture::Definition & def)
	{
		assert(textures_.size() + 1 < TEXTURES_POOL_SIZE_);

		textures_.push_back(UniqueResource<Texture>());
		auto& newElement = textures_.back();
		auto& newValue = newElement.resourceData;
		newElement.hash = Hash(def.datas[0], def.byteLens[0], 0);
		newValue.Init_(def);
		UniqueResourceHandle<Texture> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	UniqueResourceHandle<Material> Renderer::CreateMaterial(const Material::Definition & def)
	{
		assert(materials_.size() + 1 < MATERIALS_POOL_SIZE_);

		materials_.push_back(UniqueResource<Material>());
		auto& newElement = materials_.back();
		auto& newValue = newElement.resourceData;
		newElement.hash = def.ComputeHash();
		newValue.Init_(def);
		UniqueResourceHandle<Material> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	UniqueResourceHandle<IndexedMesh> Renderer::CreateIndexedMesh(const IndexedMesh::Definition & def)
	{
		assert(indexedMeshes_.size() + 1 < INDEXED_MESHES_POOL_SIZE_);

		indexedMeshes_.push_back(UniqueResource<IndexedMesh>());
		auto& newElement = indexedMeshes_.back();
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		newElement.hash = def.ComputeHash();
		UniqueResourceHandle<IndexedMesh> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	UniqueResourceHandle<StaticModel> Renderer::CreateModel(const StaticModel::Definition & def)
	{
		assert(models_.size() + 1 < MODELS_POOL_SIZE_);

		models_.push_back(UniqueResource<StaticModel>());
		auto& newElement = models_.back();
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		newElement.hash = def.ComputeHash();
		UniqueResourceHandle<StaticModel> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	UniqueResourceHandle<Shader> Renderer::GetShaderForShadingMode(const ShadingMode mode)
	{
		UniqueResourceHandle<Shader> handle;
		switch(mode)
		{
			case ShadingMode::GIZMO:
			{
				handle.hash = gizmoShader_.hash;
				handle.ptr = &gizmoShader_;
				sge_ERROR("Shader not yet implemented!");
			}break;
			case ShadingMode::GOOCH:
			{
				handle.hash = goochShader_.hash;
				handle.ptr = &goochShader_;
			}break;
			case ShadingMode::ALBEDO_ONLY:
			{
				handle.hash = albedoOnlyShader_.hash;
				handle.ptr = &albedoOnlyShader_;
			}break;
			case ShadingMode::BLINN_PHONG:
			{
				handle.hash = blinnPhongShader_.hash;
				handle.ptr = &blinnPhongShader_;
			}break;
			case ShadingMode::BLINN_PHONG_NORMALMAPPED:
			{
				handle.hash = blinnPhongNormalmappedShader_.hash;
				handle.ptr = &blinnPhongNormalmappedShader_;
			}break;
			case ShadingMode::SHADOW_PASS:
			{
				// handle.hash = shadowPassShader_.hash;
				// handle.ptr = &shadowPassShader_;
				sge_ERROR("Shader not yet implemented!");
			}break;
			case ShadingMode::DEFERRED_PASS:
			{
				// handle.hash = deferredPassShader_.hash;
				// handle.ptr = &deferredPassShader_;
				sge_ERROR("Shader not yet implemented!");
			}break;
			case ShadingMode::POST_PROCESS_PASS:
			{
				// handle.hash = postprocessPassShader_.hash;
				// handle.ptr = &postprocessPassShader_;
				sge_ERROR("Shader not yet implemented!");
			}break;
			default:
			{
				sge_ERROR("Unexpected shading mode passed!");
			}break;
		}
		assert(handle.IsValid());
		return handle;
	}

	void Renderer::BindModelMatricesVbo() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
	}

	void Renderer::Schedule(const UniqueResourceHandle<StaticModel>& model, const Primitive primitive, const ShadingMode mode)
	{
		assert(model->IsValid() && (uint32_t)mode);
		drawQueue_.push_back(DrawCall_());
		auto& drawCall = drawQueue_.back();
		drawCall.model = model;
		drawCall.primitive = (int32_t)primitive;
		drawCall.mode = mode;
	}
}//!sge