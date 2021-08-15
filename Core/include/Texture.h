#pragma once

#include <vector>

#include <glad/glad.h>

#include "globals.h"

namespace sge
{
	struct Texture
	{
		enum SamplingMode: uint32_t
		{
			INVALID = 0,
			LINEAR = GL_LINEAR,
			NEAREST = GL_NEAREST,
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
		};
		enum WrappingMode: uint32_t
		{
			INVALID = 0,
			CLAMP = GL_CLAMP_TO_EDGE,
			REPEAT = GL_REPEAT,
			MIRRORED_REPEAT = GL_MIRRORED_REPEAT
		};
		enum Format: uint32_t
		{
			INVALID = 0,
			RED_F32,
			RGBA_B8,
		};
		struct Definition
		{
			std::vector<const void* const> datas = {}; // one per mipmap level
			Mutability mutability = Mutability::INVALID;
			Format format = Format::INVALID;
			std::vector<uint32_t> widths = {}; // one per mipmap level
			std::vector<uint32_t> heights = {};
			uint32_t mipLevels = 0;
			SamplingMode minifyingMode = SamplingMode::INVALID;
			SamplingMode magnifyingMode = SamplingMode::INVALID;
			WrappingMode onS = WrappingMode::INVALID;
			WrappingMode onT = WrappingMode::INVALID;
			bool generateMipMaps = false;

			inline bool IsValid() const
			{
				return datas.size() && mutability && (widths.size() == mipLevels + 1) && (widths.size() == heights.size()) && minifyingMode && magnifyingMode && onS && onT;
			}
			uint32_t ByteSize(const uint32_t mipLevel) const;
		};

		uint32_t TEX = 0;
		std::vector<uint32_t> widths = {}; // one per mipmap level
		std::vector<uint32_t> heights = {};
		Mutability mutability = Mutability::INVALID;
		SamplingMode minifyingMode = SamplingMode::INVALID, magnifyingMode = SamplingMode::INVALID;
		WrappingMode wrappingModeS = WrappingMode::INVALID, wrappingModeT = WrappingMode::INVALID;

		void UpdateData(const void* const data) const;
		void Bind(const uint32_t textureUnit) const;

		inline bool IsValid() const
		{
			return TEX && mutability && widths.size() && (widths.size() == heights.size()) && minifyingMode && magnifyingMode && wrappingModeS && wrappingModeT;
		}
		inline void Reset()
		{
			*this = {};
		}

	private:
		friend class Renderer;
		void Init_(const Definition& def);
		void Destroy_();
	};
}//!sge