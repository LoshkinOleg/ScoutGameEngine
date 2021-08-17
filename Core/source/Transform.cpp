#include "Transform.h"

#include <glad/glad.h>

#include "Engine.h"

namespace sge
{
	uint32_t TransformsBuffer::NrOfTransforms() const
	{
		assert(end - begin > 0);
		return (uint32_t)(end - begin);
	}
	bool TransformsBuffer::IsValid() const
	{
		return end > begin;
	}
	void TransformsBuffer::Bind(const uint32_t VAO, const uint32_t transformModelOffset) const
	{
		glBindVertexArray(VAO);
		vbo->Update(begin, NrOfTransforms() * sizeof(glm::mat4));
		glEnableVertexAttribArray((GLuint)transformModelOffset);
		glVertexAttribPointer((GLuint)transformModelOffset, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray((GLuint)transformModelOffset + 1);
		glVertexAttribPointer((GLuint)transformModelOffset + 1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
		glEnableVertexAttribArray((GLuint)transformModelOffset + 2);
		glVertexAttribPointer((GLuint)transformModelOffset + 2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)));
		glEnableVertexAttribArray((GLuint)transformModelOffset + 3);
		glVertexAttribPointer((GLuint)transformModelOffset + 3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)));
		glVertexAttribDivisor((GLuint)transformModelOffset, 1);
		glVertexAttribDivisor((GLuint)transformModelOffset + 1, 1);
		glVertexAttribDivisor((GLuint)transformModelOffset + 2, 1);
		glVertexAttribDivisor((GLuint)transformModelOffset + 3, 1);
	}
	void TransformsBuffer::Init_(const std::vector<glm::mat4>& transforms)
	{
		VertexBuffer::Definition vbDef;
		vbDef.begin = nullptr;
		vbDef.bufferContentsType = VertexBuffer::Type::MODEL_MATRIX;
		vbDef.byteLen = 0;
		vbDef.componentsPerElement = 16;
		vbDef.componentType = NumberType::FLOAT;
		vbDef.isIndexBuffer = false;
		vbDef.mutability = Mutability::DYNAMIC;
		vbDef.preComputedHash = 0;
		vbo = Engine::Get().GetRenderer().CreateVertexBuffer(vbDef);
		begin = Engine::Get().GetResourceManager().AllocateTransforms(transforms.data(), transforms.size() * sizeof(glm::mat4));
		end = begin + transforms.size();

		for(size_t i = 0; i < transforms.size(); i++)
		{
			*(begin + i) = transforms[i];
		}
	}
}//!sge