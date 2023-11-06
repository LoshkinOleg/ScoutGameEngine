#include <Scout/OpenGL33/Texture_OpenGL33.h>

#include <Scout/OpenGL33/Utils_OpenGL33.h>

namespace Scout
{
	Texture_OpenGL33::Texture_OpenGL33(Texture_OpenGL33&& other)
	{
		texGl_ = other.texGl_;
		other.freeVramOnDestruction_ = false;
	}

	Texture_OpenGL33& Texture_OpenGL33::operator=(Texture_OpenGL33&& other)
	{
		texGl_ = other.texGl_;
		other.freeVramOnDestruction_ = false;
		return *this;
	}

	Texture_OpenGL33::Texture_OpenGL33(
		const void* const uncompressedData, const std::uint64_t byteLen,
		const std::uint64_t width, const std::uint64_t height,
		const TextureSlot_OpenGL33 texType):
			textureSlot_(texType)
	{
		glGenTextures(1, &texGl_);
		CheckGlErrors();
		glBindTexture(GL_TEXTURE_2D, (GLuint)texGl_);
		CheckGlErrors();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		CheckGlErrors();

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			(GLsizei)width,
			(GLsizei)height,
			0,
			GL_RGBA,
			GL_UNSIGNED_INT_8_8_8_8,
			uncompressedData);
		CheckGlErrors();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture_OpenGL33::~Texture_OpenGL33()
	{
		if (freeVramOnDestruction_)
		{
			glDeleteTextures(1, &texGl_);
			CheckGlErrors();
		}
	}

	void Texture_OpenGL33::Bind() const
	{
		glActiveTexture((GLenum)textureSlot_);
		glBindTexture(GL_TEXTURE_2D, (GLuint)texGl_);
		CheckGlErrors();
	}

	void Texture_OpenGL33::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}