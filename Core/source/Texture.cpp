#include "Texture.h"

#include <glad/glad.h>

#include "macros.h"

namespace sge
{
	bool Texture::Definition::IsValid() const
	{
		bool returnVal = preComputedHash.IsValid();
		assert(returnVal);
		returnVal &= (bool)compression;
		assert(returnVal);
		returnVal &= datas.size() > 0;
		assert(returnVal);
		returnVal &= byteLens.size() > 0;
		assert(returnVal);
		returnVal &= widths.size() > 0;
		assert(returnVal);
		returnVal &= heights.size() > 0;
		assert(returnVal);
		returnVal &= (datas.size() == byteLens.size() == widths.size() == heights.size());
		assert(returnVal);
		returnVal &= (bool)mutability;
		assert(returnVal);
		returnVal &= (bool)format;
		assert(returnVal);
		returnVal &= (mipLevels + 1 == (uint32_t)datas.size());
		assert(returnVal);
		returnVal &= (bool)minifyingMode;
		assert(returnVal);
		returnVal &= (bool)magnifyingMode;
		assert(returnVal);
		returnVal &= (bool)compression;
		assert(returnVal);
		returnVal &= (bool)onS;
		assert(returnVal);
		returnVal &= (bool)onT;
		assert(returnVal);
		returnVal &= preComputedHash.IsValid();
		assert(returnVal);
		if(generateMipMaps)
		{
			returnVal &= mipLevels == 0;
			assert(returnVal);
		}
		return returnVal;
	}
	bool Texture::IsValid() const
	{
		return TEX && (uint32_t)mutability && widths.size() && (widths.size() == heights.size()) && (uint32_t)minifyingMode && (uint32_t)magnifyingMode && (uint32_t)onS && (uint32_t)onT;
		bool returnVal = TEX > 0;
		assert(returnVal);
		returnVal &= (bool)mutability;
		assert(returnVal);
		returnVal &= widths.size() > 0;
		assert(returnVal);
		returnVal &= heights.size() > 0;
		assert(returnVal);
		returnVal &= (widths.size() == heights.size());
		assert(returnVal);
		returnVal &= (bool)minifyingMode;
		assert(returnVal);
		returnVal &= (bool)magnifyingMode;
		assert(returnVal);
		returnVal &= (bool)onS;
		assert(returnVal);
		returnVal &= (bool)onT;
		assert(returnVal);
	}
	void Texture::Init_(const Definition& def)
	{
		assert(def.IsValid());

		widths = def.widths;
		heights = def.heights;
		mutability = def.mutability;
		minifyingMode = def.minifyingMode;
		magnifyingMode = def.magnifyingMode;
		onS = def.onS;
		onT = def.onT;

		if(def.generateMipMaps)
		{
			sge_ERROR("Implement generation of mip maps!");
		}

		glGenTextures(1, &TEX);
		glBindTexture(GL_TEXTURE_2D, TEX);

		if(mutability == Mutability::DYNAMIC)
		{
			sge_CHECK_GL_ERROR();
			assert(def.compression == TextureCompressionMode::NONE);
			assert(def.format == ColorFormat::RED_F32); // Supporting only single channel float textures for dynamic ones.
			assert(!def.mipLevels && !def.generateMipMaps); // Not supporting mip mapping for dynamic textures.
			assert(def.widths.size() == 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)widths[0], (GLsizei)heights[0], 0, GL_RED, GL_FLOAT, def.datas[0]);
			sge_CHECK_GL_ERROR();
			assert((uint32_t)minifyingMode < GL_LINEAR_MIPMAP_NEAREST && (uint32_t)magnifyingMode < GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)onS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)onT);
			sge_CHECK_GL_ERROR();
		}
		else
		{
			// TODO: add support for compressed textures.
			if(def.compression != TextureCompressionMode::NONE)
			{
				switch(def.compression)
				{
					case TextureCompressionMode::ASTC_RGBA_4x4:
					{
						sge_ERROR("The loading of astc textures is not working yet!");

						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(def.mipLevels));
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)onS);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)onT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
						sge_CHECK_GL_ERROR();
						for(uint32_t level = 0; level < def.mipLevels + 1; level++)
						{
							glCompressedTexImage2D(GL_TEXTURE_2D,
												   (GLint)level,
												   (GLenum)def.compression, // Might be causing the crashing. Maybe there's a good reason glad doesn't have the necessary define...
												   (GLsizei)def.widths[level],
												   (GLsizei)def.heights[level],
												   0,
												   (GLsizei)def.byteLens[level],
												   def.datas[level]);
							sge_CHECK_GL_ERROR();
						}
					}break;
					case TextureCompressionMode::ETC1:
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(def.mipLevels));
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)onS);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)onT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
						sge_CHECK_GL_ERROR();
						for(uint32_t level = 0; level < def.mipLevels + 1; level++)
						{
							glCompressedTexImage2D(GL_TEXTURE_2D,
												   (GLint)level,
												   (GLenum)def.compression,
												   (GLsizei)def.widths[level],
												   (GLsizei)def.heights[level],
												   0,
												   (GLsizei)def.byteLens[level],
												   def.datas[level]);
							sge_CHECK_GL_ERROR();
						}
					}break;
					case TextureCompressionMode::ETC2:
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(def.mipLevels));
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)onS);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)onT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
						sge_CHECK_GL_ERROR();
						for(uint32_t level = 0; level < def.mipLevels + 1; level++)
						{
							glCompressedTexImage2D(GL_TEXTURE_2D,
												   (GLint)level,
												   (GLenum)def.compression,
												   (GLsizei)def.widths[level],
												   (GLsizei)def.heights[level],
												   0,
												   (GLsizei)def.byteLens[level],
												   def.datas[level]);
							sge_CHECK_GL_ERROR();
						}
					}break;
					default:
					{
						sge_ERROR("Unexpected compression algorithm retireved from texture definition!");
					}break;
				}
			}
			else
			{
				assert(def.format == ColorFormat::RGBA_B8); // Supporting only uint8_t type for each color channel in RGBA.
				if(minifyingMode > SamplingMode::NEAREST) assert(def.mipLevels > 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(def.mipLevels));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)onS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)onT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
				glTexStorage2D(GL_TEXTURE_2D, (GLsizei)def.mipLevels, GL_RGBA8, (GLsizei)def.widths[0], (GLsizei)def.heights[0]);
				sge_CHECK_GL_ERROR();
				for(uint32_t level = 0; level < def.mipLevels + 1; level++)
				{
					glTexSubImage2D
					(
						GL_TEXTURE_2D,
						(GLint)level,
						0,
						0,
						(GLsizei)def.widths[level],
						(GLsizei)def.heights[level],
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						def.datas[level]
					);
					sge_CHECK_GL_ERROR();
				}
			}
		}
		for(uint32_t i = 0; i < def.datas.size(); i++)
		{
			delete[] def.datas[i];
		}
	}
	void Texture::Destroy_()
	{
		assert(IsValid());
		glDeleteTextures(1, &TEX);
	}
	void Texture::UpdateData(const void* const data) const
	{
		assert(IsValid());
		assert(mutability == Mutability::DYNAMIC);
		glBindTexture(GL_TEXTURE_2D, TEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)widths[0], (GLsizei)heights[0], 0, GL_RED, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)onS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)onT);
	}
	void Texture::Bind(const uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + (GLenum)textureUnit);
		glBindTexture(GL_TEXTURE_2D, TEX);
	}
}//!sge