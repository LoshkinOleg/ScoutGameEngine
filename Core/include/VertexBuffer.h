#pragma once

#include <glad/glad.h>

#include "globals.h"

namespace sge
{
	struct VertexBuffer
	{
		struct Definition
		{
			uint8_t* begin = nullptr;
			uint32_t byteLen = 0;
			Mutability usage = Mutability::INVALID;
			ComponentType componentType = ComponentType::INVALID;
			uint32_t componentsPerElement = 0;
			bool isIndexBuffer = false;

			inline bool IsValid() const
			{
				return begin && byteLen && usage && componentType && componentsPerElement;
			}
		};

		uint32_t VBO = 0;
		Mutability usage = Mutability::INVALID;
		ComponentType componentType = ComponentType::INVALID;
		uint32_t componentsPerElement = 0;
		bool isIndexBuffer = false;

		void Init(const Definition& def);
		void Update(void* data, const uint32_t byteLen) const;
		void Destroy();

		void Bind() const;

		inline bool IsValid() const
		{
			return VBO && usage && componentType && componentsPerElement;
		}
		inline void Reset()
		{
			*this = {};
		}
		inline GLenum Target() const
		{
			return isIndexBuffer ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
		}
		uint32_t Stride() const;
	};
}//!sge