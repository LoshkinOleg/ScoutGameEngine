#pragma once

#include "Resources.h"
#include "EnumsAndFlags.h"
#include "utility.h"

namespace sge
{
	class VertexBuffer
	{
		friend class Renderer;

	public:
		class Definition
		{
		public:
			// Note: this points heap! Use Renderer's CreateVertexBuffer() method to ensure the heap gets freed!
			Pair<void*, uint32_t> data = {};
			E_Mutability mutability = E_Mutability::INVALID;
			F_VertexBufferType bufferContentsType = F_VertexBufferType::INVALID;
			Hash opt_preComputedHash = 0; // Needs to be here since begin will get deallocated upon VertexBuffer::Init_() .

			bool IsValid() const;
		};

		void Bind() const;
		void Update(void* data, const uint32_t byteLen) const;
		uint32_t Stride() const;

		bool IsValid() const;

	private:
		uint32_t VBO_ = 0; // 4 bytes
		E_Mutability mutability_ = E_Mutability::INVALID; // 4 bytes
		F_VertexBufferType contentsType_ = F_VertexBufferType::INVALID; // 2 bytes
		// 2 bytes padding

		void Init_(const Definition& def);
		void Destroy_();
	};
}//!sge