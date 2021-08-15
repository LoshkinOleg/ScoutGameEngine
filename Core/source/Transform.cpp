#include "Transform.h"

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
}//!sge