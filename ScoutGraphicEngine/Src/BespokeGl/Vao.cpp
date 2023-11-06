#include <Scout/OpenGL33/VAO_OpenGL33.h>

#include <glad/glad.h>

#include <Scout/OpenGL33/Utils_OpenGL33.h>

namespace Scout
{
	VAO_OpenGL33::VAO_OpenGL33(VAO_OpenGL33&& other)
	{
		vaoGl_ = other.vaoGl_;
		other.freeVramOnDestruction_ = false;
	}

	VAO_OpenGL33& VAO_OpenGL33::operator=(VAO_OpenGL33&& other)
	{
		vaoGl_ = other.vaoGl_;
		other.freeVramOnDestruction_ = false;
		return *this;
	}

	VAO_OpenGL33::VAO_OpenGL33()
	{
		glGenVertexArrays(1, &vaoGl_);
		CheckGlErrors();
	}

	VAO_OpenGL33::~VAO_OpenGL33()
	{
		if (freeVramOnDestruction_) glDeleteVertexArrays(1, &vaoGl_);
	}

	void VAO_OpenGL33::Bind() const
	{
		glBindVertexArray(vaoGl_);
		CheckGlErrors();
	}

	void VAO_OpenGL33::Unbind()
	{
		glBindVertexArray(0);
		CheckGlErrors();
	}
}