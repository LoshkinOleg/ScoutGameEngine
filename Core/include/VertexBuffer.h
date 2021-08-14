#pragma once

#include <glad/glad.h>

#include "globals.h"
#include "macros.h"

namespace sge
{
	struct VertexBuffer
	{
		struct Definition
		{
			enum Type: uint32_t
			{
				INVALID = 0,

				POSITIONS_VEC3,
				POSITIONS_VEC2,
				NORMALS,
				TANGENTS,
				BITANGENTS,
				UVS,
				INDICES_UINT32,
				OTHER
			};

			sge_ALLOW_CONSTRUCTION(Definition);
			sge_DISALLOW_COPY(Definition);

			// Note: this points heap! Use Renderer's CreateVertexBuffer() method to ensure the heap gets freed!
			uint8_t* begin = nullptr;
			uint32_t byteLen = 0;
			Mutability usage = Mutability::INVALID;
			ComponentType componentType = ComponentType::INVALID;
			uint32_t componentsPerElement = 0;
			Type type = Type::INVALID;
			bool isIndexBuffer = false;

			inline bool IsValid() const
			{
				return begin && byteLen && usage && componentType && componentsPerElement && type;
			}
		};

		uint32_t VBO = 0;
		Mutability usage = Mutability::INVALID;
		ComponentType componentType = ComponentType::INVALID;
		uint32_t componentsPerElement = 0;
		bool isIndexBuffer = false;

		void Update(void* data, const uint32_t byteLen) const;

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

	private:
		friend class Renderer;
		void Init_(const Definition& def);
	};
}//!sge