#pragma once

#include "globals.h"

namespace sge
{
	struct Shader
	{
		enum IlluminationModel : uint32_t
		{
			INVALID = 0,
			GOOCH,
			ALBEDO_ONLY,
			BLINN_PHONG,
			BLINN_PHONG_NORMALMAPPED,
			GIZMO
		};

		uint32_t PROGRAM = 0;
		uint32_t primitive = GL_TRIANGLES;
		IlluminationModel illum = IlluminationModel::INVALID;
		std::map<Hash, uint32_t> uniformLocationCache = {};

		void Init(const std::string_view vertexSrc, const std::string_view fragmentSrc, const std::string_view geometrySrc, const IlluminationModel illum, const uint32_t primitive);
		void Destroy();

		int32_t GetUniformLocation(const std::string_view name);
		void SetInt(const std::string_view name, const int32_t value);
		void SetFloat(const std::string_view name, const float value);
		void SetVec3(const std::string_view name, const glm::vec3 value);
		void SetMat4(const std::string_view name, const glm::mat4& value);
		void SetMat3(const std::string_view name, const glm::mat3& value);

		inline bool IsValid() const
		{
			return PROGRAM && illum;
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge