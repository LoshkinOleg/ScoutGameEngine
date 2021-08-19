#include "Material.h"

#include "Engine.h"

namespace sge
{
	bool Material::IsValid() const
	{
		bool returnVal = (bool)shadingMode;
		switch(shadingMode)
		{
			case ShadingMode::GIZMO:
			{
				returnVal = returnVal && textures.size() == 0;
				returnVal = returnVal && vec3s.size() == 1;
				returnVal = returnVal && floats.size() == 0;
			}break;
			case ShadingMode::GOOCH:
			{
				returnVal = returnVal && textures.size() == 0;
				returnVal = returnVal && vec3s.size() == 1;
				returnVal = returnVal && floats.size() == 0;
			}break;
			case ShadingMode::ALBEDO_ONLY:
			{
				returnVal = returnVal && textures.size() == 1;
				returnVal = returnVal && vec3s.size() == 0;
				returnVal = returnVal && floats.size() == 0;
			}break;
			case ShadingMode::BLINN_PHONG:
			{
				returnVal = returnVal && textures.size() == 2;
				returnVal = returnVal && vec3s.size() == 0;
				returnVal = returnVal && floats.size() == 1;
			}break;
			case ShadingMode::BLINN_PHONG_NORMALMAPPED:
			{
				returnVal = returnVal && textures.size() == 3;
				returnVal = returnVal && vec3s.size() == 0;
				returnVal = returnVal && floats.size() == 1;
			}break;
			default:
			{
				returnVal = false;
			}break;
		}
		return returnVal;
	}

	void Material::Init_(const Definition& def)
	{
		shadingMode = def.shadingMode;
		vec3s = def.vec3s;
		floats = def.floats;
		auto& rm = Engine::Get().GetResourceManager();
		auto& renderer = Engine::Get().GetRenderer();

		for(const auto& texDef : def.texDefs)
		{
			textures.push_back(renderer.CreateTexture(texDef));
		}

		UniqueResourceHandle<Shader> shaderHandle = renderer.GetShaderForShadingMode(def.shadingMode);
		switch(def.shadingMode)
		{
			case ShadingMode::GIZMO:
			{
				// auto shaderDataHandle = rm.LoadShader(sge_SHADERS_PATH + "gizmos.vert", sge_SHADERS_PATH + "gizmos.frag", "");
				// auto shaderHandle = renderer.GetShaderForShadingMode(def.shadingMode);
				assert(vec3s.size() == 1);
				shaderHandle->SetVec3("color", vec3s[0]);
			}break;
			case ShadingMode::GOOCH:
			{
				// auto shaderDataHandle = rm.LoadShader(sge_SHADERS_PATH + "gooch.vert", sge_SHADERS_PATH + "gooch.frag", "");
				// auto shaderHandle = renderer.GetShaderForShadingMode(def.shadingMode);
				assert(vec3s.size() == 1);
				shaderHandle->SetVec3("color", vec3s[0]);
			}break;
			case ShadingMode::ALBEDO_ONLY:
			{
				// auto shaderDataHandle = rm.LoadShader(sge_SHADERS_PATH + "albedoOnly.vert", sge_SHADERS_PATH + "albedoOnly.frag", "");
				// auto shaderHandle = renderer.GetShaderForShadingMode(def.shadingMode);
				assert(textures.size() == 1);
				shaderHandle->SetInt("albedoMap", 0);
			}break;
			case ShadingMode::BLINN_PHONG:
			{
				// auto shaderDataHandle = rm.LoadShader(sge_SHADERS_PATH + "blinnPhong.vert", sge_SHADERS_PATH + "blinnPhong.frag", "");
				// auto shaderHandle = renderer.GetShaderForShadingMode(def.shadingMode);
				assert(textures.size() == 2 && floats.size() == 1);
				shaderHandle->SetInt("albedoMap", 0);
				shaderHandle->SetInt("specularMap", 1);
				shaderHandle->SetFloat("shininess", floats[0]);
			}break;
			case ShadingMode::BLINN_PHONG_NORMALMAPPED:
			{
				// auto shaderDataHandle = rm.LoadShader(sge_SHADERS_PATH + "blinnPhongNormalmapped.vert", sge_SHADERS_PATH + "blinnPhongNormalmapped.frag", "");
				// auto shaderHandle = renderer.GetShaderForShadingMode(def.shadingMode);
				assert(textures.size() == 3 && floats.size() == 1);
				shaderHandle->SetInt("albedoMap", 0);
				shaderHandle->SetInt("specularMap", 1);
				shaderHandle->SetInt("normalMap", 2);
				shaderHandle->SetFloat("shininess", floats[0]);
			}break;
			default:
			{
				sge_ERROR("Invalid IllumMode passed to Material Definition!");
			}break;
		}
		assert(IsValid());
	}

	bool Material::Definition::IsValid() const
	{
		bool returnVal = (bool)shadingMode;
		for(const auto& def : texDefs)
		{
			returnVal &= def.IsValid();
		}
		return returnVal;
	}
	Hash Material::Definition::ComputeHash() const
	{
		Hash hash = 0;
		for(const auto& element : texDefs)
		{
			hash.Accumulate(element.preComputedHash);
		}
		hash.Accumulate(vec3s.data(), (uint32_t)(sizeof(glm::vec3) * vec3s.size()));
		hash.Accumulate(floats.data(), (uint32_t)(sizeof(float) * floats.size()));
		hash.Accumulate(&shadingMode, (uint32_t)(sizeof(ShadingMode)));
		return hash;
	}
	void Material::Bind() const
	{
		assert(IsValid());
		const uint32_t len = (uint32_t)textures.size();
		for(uint32_t i = 0; i < len; i++)
		{
			textures[i]->Bind(i);
		}
		auto shaderHandle = Engine::Get().GetRenderer().GetShaderForShadingMode(shadingMode);
		switch(shadingMode)
		{
			case ShadingMode::GIZMO:
			{
				shaderHandle->SetVec3("color", vec3s[0]);
			}break;
			case ShadingMode::GOOCH:
			{
				shaderHandle->SetVec3("color", vec3s[0]);
			}break;
			case ShadingMode::ALBEDO_ONLY: break;
			case ShadingMode::BLINN_PHONG:
			{
				shaderHandle->SetFloat("shininess", floats[0]);
			}break;
			case ShadingMode::BLINN_PHONG_NORMALMAPPED:
			{
				shaderHandle->SetFloat("shininess", floats[0]);
			}break;
			default:
			{
				sge_ERROR("Invalid IllumMode in Material!");
			}break;
		}
	}
}//!sge