#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "VertexBuffer.h"
#include "Resources.h"

#include "macros.h"

namespace sge
{
	class TransformsBuffer
	{
	private:
		friend class ResourceManager;

		glm::mat4* begin_ = nullptr;
		uint32_t nrOfTransforms_ = 0;

		void Init_(const std::vector<glm::mat4>& transforms);

	public:
		void Translate(const glm::vec3 deltaPos, const uint32_t begin, const uint32_t end);
		void Rotate(const float radians, const glm::vec3 axis, const uint32_t begin, const uint32_t end);
		void Scale(const glm::vec3 deltaScale, const uint32_t begin, const uint32_t end);
		void SetMatrix(const glm::mat4& value, const uint32_t begin, const uint32_t end);

		bool IsValid() const;
		const glm::mat4& GetBegin() const;
		const uint32_t GetNrOfTransforms() const;
	};

	class ModelMatrixPool
	{
	private:
		friend class ResourceManager;
		glm::mat4* begin_ = nullptr;
		uint32_t end_ = 0;
		uint32_t max_ = 0;

		void Init_(const uint32_t nrOfTransforms);
		void Destroy_();
	public:
		glm::mat4* const Allocate(const uint32_t nrOfTransforms);
	};
}//!sge