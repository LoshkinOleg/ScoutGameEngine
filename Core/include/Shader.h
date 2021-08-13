#pragma once

#include <string_view>

#include "Hash.h"
#include "globals.h"

namespace sge
{
	struct Shader
	{
		uint32_t PROGRAM = 0;
		uint32_t primitive = GL_TRIANGLES; // Note: 0 is GL_POINTS anyways so might as well set a default value.
		IllumMode illum = IllumMode::INVALID;
		std::map<Hash, uint32_t> uniformLocationCache = {};

		void Init(const std::string_view vertexSrc, const std::string_view fragmentSrc, const std::string_view geometrySrc, const IllumMode illum, const uint32_t primitive);
		void Destroy();

		void Bind() const;

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