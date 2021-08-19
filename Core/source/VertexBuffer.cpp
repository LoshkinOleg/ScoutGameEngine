#include "VertexBuffer.h"

#include <glad/glad.h>

#include "macros.h"

namespace sge
{
	bool VertexBuffer::Definition::IsValid() const
	{
		bool returnVal = (bool)componentType;
		assert(returnVal);
		returnVal &= (bool)componentsPerElement;
		assert(returnVal);
		returnVal &= (bool)bufferContentsType;
		assert(returnVal);
		returnVal &= (bool)mutability;
		assert(returnVal);
		if(mutability == Mutability::STATIC) // If it's dynamic, begin, byteLen and preComputedHash can be 0.
		{
			returnVal &= (bool)begin;
			assert(returnVal);
			returnVal &= (bool)byteLen;
			assert(returnVal);
			returnVal &= (bool)preComputedHash.value;
			assert(returnVal);
		}
		return returnVal;
	}
	bool VertexBuffer::IsValid() const
	{
		bool returnVal = VBO > 0;
		assert(returnVal);
		returnVal &= (bool)mutability;
		assert(returnVal);
		returnVal &= (bool)componentType;
		assert(returnVal);
		returnVal &= componentsPerElement > 0;
		assert(returnVal);
		returnVal &= (bool)bufferContentsType;
		assert(returnVal);
		return returnVal;
	}
	VertexBuffer::VertexBufferTarget VertexBuffer::Target_() const
	{
		return isIndexBuffer ? VertexBufferTarget::EBO : VertexBufferTarget::VBO;
	}
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
			// Note: this is the reason there needs to be a preComputedHash.
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
		// Note: this recreates the whole buffer... Not ideal.
		glBufferData(target, (GLsizeiptr)byteLen, data, (GLenum)Mutability::DYNAMIC);
		sge_CHECK_GL_ERROR();
	}
	void VertexBuffer::Destroy_()
	{
		assert(IsValid());
		glDeleteBuffers(1, &VBO);
	}
	void VertexBuffer::Bind() const
	{
		glBindBuffer((GLenum)Target_(), VBO);
	}
	uint32_t VertexBuffer::Stride() const
	{
		switch(bufferContentsType)
		{
			case sge::VertexBuffer::VertexBufferType::POSITIONS_VEC3:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 3);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::POSITIONS_VEC2:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 2);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::NORMALS:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 3);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::TANGENTS:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 3);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::BITANGENTS:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 3);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::UVS:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 2);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::INDICES_UINT32:
			{
				assert(componentType == NumberType::UINT && componentsPerElement == 1);
				return sizeof(uint32_t) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::MODEL_MATRIX:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 16);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::GENERIC_VEC3:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 3);
				return sizeof(float) * componentsPerElement;
			}break;
			case sge::VertexBuffer::VertexBufferType::GENERIC_FLOAT:
			{
				assert(componentType == NumberType::FLOAT && componentsPerElement == 1);
				return sizeof(float) * componentsPerElement;
			}break;
			default:
			{
				sge_ERROR("Unexpected vertex buffer content type!");
			}break;
		}
	}
}//!sge