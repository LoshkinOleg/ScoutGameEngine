#include <Scout/OpenGL33/GraphicsEngine_OpenGL33.h>

#include <stdexcept>
#include <fstream>

#include <glad/glad.h>

#include <Scout/OpenGL33/Utils_OpenGL33.h>
#include <Scout/StringUtils.h>

namespace Scout
{
	GraphicsEngine_OpenGL33* GraphicsEngine_OpenGL33::self_ = nullptr;

	GraphicsEngine_OpenGL33::GraphicsEngine_OpenGL33(
		GLADloadproc proc,
		const bool depthTest,
		const bool cullFace, const bool cullBackface, const bool frontfaceWindingIsCCW,
		const bool alphaBlend, const AlphaBlending sFactor, const AlphaBlending dFactor,
		const std::vector<ShadingType_OpenGL33> shadingTypes, const std::vector<std::string> vertSources, const std::vector<std::string> fragSources)
	{
		self_ = this;

		if (!gladLoadGLLoader(proc)) throw std::runtime_error("Unable to lead OpenGL33 with GLAD.");

		glClearColor(CLEAR_COLOR_[0], CLEAR_COLOR_[1], CLEAR_COLOR_[2], CLEAR_COLOR_[3]);
		CheckGlErrors();

		if (depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		CheckGlErrors();

		if (cullFace)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(cullBackface ? GL_BACK : GL_FRONT);
			glFrontFace(frontfaceWindingIsCCW ? GL_CW : GL_CCW);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
		CheckGlErrors();

		if (alphaBlend)
		{
			glEnable(GL_BLEND);
			ErrorCode err = ErrorCode::NONE;
			const auto sFactorGl = (GLenum)ToOpenGL33_Enum(sFactor, &err);
			if (err != ErrorCode::NONE) throw std::runtime_error("Failed to convert AlphaBlending value to GLenum value.");
			const auto dFactorGl = (GLenum)ToOpenGL33_Enum(dFactor, &err);
			if (err != ErrorCode::NONE) throw std::runtime_error("Failed to convert AlphaBlending value to GLenum value.");
			glBlendFunc(sFactorGl, dFactorGl);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		CheckGlErrors();

		// Create shaders.
		if (shadingTypes.size() != vertSources.size() != fragSources.size()) throw std::runtime_error("Mismatching shaders passed in shaderSources.");
		for (size_t i = 0; i < shadingTypes.size(); i++)
		{
			const auto& key = shadingTypes[i];
			const auto& vertSrc = vertSources[i];
			const auto& fragSrc = fragSources[i];

			const auto handle = MakeShader_(vertSrc, fragSrc);
			if (shaderHandles_.find(key) != shaderHandles_.end()) throw std::runtime_error("Multiple shader sources defined for the same shading type.");
			shaderHandles_.insert({ key, handle });
		}
	}

	ErrorCode GraphicsEngine_OpenGL33::Schedule(const ModelHandle modelId)
	{
		if (modelId == ModelHandle::INVALID_ID) return ErrorCode::INVALID_ID;
		scheduled_.push_back(modelId);
		return ErrorCode::NONE;
	}

	ErrorCode GraphicsEngine_OpenGL33::Unschedule(const ModelHandle modelId)
	{
		const auto element = std::find(scheduled_.begin(), scheduled_.end(), modelId);
		if (element == scheduled_.end()) return ErrorCode::INVALID_ID;
		scheduled_.erase(element);
		return ErrorCode::NONE;
	}

	ErrorCode GraphicsEngine_OpenGL33::Update()
	{
		glClear(CLEAR_FLAGS_GL_);

		// TODO: update model transforms on gpu

		// TODO: update view matrix on gpu

		for (auto& model : scheduled_)
		{
			models_[model].Draw();
		}

		// TODO
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}

	ErrorCode GraphicsEngine_OpenGL33::UploadModelMatrices_() const
	{
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}
	ErrorCode GraphicsEngine_OpenGL33::UploadViewMatrix_() const
	{
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}

	ModelHandle GraphicsEngine_OpenGL33::MakeModel(const ModelDef def, ErrorCode* outError)
	{
		if (outError) *outError = ErrorCode::FEATURE_NOT_IMPLEMENTED;

		return ModelHandle::INVALID_ID;
	}

	ErrorCode GraphicsEngine_OpenGL33::DrawPoint(
		const float xPos, const float yPos,
		const float thickness,
		const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a)
	{

		// TODO
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}

	ErrorCode GraphicsEngine_OpenGL33::DrawLine(
		const float xPos0, const float yPos0,
		const float xPos1, const float yPos1,
		const float thickness,
		const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a)
	{
		// TODO
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}

	ErrorCode GraphicsEngine_OpenGL33::DrawTriangle(
		const float xPos0, const float yPos0,
		const float xPos1, const float yPos1,
		const float xPos2, const float yPos2,
		const float thickness,
		const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a,
		const bool filled)
	{
		// TODO
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}

	ErrorCode GraphicsEngine_OpenGL33::DrawCircle(
		const float xPos, const float yPos,
		const float radius,
		const float thickness,
		const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a,
		const bool filled)
	{
		// TODO
		return ErrorCode::FEATURE_NOT_IMPLEMENTED;
	}

	GraphicsEngine_OpenGL33& GraphicsEngine_OpenGL33::Get()
	{
		return *self_;
	}

	ShaderHandle GraphicsEngine_OpenGL33::MakeShader_(const std::string_view vertSrc, const std::string_view fragSrc)
	{
		shaders_.emplace_back(Shader_OpenGL33(vertSrc, fragSrc));
		return shaders_.size();
	}

	VaoHandle GraphicsEngine_OpenGL33::MakeVAO_()
	{
		vao_.emplace_back(VAO_OpenGL33());
		return vao_.size();
	}

	VboHandle GraphicsEngine_OpenGL33::MakeVBO_(
		const VaoHandle vao,
		const void* const data, const std::uint64_t byteLen,
		const std::uint32_t componentsPerElement, const std::uint32_t byteSizeOfComponent,
		const BufferContents contentsType)
	{
		vbo_.emplace_back(VBO_OpenGL33(vao, data, byteLen, componentsPerElement, byteSizeOfComponent, contentsType));
		return vbo_.size();
	}

	EboHandle GraphicsEngine_OpenGL33::MakeEBO_(
		const VaoHandle vao,
		const void* const data, const std::uint64_t byteLen,
		const std::uint32_t byteSizeOfIndex)
	{
		ebo_.emplace_back(EBO_OpenGL33(vao, data, byteLen, byteSizeOfIndex));
		return ebo_.size();
	}

	TextureHandle GraphicsEngine_OpenGL33::MakeTexture_(
		const void* const uncompressedData, const std::uint64_t byteLen,
		const std::uint64_t width, const std::uint64_t height,
		const TextureSlot_OpenGL33 texType)
	{
		textures_.emplace_back(Texture_OpenGL33(uncompressedData, byteLen, width, height, texType));
		return textures_.size();
	}

	MaterialHandle GraphicsEngine_OpenGL33::MakeMaterial_(
		const ShaderHandle shader,
		const std::vector<TextureHandle> textures,
		const std::vector<float> fShaderParams)
	{
		materials_.emplace_back(Material_OpenGL33(shader, textures, fShaderParams));
		return materials_.size();
	}

	MeshHandle GraphicsEngine_OpenGL33::MakeMesh_(
		const VaoHandle vao, const VboHandle ebo,
		const std::vector<VboHandle> vertexBuffers,
		const MaterialHandle mat)
	{
		meshes_.emplace_back(Mesh_OpenGL33(vao, ebo, vertexBuffers, mat));
		return meshes_.size();
	}
}