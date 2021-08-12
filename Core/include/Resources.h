#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <type_traits>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <gli/gli.hpp>
#include <tiny_gltf.h>

#include "globals.h"
#include "macros.h"

namespace sge
{
	

	struct JsonData : public A_Resource
	{
		json data = json::value_t::discarded;

		void Destroy() override;
	};

	struct JsonDataHandle : public A_Handle
	{
		JsonData* operator->() const override;
		JsonData& operator*() const override;
	};

	struct KtxData : public A_Resource
	{
		gli::texture data = {};

		void Destroy() override;
	};

	struct KtxDataHandle : public A_Handle
	{
		KtxData* operator->() const override;
		KtxData& operator*() const override;
	};

	struct GltfData : public A_Resource
	{
		tinygltf::Model model = {};
		KtxDataHandle alphaMap = {};
		KtxDataHandle albedoMap = {};
		KtxDataHandle specularMap = {};
		KtxDataHandle normalMap = {};

		void Destroy() override;
	};

	struct GltfDataHandle : public A_Handle
	{
		GltfData* operator->() const override;
		GltfData& operator*() const override;
	};

	struct ShaderData : public A_Resource
	{
		std::string vertexCode = "";
		std::string fragmentCode = "";

		void Destroy() override;
	};

	struct ShaderDataHandle : public A_Handle
	{
		ShaderData* operator->() const override;
		ShaderData& operator*() const override;
	};

	

	

	
	struct BlinnPhongModel : public A_Resource
	{
		std::vector<BlinnPhongMeshHandle> meshes = {};
		glm::mat4* transformsBegin = nullptr;
		glm::mat4* transformsEnd = nullptr;
		VertexBufferHandle transformsVertexBuffer = {};
		float radius = 0.0f;

		void Destroy() override;
	};

	struct StaticSingleBufferModel: public A_Resource
	{
		BlinnPhongMeshHandle mesh = {};
		glm::mat4* transformsBegin = nullptr;
		glm::mat4* transformsEnd = nullptr;
		VertexBufferHandle transformsVertexBuffer = {};
		float radius = 0.0f;

		void Destroy() override;
	};


}//!sge