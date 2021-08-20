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
			glm::mat4* beginPtr = nullptr;
			glm::mat4* endPtr = nullptr;
			uint32_t beginIdx = 0;
			uint32_t endIdx = 0;
			uint32_t nrOfMatrices = 0;

			MatrixRange() = delete;
			MatrixRange(glm::mat4* const beginPtr, glm::mat4* const endPtr):
				beginPtr(beginPtr), endPtr(endPtr), beginIdx(0), endIdx(endPtr - beginPtr), nrOfMatrices(endPtr - beginPtr){};
			MatrixRange(glm::mat4* const beginPtr, const uint32_t nrOfMatrices):
				beginPtr(beginPtr), endPtr(beginPtr + nrOfMatrices), beginIdx(0), endIdx(nrOfMatrices), nrOfMatrices(nrOfMatrices){};
			MatrixRange(const uint32_t beginIdx, const uint32_t endIdx):
				beginPtr(nullptr), endPtr(nullptr), beginIdx(beginIdx), endIdx(endIdx), nrOfMatrices(endIdx - beginIdx){};
			MatrixRange(const uint32_t nrOfMatrices):
				beginPtr(nullptr), endPtr(nullptr), beginIdx(0), endIdx(nrOfMatrices), nrOfMatrices(nrOfMatrices){};

			bool IsValid() const override;
		};

		void Translate(const glm::vec3 deltaPos, const MatrixRange& range);
		void Rotate(const float radians, const glm::vec3 axis, const MatrixRange& range);
		void Scale(const glm::vec3 deltaScale, const MatrixRange& range);

		const MatrixRange ReadMatrix(const MatrixRange& range) const;
		void WriteMatrix(const glm::mat4& value, const MatrixRange& range);
		void WriteMatrix(const MatrixRange& range);

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