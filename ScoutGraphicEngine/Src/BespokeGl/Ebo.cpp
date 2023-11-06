#include <Scout/OpenGL33/EBO_OpenGL33.h>

#include <glad/glad.h>

#include <Scout/OpenGL33/Utils_OpenGL33.h>
#include <Scout/OpenGL33/GraphicsEngine_OpenGL33.h>

namespace Scout
{
	EBO_OpenGL33::EBO_OpenGL33(EBO_OpenGL33&& other)
	{
		vao_ = other.vao_;
		eboGl_ = other.eboGl_;
		other.freeVramOnDestruction_ = false;
	}

	EBO_OpenGL33& EBO_OpenGL33::operator=(EBO_OpenGL33&& other)
	{
		vao_ = other.vao_;
		eboGl_ = other.eboGl_;
		other.freeVramOnDestruction_ = false;
		return *this;
	}

	EBO_OpenGL33::EBO_OpenGL33(
		const VaoHandle vao,
		const void* const data, const std::uint64_t byteLen,
		const std::uint32_t byteSizeOfIndex):
			vao_(vao)
	{
		GraphicsEngine_OpenGL33::Get()[vao_].Bind();
		CheckGlErrors();
		glGenBuffers(1, &eboGl_);
		CheckGlErrors();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboGl_);
		CheckGlErrors();
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			(GLsizeiptr)byteLen, (void*)data,
			GL_STATIC_DRAW); // TODO: add support for mutable EBOs
		CheckGlErrors();
		Unbind();
	}

	EBO_OpenGL33::~EBO_OpenGL33()
	{
		if (freeVramOnDestruction_)
		{
			glDeleteBuffers(1, &eboGl_);
			CheckGlErrors();
		}
	}

	void EBO_OpenGL33::Bind() const
	{
		GraphicsEngine_OpenGL33::Get()[vao_].Bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboGl_);
	}

	void EBO_OpenGL33::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		CheckGlErrors();
	}
}