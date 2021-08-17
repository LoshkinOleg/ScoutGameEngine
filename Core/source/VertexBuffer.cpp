#include "VertexBuffer.h"

#include <glad/glad.h>

#include "macros.h"

namespace sge
{
	void VertexBuffer::Init_(const Definition& def)
	{
		assert(def.IsValid());
		mutability = def.mutability;
		componentType = def.componentType;
		componentsPerElement = def.componentsPerElement;
		isIndexBuffer = def.isIndexBuffer;
		bufferContentsType = def.bufferContentsType;
		sge_CHECK_GL_ERROR();
		glGenBuffers(1, &VBO);
		if(def.isIndexBuffer)
		{
			// Note: if it's an index buffer, a VAO must be bound to link it to the VAO.
			GLint current = 0;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
			assert(current);
		}
		const GLenum target = (GLenum)Target_();
		glBindBuffer(target, VBO);
		glBufferData(target, (GLsizeiptr)def.byteLen, def.begin, (GLenum)def.mutability);
		sge_CHECK_GL_ERROR();

		if(def.begin)
		{
			delete[] def.begin; // Free buffer data stored on heap.
		}

		assert(IsValid());
	}
	void VertexBuffer::Update(void* data, const uint32_t byteLen) const
	{
		assert(IsValid());
		const GLenum target = (GLenum)Target_();
		assert(mutability == Mutability::DYNAMIC && target == GL_ARRAY_BUFFER); // Not handling dynamic indices.
		glBindBuffer(target, VBO);
		// glBufferSubData(target, 0, (GLsizeiptr)byteLen, data);
		glBufferData(target, (GLsizeiptr)byteLen, data, (GLenum)Mutability::DYNAMIC);
		sge_CHECK_GL_ERROR();
	}
	void VertexBuffer::Destroy_()
	{
		if(IsValid())
		{
			glDeleteBuffers(1, &VBO);
			Reset();
		}
		else
		{
			sge_WARNING("Attempting to delete an invalid VertexBuffer!");
		}
	}
	void VertexBuffer::Bind() const
	{
		glBindBuffer((GLenum)Target_(), VBO);
	}
	uint32_t VertexBuffer::Stride() const
	{
		switch(componentType)
		{
			case NumberType::FLOAT:
			{
				return sizeof(float) * componentsPerElement;
			}break;
			case NumberType::UINT:
			{
				return sizeof(uint32_t) * componentsPerElement;
			}break;
			default:
			{
				sge_ERROR("Unexpected component type!");
			}break;
		}
	}
}//!sge