#pragma once

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

		uint32_t TEX = 0;
		uint32_t width = 0, height = 0;
		Mutability mutability = Mutability::INVALID;
		SamplingMode minifyingMode = SamplingMode::INVALID, magnifyingMode = SamplingMode::INVALID;
		WrappingMode wrappingModeS = WrappingMode::INVALID, wrappingModeT = WrappingMode::INVALID;

		void Init(void* data,
				  const Mutability mutability,
				  const Format format,
				  const uint32_t width,
				  const uint32_t height,
				  const uint32_t mipLevels,
				  const SamplingMode minifyingMode,
				  const SamplingMode magnifyingMode,
				  const WrappingMode wrappingModeS,
				  const WrappingMode wrappingModeT);
		void Update(void* data) const;
		void Destroy();

		inline bool IsValid() const
		{
			return TEX && mutability && width && height && minifyingMode && magnifyingMode && wrappingModeS && wrappingModeT;
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge