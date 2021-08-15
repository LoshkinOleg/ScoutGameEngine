#pragma once

#include <glm/glm.hpp>

namespace sge
{
	struct TransformsBuffer
	{
		glm::mat4* begin = nullptr;
		glm::mat4* end = nullptr;

		uint32_t NrOfTransforms() const;
		bool IsValid() const;
	};
}//!sge