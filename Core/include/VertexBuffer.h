#pragma once

#include "globals.h"
#include "macros.h"

namespace sge
{
	struct VertexBuffer
	{
		enum Target: uint32_t
		{
			INVALID = 0,

			VBO = 0x8892, // GL_ARRAY_BUFFER
			EBO = 0x8893, // GL_ELEMENT_ARRAY_BUFFER

			MAX_VALUE = EBO
		};

		struct Definition
		{
			enum Type: uint8_t
			{
				INVALID = 0,

				POSITIONS_VEC3 = 1 << 0,
				POSITIONS_VEC2 = 1 << 1,
				NORMALS = 1 << 2,
				TANGENTS = 1 << 3,

				BITANGENTS = 1 << 4,
				UVS = 1 << 5,
				INDICES_UINT32 = 1 << 6,
				OTHER = 1 << 7,

				MAX_VALUE = OTHER
			};

			sge_ALLOW_CONSTRUCTION(Definition);
			sge_DISALLOW_COPY(Definition);

			// Note: this points heap! Use Renderer's CreateVertexBuffer() method to ensure the heap gets freed!
			uint8_t* begin = nullptr;
			uint32_t byteLen = 0;
			Mutability usage = Mutability::INVALID;
			NumberType componentType = NumberType::INVALID;
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
		NumberType componentType = NumberType::INVALID;
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
		uint32_t Stride() const;

	private:
		friend class Renderer;
		void Init_(const Definition& def);
		void Destroy_();

		inline Target Target_() const
		{
			return isIndexBuffer ? Target::EBO : Target::VBO;
		}
	};
}//!sge