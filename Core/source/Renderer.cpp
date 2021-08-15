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
		const std::string ALBEDO_ONLY_SHADER_PATH[2] = { sge_SHADERS_PATH + "albedyOnly.vert", sge_SHADERS_PATH + "albedyOnly.frag"};
		const std::string BLINN_PHONG_SHADER_PATH[2] = { sge_SHADERS_PATH + "blinnPhong.vert", sge_SHADERS_PATH + "blinnPhong.frag"};
		const std::string BLINN_PHONG_NORMALMAPPED_SHADER_PATH[2] = { sge_SHADERS_PATH + "blinnPhongNormalmapped.vert", sge_SHADERS_PATH + "blinnPhongNormalmapped.frag"};
		const std::string SHADOW_PASS_SHADER_PATH[2] = { sge_SHADERS_PATH + "shadowPass.vert", sge_SHADERS_PATH + "shadowPass.frag"};
		const std::string DEFERRED_PASS_SHADER_PATH[2] = { sge_SHADERS_PATH + "deferredPass.vert", sge_SHADERS_PATH + "deferredPass.frag"};
		const std::string POSTPROCESS_PASS_SHADER_PATH[2] = { sge_SHADERS_PATH + "postprocessPass.vert", sge_SHADERS_PATH + "postprocessPass.frag"};

		auto& rm = Engine::Get().GetResourceManager();
		auto gizmoSrcHandle = rm.LoadShader(GIZMO_SHADER_PATH[0], GIZMO_SHADER_PATH[1], "");
		auto goochSrcHandle = rm.LoadShader(GOOCH_SHADER_PATH[0], GOOCH_SHADER_PATH[1], "");
		auto albedoOnlySrcHandle = rm.LoadShader(ALBEDO_ONLY_SHADER_PATH[0], ALBEDO_ONLY_SHADER_PATH[1], "");
		auto blinnPhongSrcHandle = rm.LoadShader(BLINN_PHONG_SHADER_PATH[0], BLINN_PHONG_SHADER_PATH[1], "");
		auto blinnPhongNormalmappedSrcHandle = rm.LoadShader(BLINN_PHONG_NORMALMAPPED_SHADER_PATH[0], BLINN_PHONG_NORMALMAPPED_SHADER_PATH[1], "");
		auto shadowPassSrcHandle = rm.LoadShader(SHADOW_PASS_SHADER_PATH[0], SHADOW_PASS_SHADER_PATH[1], "");
		auto deferredPassSrcHandle = rm.LoadShader(DEFERRED_PASS_SHADER_PATH[0], DEFERRED_PASS_SHADER_PATH[1], "");
		auto postprocessPassSrcHandle = rm.LoadShader(POSTPROCESS_PASS_SHADER_PATH[0], POSTPROCESS_PASS_SHADER_PATH[1], "");

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
		shadowPassShader_.hash = shadowPassSrcHandle.hash;
		shadowPassShader_.resourceData.Init_(shadowPassSrcHandle->vertexCode, shadowPassSrcHandle->fragmentCode, shadowPassSrcHandle->geometryCode, ShadingMode::SHADOW_PASS);
		deferredPassShader_.hash = deferredPassSrcHandle.hash;
		deferredPassShader_.resourceData.Init_(deferredPassSrcHandle->vertexCode, deferredPassSrcHandle->fragmentCode, deferredPassSrcHandle->geometryCode, ShadingMode::DEFERRED_PASS);
		postprocessPassShader_.hash = postprocessPassSrcHandle.hash;
		postprocessPassShader_.resourceData.Init_(postprocessPassSrcHandle->vertexCode, postprocessPassSrcHandle->fragmentCode, postprocessPassSrcHandle->geometryCode, ShadingMode::POST_PROCESS_PASS);
	}
	void Renderer::Shutdown()
	{
		for(auto& vbResource : vertexBuffers_)
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
		shadowPassShader_.resourceData.Destroy_();
		deferredPassShader_.resourceData.Destroy_();
		postprocessPassShader_.resourceData.Destroy_();

		models_.clear();
		indexedMeshes_.clear();
		vertexBuffers_.clear();
		textures_.clear();
		materials_.clear();
		// shaders_.clear();
		drawQueue_.clear();
	}
	void Renderer::Update()
	{
		glClear(CLEAR_FLAGS_);

		// TODO: update view matrix using player input.

		// TODO: separate draw calls into two queues based on whether the mesh has transparency or not.

		gizmoShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::GIZMO);
		goochShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::GOOCH);
		albedoOnlyShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::ALBEDO_ONLY);
		blinnPhongShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::BLINN_PHONG);
		blinnPhongNormalmappedShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::BLINN_PHONG_NORMALMAPPED);
		shadowPassShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::SHADOW_PASS);
		deferredPassShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::DEFERRED_PASS);
		postprocessPassShader_.resourceData.UpdatePerFrameUniforms_(viewMatrix_, ShadingMode::POST_PROCESS_PASS);

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

	Handle<Shader> Renderer::CreateShader(const Handle<ShaderData>& handle)
	{
		sge_ERROR("You're not supposed to create your own shaders just yet.");
	}

	Handle<VertexBuffer> Renderer::CreateVertexBuffer(const VertexBuffer::Definition & def)
	{
		vertexBuffers_.push_back(Resource<VertexBuffer>());
		auto& newElement = vertexBuffers_.front();
		auto& newValue = newElement.resourceData;
		newElement.hash = Hash(def.begin, def.byteLen, 0);
		newValue.Init_(def);
		Handle<VertexBuffer> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<Texture> Renderer::CreateTexture(const Texture::Definition & def)
	{
		textures_.push_back(Resource<Texture>());
		auto& newElement = textures_.front();
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		newElement.hash = Hash(def.datas[0], def.ByteSize(0), 0);
		Handle<Texture> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<Material> Renderer::CreateMaterial(const Material::Definition & def)
	{
		materials_.push_back(Resource<Material>());
		auto& newElement = materials_.front();
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		newElement.hash = def.ComputeHash();
		Handle<Material> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<IndexedMesh> Renderer::CreateMesh(const IndexedMesh::Definition & def)
	{
		indexedMeshes_.push_back(Resource<IndexedMesh>());
		auto& newElement = indexedMeshes_.front();
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		newElement.hash = def.ComputeHash();
		Handle<IndexedMesh> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<Model> Renderer::CreateModel(const Model::Definition & def)
	{
		models_.push_back(Resource<Model>());
		auto& newElement = models_.front();
		auto& newValue = newElement.resourceData;
		newValue.Init_(def);
		newElement.hash = def.ComputeHash();
		Handle<Model> handle;
		handle.hash = newElement.hash;
		handle.ptr = &newElement;
		assert(handle.IsValid());
		return handle;
	}

	Handle<Shader> Renderer::GetShaderForShadingMode(const ShadingMode mode)
	{
		Handle<Shader> handle;
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
				handle.hash = shadowPassShader_.hash;
				handle.ptr = &shadowPassShader_;
				sge_ERROR("Shader not yet implemented!");
			}break;
			case ShadingMode::DEFERRED_PASS:
			{
				handle.hash = deferredPassShader_.hash;
				handle.ptr = &deferredPassShader_;
				sge_ERROR("Shader not yet implemented!");
			}break;
			case ShadingMode::POST_PROCESS_PASS:
			{
				handle.hash = postprocessPassShader_.hash;
				handle.ptr = &postprocessPassShader_;
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

	void Renderer::Schedule(const Handle<Model>& model, const Primitive primitive, const ShadingMode mode)
	{
		assert(model->IsValid() && (uint32_t)mode);
		drawQueue_.push_back(DrawCall_());
		auto& drawCall = drawQueue_.front();
		drawCall.model = model;
		drawCall.primitive = (int32_t)primitive;
		drawCall.mode = mode;
	}
}//!sge