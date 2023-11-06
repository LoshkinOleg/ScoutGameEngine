#include <Scout/OpenGL33/Shader_OpenGL33.h>

#include <stdexcept>

#include <glad/glad.h>

#include <Scout/OpenGL33/Utils_OpenGL33.h>

namespace Scout
{
    Shader_OpenGL33::Shader_OpenGL33(Shader_OpenGL33&& other) noexcept
    {
        programGl_ = other.programGl_;
        other.freeVramOnDestruction_ = false;
    }

    Shader_OpenGL33& Shader_OpenGL33::operator=(Shader_OpenGL33&& other) noexcept
    {
        programGl_ = other.programGl_;
        other.freeVramOnDestruction_ = false;
        return *this;
    }

    Shader_OpenGL33::Shader_OpenGL33(
        const std::string_view vertShaderSrc,
        const std::string_view fragShaderSrc)
    {
        const char* const vertSrc_cStr = vertShaderSrc.data();
        const GLint byteLenVertSrc = (GLint)vertShaderSrc.size();
        const char* const fragSrc_cStr = fragShaderSrc.data();
        const GLint byteLenFragSrc = (GLint)fragShaderSrc.size();

        GLint success = 0;

        const std::uint32_t vertGl = glCreateShader(GL_VERTEX_SHADER);
        CheckGlErrors();
        glShaderSource(vertGl, 1, &vertSrc_cStr, &byteLenVertSrc);
        CheckGlErrors();
        glCompileShader(vertGl);
        CheckGlErrors();
        glGetShaderiv(vertGl, GL_COMPILE_STATUS, &success);
        CheckGlErrors();
        if (!success) throw std::runtime_error("Failed to compile vertex shader.");

        const std::uint32_t fragGl = glCreateShader(GL_FRAGMENT_SHADER);
        CheckGlErrors();
        glShaderSource(fragGl, 1, &fragSrc_cStr, &byteLenFragSrc);
        CheckGlErrors();
        glCompileShader(fragGl);
        CheckGlErrors();
        glGetShaderiv(fragGl, GL_COMPILE_STATUS, &success);
        CheckGlErrors();
        if (!success) throw std::runtime_error("Failed to compile fragment shader.");

        programGl_ = glCreateProgram();
        CheckGlErrors();
        glAttachShader(programGl_, vertGl);
        CheckGlErrors();
        glAttachShader(programGl_, fragGl);
        CheckGlErrors();
        glLinkProgram(programGl_);
        CheckGlErrors();
        glGetProgramiv(programGl_, GL_LINK_STATUS, &success);
        CheckGlErrors();
        if (!success) throw std::runtime_error("Failed to link rendering program.");

        glDeleteShader(vertGl);
        CheckGlErrors();
        glDeleteShader(fragGl);
        CheckGlErrors();
    }

    Shader_OpenGL33::~Shader_OpenGL33()
    {
        glDeleteProgram(programGl_);
        CheckGlErrors();
    }

    void Shader_OpenGL33::Bind() const
    {
        glUseProgram(programGl_);
        CheckGlErrors();
    }

    void Shader_OpenGL33::Unbind()
    {
        glUseProgram(0);
    }

    void Shader_OpenGL33::SetShaderParam(
        const std::string_view name, const std::vector<float> value)
    {
        Bind();
        const auto location = glGetUniformLocation(programGl_, name.data());
        CheckGlErrors();
        glUniform1fv(location, (GLsizei)value.size(), value.data());
        CheckGlErrors();
        Unbind();
    }
}