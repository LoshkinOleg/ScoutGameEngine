#pragma once

#include "Resources.h"
#include "EnumsAndFlags.h"
#include "utility.h"

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
			Pair<void*, uint32_t> data = {};
			Mutability mutability = Mutability::INVALID;
			VertexBufferType bufferContentsType = VertexBufferType::INVALID;
			Hash opt_preComputedHash = 0; // Needs to be here since begin will get deallocated upon VertexBuffer::Init_() .

			bool IsValid() const override;
		};

		void Bind() const;
		void Update(void* data, const uint32_t byteLen) const;
		uint32_t Stride() const;

		bool IsValid() const override;

	private:
		uint32_t VBO_ = 0;
		Mutability mutability_ = Mutability::INVALID;
		VertexBufferType contentsType_ = VertexBufferType::INVALID;

		void Init_(const Definition& def);
		void Destroy_();
	};
}//!sge