#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "VertexBuffer.h"

#include "macros.h"

namespace sge
{
	class TransformsBuffer: public I_Validateable
	{
		friend class ResourceManager;

	public:
		class MatrixRange: public I_Validateable
		{
		public:
			glm::mat4* begin = nullptr;
			uint32_t nrOfMatrices = 0;

			bool IsValid() const override;
		};

		void Translate(const glm::vec3 deltaPos, const uint32_t begin, const uint32_t end);
		void Rotate(const float radians, const glm::vec3 axis, const uint32_t begin, const uint32_t end);
		void Scale(const glm::vec3 deltaScale, const uint32_t begin, const uint32_t end);

		const MatrixRange ReadMatrix(const uint32_t begin, const uint32_t end) const;
		void WriteMatrix(const glm::mat4& value, const uint32_t begin, const uint32_t end);

		const glm::mat4* const GetBegin() const;
		uint32_t GetNrOfTransforms() const;

		bool IsValid() const override;

	private:
		glm::mat4* begin_ = nullptr;
		uint32_t nrOfTransforms_ = 0;

		void Init_(const std::vector<glm::mat4>& transforms);
	};

	class ModelMatrixPool
	{
		friend class ResourceManager;

	public:
		glm::mat4* const Allocate(const uint32_t nrOfTransforms);

	private:
		glm::mat4* begin_ = nullptr;
		uint32_t current_ = 0;
		uint32_t max_ = 0;

		void Init_(const uint32_t nrOfTransforms);
		void Destroy_();
	};
}//!sge