#include "Texture.h"

#include <iostream>

#include "macros.h"

namespace sge
{
	void Texture::Init(void* data,
			  const Mutability mutability,
			  const Format format,
			  const uint32_t width,
			  const uint32_t height,
			  const uint32_t mipLevels,
			  const SamplingMode minifyingMode,
			  const SamplingMode magnifyingMode,
			  const WrappingMode wrappingModeS,
			  const WrappingMode wrappingModeT)
	{
		this->width = width;
		this->height = height;
		this->mutability = mutability;
		this->minifyingMode = minifyingMode;
		this->magnifyingMode = magnifyingMode;
		this->wrappingModeS = wrappingModeS;
		this->wrappingModeT = wrappingModeT;

		glGenTextures(1, &TEX);
		glBindTexture(GL_TEXTURE_2D, TEX);

		if(mutability == Mutability::DYNAMIC)
		{
			sge_CHECK_GL_ERROR();
			assert(format == Format::RED_F32);
			assert(!mipLevels);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)width, (GLsizei)height, 0, GL_RED, GL_FLOAT, data);
			sge_CHECK_GL_ERROR();
			assert(minifyingMode < LINEAR_MIPMAP_NEAREST && magnifyingMode < LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrappingModeS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrappingModeT);
			sge_CHECK_GL_ERROR();
		}
		else
		{
			// TODO: add support for compressed textures.

			assert(format == Format::RGBA_B8);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)(mipLevels));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrappingModeS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrappingModeT);
			if(minifyingMode > SamplingMode::NEAREST) assert(mipLevels > 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
			glTexStorage2D(GL_TEXTURE_2D, (GLsizei)mipLevels, GL_RGBA8, (GLsizei)width, (GLsizei)height);
			sge_CHECK_GL_ERROR();
			for(size_t level = 0; level < mipLevels + 1; level++)
			{
				glTexSubImage2D
				(
					GL_TEXTURE_2D,
					(GLint)level,
					0,
					0,
					(GLint)width,
					(GLint)height,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					data
				);
				sge_CHECK_GL_ERROR();
			}
		}
	}
	void Texture::Update(void* data) const
	{
		assert(mutability == Mutability::DYNAMIC);

		glBindTexture(GL_TEXTURE_2D, TEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)width, (GLsizei)height, 0, GL_RED, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)minifyingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)magnifyingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)wrappingModeS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)wrappingModeT);
	}
	void Texture::Destroy()
	{
		if(IsValid())
		{
			glDeleteTextures(1, &TEX);
			Reset();
		}
		else
		{
			sge_WARNING("Trying to destroy an invalid texture!");
		}
	}
}//!sge