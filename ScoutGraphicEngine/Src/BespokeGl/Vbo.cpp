#include <Scout/OpenGL33/VBO_OpenGL33.h>

#include <Scout/OpenGL33/Utils_OpenGL33.h>
#include <Scout/OpenGL33/GraphicsEngine_OpenGL33.h>

namespace Scout
{
    VBO_OpenGL33::VBO_OpenGL33(VBO_OpenGL33&& other) noexcept
    {
        vao_ = other.vao_;
        vboGl_ = other.vboGl_;
        other.freeVramOnDestruction_ = false;
    }

    VBO_OpenGL33& VBO_OpenGL33::operator=(VBO_OpenGL33&& other) noexcept
    {
        vao_ = other.vao_;
        vboGl_ = other.vboGl_;
        other.freeVramOnDestruction_ = false;
        return *this;
    }

    VBO_OpenGL33::VBO_OpenGL33(
        const VaoHandle vao,
        const void* const data, const std::uint64_t byteLen,
        const std::uint32_t componentsPerElement, const std::uint32_t byteSizeOfComponent,
        const BufferContents contentsType)
    {
        // TODO: ensure componentsPerElement and byteSizeOfComponent matches the expectations (check enum values of BufferContents_OpenGL33)

        GraphicsEngine_OpenGL33::Get()[vao_].Bind();
        
        glGenBuffers(1, &vboGl_);
        CheckGlErrors();
        glBindBuffer(GL_ARRAY_BUFFER, vboGl_);
        CheckGlErrors();
        glBufferData(
            GL_ARRAY_BUFFER,
            (GLsizeiptr)byteLen, (void*)data,
            GL_STATIC_DRAW); // TODO: add support for mutable VBOs
        CheckGlErrors();

        ErrorCode err = ErrorCode::NONE;
        const auto location = ToOpenGL33_Enum(contentsType, &err);
        if (err != ErrorCode::NONE) throw std::runtime_error("Constructor of VBO_OpenGL33 got passed an invalid contentsType.");
        glVertexAttribPointer
        (
            (GLuint)location,
            (GLint)componentsPerElement,
            GL_FLOAT,
            GL_FALSE,
            (GLsizei)(componentsPerElement * byteSizeOfComponent),
            (void*)0
        );
        glEnableVertexAttribArray((GLuint)location);
        CheckGlErrors();

        Unbind();
    }
    VBO_OpenGL33::~VBO_OpenGL33()
    {
        if (freeVramOnDestruction_)
        {
            glDeleteBuffers(1, &vboGl_);
            CheckGlErrors();
        }
    }
    void VBO_OpenGL33::Bind() const
    {
        GraphicsEngine_OpenGL33::Get()[vao_].Bind();
        glBindBuffer(GL_ARRAY_BUFFER, vboGl_);
        CheckGlErrors();
    }
    void VBO_OpenGL33::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        CheckGlErrors();
    }
}