#pragma once

#include "globals.h"
#include "Hash.h"

namespace sge
{
	struct VertexBuffer
	{
		enum class Target: uint32_t
		{
			INVALID = 0,

			VBO = 0x8892, // GL_ARRAY_BUFFER
			EBO = 0x8893, // GL_ELEMENT_ARRAY_BUFFER

			MAX_VALUE = EBO
		};

		enum class Type: uint16_t
		{
			INVALID = 0,

			POSITIONS_VEC3 = 1 << 0,
			POSITIONS_VEC2 = 1 << 1,
			NORMALS = 1 << 2,
			TANGENTS = 1 << 3,

			BITANGENTS = 1 << 4,
			UVS = 1 << 5,
			INDICES_UINT32 = 1 << 6,
			MODEL_MATRIX = 1 << 7,

			GENERIC_VEC3 = 1 << 8,
			GENERIC_FLOAT = 1 << 9,

			MAX_VALUE = GENERIC_FLOAT
		};

		struct Definition
		{
			// Note: this points heap! Use Renderer's CreateVertexBuffer() method to ensure the heap gets freed!
			uint8_t* begin = nullptr;
			uint32_t byteLen = 0;
			Mutability mutability = Mutability::INVALID;
			NumberType componentType = NumberType::INVALID; // TODO: is this really needed if we know the Type?
			uint32_t componentsPerElement = 0; // TODO: is this really needed if we know the Type?
			Type bufferContentsType = Type::INVALID;
			bool isIndexBuffer = false;
			Hash preComputedHash = 0; // Needs to be here since begin will get deallocated upon VertexBuffer::Init_() .

			bool IsValid() const;
		};

		uint32_t VBO = 0;
		Mutability mutability = Mutability::INVALID;
		NumberType componentType = NumberType::INVALID;
		uint32_t componentsPerElement = 0;
		bool isIndexBuffer = false;
		Type bufferContentsType = Type::INVALID;

		void Update(void* data, const uint32_t byteLen) const;

		void Bind() const;

		inline bool IsValid() const
		{
			return VBO && (bool)mutability && (bool)componentType && componentsPerElement && (bool)bufferContentsType;
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