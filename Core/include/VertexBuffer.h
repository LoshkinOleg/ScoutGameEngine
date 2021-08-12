#pragma once

#include "globals.h"

namespace sge
{
	struct VertexBuffer
	{
		struct Definition
		{
			uint8_t* begin = nullptr;
			uint8_t* end = nullptr;
			Mutability usage = Mutability::INVALID;
			bool isIndexBuffer = false;
		};

		uint32_t VBO = 0;
		Mutability usage = Mutability::INVALID;
		bool isIndexBuffer = false;

		void Init(const Definition& def);
		void Update(void* data, const uint32_t byteLen);
		void Destroy();

		inline bool IsValid() const
		{
			return VBO && usage;
		}
		inline void Reset()
		{
			*this = {};
		}
		inline GLenum Target() const
		{
			return isIndexBuffer ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
		}
	};
}//!sge