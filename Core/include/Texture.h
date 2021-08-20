#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Resources.h"
#include "utility.h"
#include "EnumsAndFlags.h"

namespace sge
{
	class Texture: public I_Validateable
	{
		friend class Renderer;

	public:
		class Definition: public I_Validateable
		{
		public:
			std::vector<Triple<void*, uint32_t, glm::ivec2>> data = {};
			Pair<SamplingMode, SamplingMode> samplingModes = {};
			Pair<WrappingMode, WrappingMode> wrappingModes = {};
			ColorFormat format = ColorFormat::INVALID;
			TextureCompressionMode compression = TextureCompressionMode::INVALID;
			bool generateMipMaps = false;
			Hash preComputedHash = 0; // Note: need to compute the hash before it gets destroyed on Texture's Init_(). Needed to compute hashes of any overarching structure like meshes.

			bool IsValid() const override;
		};

		void Bind(const uint32_t textureUnit) const;

		bool IsValid() const override;

	private:
		uint32_t TEX_ = 0;

		void Init_(const Definition& def);
		void Destroy_();
	};
}//!sge