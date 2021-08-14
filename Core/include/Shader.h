#pragma once

#include <string_view>

#include "Hash.h"
#include "globals.h"
#include "macros.h"

namespace sge
{
	struct Shader
	{
		uint32_t PROGRAM = 0;
		uint32_t primitive = GL_TRIANGLES; // Note: 0 is GL_POINTS anyways so might as well set a default value.
		std::map<Hash, uint32_t> uniformLocationCache = {};
		ShadingMode shadingMode = ShadingMode::INVALID;

		void Bind() const;

		int32_t GetUniformLocation(const std::string_view name);
		void SetInt(const std::string_view name, const int32_t value);
		void SetFloat(const std::string_view name, const float value);
		void SetVec3(const std::string_view name, const glm::vec3 value);
		void SetMat4(const std::string_view name, const glm::mat4& value);
		void SetMat3(const std::string_view name, const glm::mat3& value);

		inline bool IsValid() const
		{
			return PROGRAM && shadingMode;
		}
		inline void Reset()
		{
			*this = {};
		}

	private:
		friend class Renderer;
		void Init_(const std::string_view vertexSrc, const std::string_view fragmentSrc, const std::string_view geometrySrc, const uint32_t primitive, const ShadingMode shadingMode);
		void Update_(const glm::mat4& viewMatrix)
		{
			assert(IsValid());
			switch(shadingMode)
			{
				case ShadingMode::GIZMO:
				{
					SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix);
				}break;
				case ShadingMode::GOOCH:
				{
					SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix);
					SetVec3("viewPos", glm::vec3(viewMatrix[3]));
				}break;
				case ShadingMode::ALBEDO_ONLY:
				{
					SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix);
				}break;
				case ShadingMode::BLINN_PHONG:
				{
					SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix);
					SetVec3("viewPos", glm::vec3(viewMatrix[3]));
				}break;
				case ShadingMode::BLINN_PHONG_NORMALMAPPED:
				{
					SetMat4("cameraMatrix", WINDOW_PROJECTION * viewMatrix);
					SetVec3("viewPos", glm::vec3(viewMatrix[3]));
				}break;
				default:
				{
					sge_ERROR("Invalid IllumMode in Shader!");
				}break;
			}
		}
	};
}//!sge