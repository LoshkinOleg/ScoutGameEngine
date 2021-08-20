#pragma once

#include <string_view>
#include <map>

#include <glm/glm.hpp>

#include "Resources.h"
#include "EnumsAndFlags.h"

namespace sge
{
	class Shader: public I_Validateable
	{
		friend class Renderer;

	public:
		void Bind() const;

		int32_t GetUniformLocation(const std::string_view name);
		void SetInt(const std::string_view name, const int32_t value);
		void SetFloat(const std::string_view name, const float value);
		void SetVec3(const std::string_view name, const glm::vec3 value);
		void SetMat4(const std::string_view name, const glm::mat4& value);
		void SetMat3(const std::string_view name, const glm::mat3& value);

		bool IsValid() const override;

	private:
		uint32_t PROGRAM_ = 0;
		std::map<Hash, uint32_t> uniformLocationCache_ = {};

		void Init_(const std::string_view vertexSrc,
				   const std::string_view fragmentSrc,
				   const std::string_view geometrySrc);
		void UpdatePerFrameUniforms_(const glm::mat4& viewMatrix, const ShadingMode mode);
		void Destroy_();
	};
}//!sge