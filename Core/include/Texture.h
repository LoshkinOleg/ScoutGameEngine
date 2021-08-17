#pragma once

#include <vector>

#include "Hash.h"
#include "globals.h"

namespace sge
{
	struct Texture
	{
		enum class SamplingMode: uint32_t
		{
			INVALID = 0,

			NEAREST = 0x2600, // GL_NEAREST
			LINEAR = 0x2601, // GL_LINEAR
			LINEAR_MIPMAP_NEAREST = 0x2701, // GL_LINEAR_MIPMAP_NEAREST
			LINEAR_MIPMAP_LINEAR = 0x2703, // GL_LINEAR_MIPMAP_LINEAR

			MAX_VALUE = LINEAR_MIPMAP_LINEAR
		};
		enum class WrappingMode: uint32_t
		{
			INVALID = 0,

			CLAMP = 0x812F, // GL_CLAMP_TO_EDGE
			REPEAT = 0x2901, // GL_REPEAT
			MIRRORED_REPEAT = 0x8370, // GL_MIRRORED_REPEAT

			MAX_VALUE = MIRRORED_REPEAT
		};
		enum class Format: uint8_t
		{
			INVALID = 0,

			RED_F32,
			RGBA_B8,

			MAX_VALUE = RGBA_B8
		};
		enum class Compression: uint32_t
		{
			INVALID = 0,

			NONE = 1,
			ETC1 = 0x8D64, // GL_COMPRESSED_RGB8_ETC1
			ETC2 = 0x9278, // GL_COMPRESSED_RGBA8_ETC2_EAC
			ASTC_RGBA_4x4 = 0x93B0, // GL_COMPRESSED_RGBA_ASTC_4x4_KHR

			MAX_VALUE = ASTC_RGBA_4x4
		};
		struct Definition
		{
			std::vector<void*> datas = {};
			std::vector<uint32_t> byteLens = {};
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
			Compression compression = Compression::INVALID;
			Hash preComputedHash = 0; // Note: need to compute the hash before it gets destroyed on Texture's Init_(). Needed to compute hashes of any overarching structure like meshes.

			// Definition() = default;
			// ~Definition() = default;
			// Definition(Definition&) = default;
			// Definition(Definition&&) = default;
			// Definition& operator=(Definition&&) = default;

			inline bool IsValid() const
			{
				return preComputedHash.value && (bool)compression && datas.size() && byteLens.size() && (uint32_t)mutability && (widths.size() == (size_t)mipLevels + 1) && (widths.size() == heights.size()) && (uint32_t)minifyingMode && (uint32_t)magnifyingMode && (uint32_t)onS && (uint32_t)onT;
			}
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
			return TEX && (uint32_t)mutability && widths.size() && (widths.size() == heights.size()) && (uint32_t)minifyingMode && (uint32_t)magnifyingMode && (uint32_t)wrappingModeS && (uint32_t)wrappingModeT;
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