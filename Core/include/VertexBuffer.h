#pragma once

#include "Resources.h"
#include "EnumsAndFlags.h"

namespace sge
{
	class VertexBuffer: public I_Validateable
	{
		friend class Renderer;

	public:
		class Definition: public I_Validateable
		{
		public:
			// Note: this points heap! Use Renderer's CreateVertexBuffer() method to ensure the heap gets freed!
			uint8_t* begin = nullptr;
			uint32_t byteLen = 0;
			Mutability mutability = Mutability::INVALID;
			NumberType componentType = NumberType::INVALID; // TODO: is this really needed if we know the Type?
			uint32_t componentsPerElement = 0; // TODO: is this really needed if we know the Type?
			VertexBufferType bufferContentsType = VertexBufferType::INVALID;
			bool isIndexBuffer = false;
			Hash preComputedHash = 0; // Needs to be here since begin will get deallocated upon VertexBuffer::Init_() .

			bool IsValid() const override;
		};

		void Bind() const;
		void Update(void* data, const uint32_t byteLen) const;
		uint32_t Stride() const;

		bool IsValid() const override;

	private:
		uint32_t VBO_ = 0;
		bool isIndexBuffer_ = false;
		Mutability mutability_ = Mutability::INVALID;
		VertexBufferType contentsType_ = VertexBufferType::INVALID;

		void Init_(const Definition& def);
		void Destroy_();

		VertexBufferTarget Target_() const;
	};
}//!sge