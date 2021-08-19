#include "Transform.h"

#include <glad/glad.h>

#include "Engine.h"

namespace sge
{
	bool TransformsBuffer::IsValid() const
	{
		bool returnVal = begin_ != nullptr;
		assert(returnVal);
		returnVal &= nrOfTransforms_ > 0;
		assert(returnVal);
		return returnVal;
	}
	void TransformsBuffer::Init_(const std::vector<glm::mat4>& transforms)
	{
		auto& rm = Engine::Get().GetResourceManager();
		begin_ = rm.AllocateModelMatrices((uint32_t)transforms.size());
		nrOfTransforms_ = (uint32_t)transforms.size();
		for(size_t i = 0; i < nrOfTransforms_; i++)
		{
			*(begin_ + i) = transforms[i];
		}
	}
	void TransformsBuffer::Translate(const glm::vec3 deltaPos, const uint32_t begin, const uint32_t end)
	{
		for(uint32_t i = begin; i < end + 1; i++)
		{
			auto& matrix = *(begin_ + i);
			matrix = glm::translate(matrix, deltaPos);
		}
	}
	void TransformsBuffer::Rotate(const float radians, const glm::vec3 axis, const uint32_t begin, const uint32_t end)
	{
		for(uint32_t i = begin; i < end + 1; i++)
		{
			auto& matrix = *(begin_ + i);
			matrix = glm::rotate(matrix, radians, axis);
		}
	}
	void TransformsBuffer::Scale(const glm::vec3 deltaScale, const uint32_t begin, const uint32_t end)
	{
		for(uint32_t i = begin; i < end + 1; i++)
		{
			auto& matrix = *(begin_ + i);
			matrix = glm::scale(matrix, deltaScale);
		}
	}
	void TransformsBuffer::WriteMatrix(const glm::mat4 & value, const uint32_t begin, const uint32_t end)
	{
		for(uint32_t i = begin; i < end + 1; i++)
		{
			*(begin_ + i) = value;
		}
	}
	glm::mat4* TransformsBuffer::GetBegin()
	{
		return begin_;
	}
	const uint32_t TransformsBuffer::GetNrOfTransforms() const
	{
		return nrOfTransforms_;
	}
	void ModelMatrixPool::Init_(const uint32_t nrOfTransforms)
	{
		begin_ = new glm::mat4[nrOfTransforms];
		max_ = nrOfTransforms;
		for(glm::mat4* it = begin_; it < begin_ + nrOfTransforms; it++)
		{
			*it = IDENTITY_MAT4;
		}
	}
	void ModelMatrixPool::Destroy_()
	{
		delete[] begin_;
	}
	glm::mat4 * const ModelMatrixPool::Allocate(const uint32_t nrOfTransforms)
	{
		assert(current_ + nrOfTransforms < max_);
		glm::mat4* returnVal = begin_ + current_;
		current_ += nrOfTransforms;
		return returnVal;
	}
}//!sge