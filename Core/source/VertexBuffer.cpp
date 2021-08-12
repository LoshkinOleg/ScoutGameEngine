#include "VertexBuffer.h"

#include <iostream>

#include "macros.h"

namespace sge
{
	void VertexBuffer::Init(const Definition& def)
	{
		usage = def.usage;
		isIndexBuffer = def.isIndexBuffer;
		sge_CHECK_GL_ERROR();
		glGenBuffers(1, &VBO);
		glBindBuffer(Target(), VBO);
		glBufferData(Target(), (GLsizeiptr)(def.end - def.begin), def.begin, (GLenum)def.usage);
		sge_CHECK_GL_ERROR();
	}
	void VertexBuffer::Update(void* data, const uint32_t byteLen)
	{
		assert(IsValid());
		assert(usage == Mutability::DYNAMIC);
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
}//!sge