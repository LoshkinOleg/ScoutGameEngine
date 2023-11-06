/*
    Fake immediate mode implementation has been inspired by amengol's MinGL implementation: https://github.com/amengol/MinGL 
*/

#pragma once

#include <glad/glad.h>
#include <glm.hpp>

#include <Scout/IGraphicsEngine.h>
#include <Scout/PreprocessorMacros.h>
#include <Scout/TypedefsAndEnums.h>
#include <Scout/Path.h>
#include <Scout/OpenGL33/VAO_OpenGL33.h>
#include <Scout/OpenGL33/VBO_OpenGL33.h>
#include <Scout/OpenGL33/EBO_OpenGL33.h>
#include <Scout/OpenGL33/Texture_OpenGL33.h>
#include <Scout/OpenGL33/Shader_OpenGL33.h>
#include <Scout/OpenGL33/Material_OpenGL33.h>
#include <Scout/OpenGL33/Mesh_OpenGL33.h>
#include <Scout/OpenGL33/Model_OpenGL33.h>

namespace Scout
{
    struct ModelDef
    {
        std::vector<std::uint32_t> indices{};
        std::vector<glm::vec3> positions{};
        std::vector<glm::vec2> uvs{};
        std::vector<glm::vec3> normals{};
        std::vector<glm::vec3> tangents{};

        std::vector<void*> texturesData{};
        std::vector<TextureColorFormat> texturesLayout{};

        ShadingType shader{};

        std::vector<std::string> shaderParamsNames{};
        std::vector<ShaderParamSizes> paramsLayout{};
        void* interleavedShaderParams = nullptr;
    };

    /*
        DON'T INSTANCIATE IT YOURSELF! USE MakeGraphicsEngine() DEFINED IN IGraphicsEngine.h INSTEAD!
        
        TODO:
        - implement use of ModelDef,
        - implement common model matrices buffer,
        - implement view matrix buffer,
        - implement fake immediate mode
        - find way to make this class visible only to factory function
    */
    class GraphicsEngine_OpenGL33 final: public IGraphicsEngine
    {
    public:
        GraphicsEngine_OpenGL33() = delete;
        ~GraphicsEngine_OpenGL33() = default;
        SCOUT_NO_COPY(GraphicsEngine_OpenGL33);
        SCOUT_NO_MOVE(GraphicsEngine_OpenGL33);

        GraphicsEngine_OpenGL33(
            GLADloadproc proc,
            const bool depthTest,
            const bool cullFace, const bool cullBackface, const bool frontfaceWindingIsCCW,
            const bool alphaBlend, const AlphaBlending sFactor, const AlphaBlending dFactor,
            const std::vector<ShadingType_OpenGL33> shadingTypes, const std::vector<std::string> vertSources, const std::vector<std::string> fragSources);

        ErrorCode Schedule(const ModelHandle modelId) override;
        ErrorCode Unschedule(const ModelHandle modelId) override;
        ErrorCode Update() override;
        /*
        NOT YET IMPLEMENTED
        */
        ErrorCode DrawPoint(
            const float xPos, const float yPos,
            const float thickness,
            const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) override;
        /*
        NOT YET IMPLEMENTED
        */
        ErrorCode DrawLine(
            const float xPos0, const float yPos0,
            const float xPos1, const float yPos1,
            const float thickness,
            const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) override;
        /*
        NOT YET IMPLEMENTED
        */
        ErrorCode DrawTriangle(
            const float xPos0, const float yPos0,
            const float xPos1, const float yPos1,
            const float xPos2, const float yPos2,
            const float thickness,
            const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a,
            const bool filled) override;
        /*
        NOT YET IMPLEMENTED
        */
        ErrorCode DrawCircle(
            const float xPos, const float yPos,
            const float radius,
            const float thickness,
            const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a,
            const bool filled) override;
        
        ModelHandle MakeModel(const ModelDef def, ErrorCode* outError = nullptr);

        static GraphicsEngine_OpenGL33& Get(); // Get rid of this sheit

        inline VAO_OpenGL33& operator[](const VaoHandle handle)
        {
            return vao_[handle];
        }
        inline VBO_OpenGL33& operator[](const VboHandle handle)
        {
            return vbo_[handle];
        }
        inline EBO_OpenGL33& operator[](const EboHandle handle)
        {
            return ebo_[handle];
        }
        inline Texture_OpenGL33& operator[](const TextureHandle handle)
        {
            return textures_[handle];
        }
        inline Shader_OpenGL33& operator[](const ShaderHandle handle)
        {
            return shaders_[handle];
        }
        inline Material_OpenGL33& operator[](const MaterialHandle handle)
        {
            return materials_[handle];
        }
        inline Mesh_OpenGL33& operator[](const MeshHandle handle)
        {
            return meshes_[handle];
        }
        inline Model_OpenGL33& operator[](const ModelHandle handle)
        {
            return models_[handle];
        }
    private:
        ErrorCode UploadModelMatrices_() const;
        ErrorCode UploadViewMatrix_() const;
        VaoHandle MakeVAO_();
        VboHandle MakeVBO_(
            const VaoHandle vao,
            const void* const data, const std::uint64_t byteLen,
            const std::uint32_t componentsPerElement, const std::uint32_t byteSizeOfComponent,
            const BufferContents contentsType);
        EboHandle MakeEBO_(
            const VaoHandle vao,
            const void* const data, const std::uint64_t byteLen,
            const std::uint32_t byteSizeOfIndex);
        TextureHandle MakeTexture_(
            const void* const uncompressedData, const std::uint64_t byteLen,
            const std::uint64_t width, const std::uint64_t height,
            const TextureSlot_OpenGL33 texType);
        ShaderHandle MakeShader_(
            const std::string_view vertSrc,
            const std::string_view fragSrc);
        MaterialHandle MakeMaterial_(
            const ShaderHandle shader,
            const std::vector<TextureHandle> textures,
            const std::vector<float> fShaderParams);
        MeshHandle MakeMesh_(
            const VaoHandle vao, const VboHandle ebo,
            const std::vector<VboHandle> vertexBuffers,
            const MaterialHandle mat);

    private:
        constexpr static const float CLEAR_COLOR_[4] = {0.33f, 0.0f, 0.33f, 1.0f};
        constexpr static const std::uint32_t CLEAR_FLAGS_GL_ = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
        
        VboHandle modelMatrices_ = VboHandle::INVALID_ID;

        std::unordered_map<ShadingType_OpenGL33, ShaderHandle> shaderHandles_{};

        std::vector<VAO_OpenGL33> vao_{};
        std::vector<VBO_OpenGL33> vbo_{};
        std::vector<EBO_OpenGL33> ebo_{};
        std::vector<Texture_OpenGL33> textures_{};
        std::vector<Shader_OpenGL33> shaders_{};
        std::vector<Material_OpenGL33> materials_{};
        std::vector<Mesh_OpenGL33> meshes_{};
        std::vector<Model_OpenGL33> models_{};

        std::vector<ModelHandle> scheduled_{};
        static GraphicsEngine_OpenGL33* self_; // Get rid of this sheit
    };
}