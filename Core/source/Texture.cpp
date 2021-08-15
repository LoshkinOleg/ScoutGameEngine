#include "Texture.h"

#include <glad/glad.h>

#include "macros.h"

namespace sge
{
	void Texture::Init_(const Definition& def)
	{
		assert(def.IsValid());

		widths = def.widths;
		heights = def.heights;
		mutability = def.mutability;
		minifyingMode = def.minifyingMode;
		magnifyingMode = def.magnifyingMode;
		wrappingModeS = def.onS;
		wrappingModeT = def.onT;

		if(def.generateMipMaps) sge_ERROR("Implement generation of mip maps!");

		glGenTextures(1, &TEX);
		glBindTexture(GL_TEXTURE_2D, TEX);

		if(mutability == Mutability::DYNAMIC)
		{
			sge_CHECK_GL_ERROR();
			assert(def.format == Format::RED_F32); // Supporting only single channel float textures for dynamic ones.
			assert(!def.mipLevels && !def.generateMipMaps); // Not supporting mip mapping for dynamic textures.
			assert(def.widths.size() == 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)widths[0], (GLsizei)heights[0], 0, GL_RED, GL_FLOAT, def.datas[0]);
			sge_CHECK_GL_ERROR();
			assert((uint32_t)minifyingMode < GL_LINEAR_MIPMAP_NEAREST && (uint32_t)magnifyingMode < GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrappingModeS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrappingModeT);
			sge_CHECK_GL_ERROR();
		}
		else
		{
			// TODO: add support for compressed textures.

			assert(def.format == Format::RGBA_B8); // Supporting only uint8_t type for each color channel in RGBA.
			if(minifyingMode > SamplingMode::NEAREST) assert(def.mipLevels > 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(def.mipLevels));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrappingModeS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrappingModeT);
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
					(GLint)def.widths[level],
					(GLint)def.heights[level],
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					def.datas[level]
				);
				sge_CHECK_GL_ERROR();
			}
		}
	}
	uint32_t Texture::Definition::ByteSize(const uint32_t mipLevel) const
	{
		size_t sizeOfElement = 0;
		switch(format)
		{
			case sge::Texture::Format::RED_F32:
			{
				sizeOfElement = sizeof(float);
			}break;
			case sge::Texture::Format::RGBA_B8:
			{
				sizeOfElement = sizeof(char) * 4;
			}break;
			default:
			{
				sge_ERROR("Unexpected pixel format!");
			}break;
		}
		const size_t width = (size_t)widths[mipLevel];
		const size_t height = (size_t)heights[mipLevel];
		return (uint32_t)(width * height * sizeOfElement);
	}
	void Texture::Destroy_()
	{
		if(IsValid())
		{
			glDeleteTextures(1, &TEX);
			Reset();
		}
		else
		{
			sge_WARNING("Attempting to delete an invalid Texture!");
		}
	}
	void Texture::UpdateData(const void* const data) const
	{
		assert(IsValid());
		assert(mutability == Mutability::DYNAMIC && widths.size() == 1);
		// Note: if using dynamic mutability, mipmaps aren't used.
		glBindTexture(GL_TEXTURE_2D, TEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)widths[0], (GLsizei)heights[0], 0, GL_RED, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrappingModeS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrappingModeT);
	}
	void Texture::Bind(const uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + (GLenum)textureUnit);
		glBindTexture(GL_TEXTURE_2D, TEX);
	}
}//!sge