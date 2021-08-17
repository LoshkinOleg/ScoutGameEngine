#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "VertexBuffer.h"
#include "Resources.h"

namespace sge
{
	struct TransformsBuffer
	{
		glm::mat4* begin = nullptr;
		glm::mat4* end = nullptr;

		Handle<VertexBuffer> vbo = {};

		void Bind(const uint32_t VAO, const uint32_t transformModelOffset) const;

		uint32_t NrOfTransforms() const;
		bool IsValid() const;

	private:
		friend struct Model;
		void Init_(const std::vector<glm::mat4>& transforms);
	};
}//!sge