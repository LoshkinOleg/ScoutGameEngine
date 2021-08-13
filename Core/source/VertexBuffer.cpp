#include "VertexBuffer.h"

#include <iostream>

#include "macros.h"

namespace sge
{
	void VertexBuffer::Init(const Definition& def)
	{
		usage = def.usage;
		componentType = def.componentType;
		componentsPerElement = def.componentsPerElement;
		isIndexBuffer = def.isIndexBuffer;
		sge_CHECK_GL_ERROR();
		glGenBuffers(1, &VBO);
		if(def.isIndexBuffer)
		{
			// Note: if it's an index buffer, a VAO must be bound to link it to the VAO.
			GLint current = 0;
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
			assert(current);
		}
		glBindBuffer(Target(), VBO);
		glBufferData(Target(), (GLsizeiptr)def.byteLen, def.begin, (GLenum)def.usage);
		sge_CHECK_GL_ERROR();
	}
	void VertexBuffer::Update(void* data, const uint32_t byteLen) const
	{
		assert(IsValid());
		assert(usage == Mutability::DYNAMIC && Target() == GL_ARRAY_BUFFER); // Not handling dynamic indices.
		glBindBuffer(Target(), VBO);
		glBufferSubData(Target(), 0, (GLsizeiptr)byteLen, data);
		sge_CHECK_GL_ERROR();
	}
	void VertexBuffer::Destroy()
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
		glBindBuffer(Target(), VBO);
	}
	uint32_t VertexBuffer::Stride() const
	{
		switch(componentType)
		{
			case ComponentType::FLOAT:
			{
				return sizeof(float) * componentsPerElement;
			}break;
			case ComponentType::UINT:
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