#pragma once

#include <cstdint>

#include <Scout/PreprocessorMacros.h>

namespace Scout
{
    // TODO: align
    class VAO_OpenGL33
    {
    public:
        SCOUT_NO_COPY(VAO_OpenGL33);
        VAO_OpenGL33(VAO_OpenGL33&&);
        VAO_OpenGL33& operator=(VAO_OpenGL33&&);

        VAO_OpenGL33();
        ~VAO_OpenGL33();

        void Bind() const;
        static void Unbind();

    private:
        std::uint32_t vaoGl_ = 0;
        bool freeVramOnDestruction_ = true;
    };
} // namespace Scout