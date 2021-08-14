#include "Shader.h"

#include <map>
#include <iostream>

#include <glad/glad.h>

#include "macros.h"

namespace sge
{
	void Shader::Init(const std::string_view vertexSrc, const std::string_view fragmentSrc, const std::string_view geometrySrc, const ShadingMode illum, const uint32_t primitive)
	{
		this->primitive = primitive;

		uint32_t VERT = 0, FRAG = 0, GEO = 0;
		int32_t success = false;
		const bool usingGeometryShader = !geometrySrc.empty();

		assert(
			(vertexSrc != "" && vertexSrc != "\0") &&
			(fragmentSrc != "" && fragmentSrc != "\0"));
		if(usingGeometryShader) assert(geometrySrc != "\0");

		const char* vertSrc = vertexSrc.data();
		const char* fragSrc = fragmentSrc.data();
		const char* geoSrc = geometrySrc.data();

		VERT = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(VERT, 1, &vertSrc, NULL);
		sge_CHECK_GL_ERROR();
		glCompileShader(VERT);
		sge_CHECK_GL_ERROR();
		glGetShaderiv(VERT, GL_COMPILE_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if(!success) { sge_ERROR("Error compiling the vertex shader!"); }

		FRAG = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(FRAG, 1, &fragSrc, NULL);
		sge_CHECK_GL_ERROR();
		glCompileShader(FRAG);
		sge_CHECK_GL_ERROR();
		glGetShaderiv(FRAG, GL_COMPILE_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if(!success) { sge_ERROR("Error compiling the fragment shader!"); }

		if(usingGeometryShader)
		{
			GEO = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(GEO, 1, &geoSrc, NULL);
			sge_CHECK_GL_ERROR();
			glCompileShader(GEO);
			sge_CHECK_GL_ERROR();
			glGetShaderiv(GEO, GL_COMPILE_STATUS, &success);
			sge_CHECK_GL_ERROR();
			if(!success) { sge_ERROR("Error compiling the geometry shader!"); }
		}

		PROGRAM = glCreateProgram();
		glAttachShader(PROGRAM, VERT);
		glAttachShader(PROGRAM, FRAG);
		if(usingGeometryShader)
		{
			glAttachShader(PROGRAM, GEO);
		}
		sge_CHECK_GL_ERROR();
		glLinkProgram(PROGRAM);
		sge_CHECK_GL_ERROR();
		glGetProgramiv(PROGRAM, GL_LINK_STATUS, &success);
		sge_CHECK_GL_ERROR();
		if(!success) { sge_ERROR("Error linking shaders!"); }

		glDeleteShader(VERT);
		glDeleteShader(FRAG);
		if(usingGeometryShader)
		{
			glDeleteShader(GEO);
		}
		sge_CHECK_GL_ERROR();
	}
	int32_t Shader::GetUniformLocation(const std::string_view name)
	{
		const Hash hash = Hash(name.data(), (uint32_t)name.size(), 0);
		const auto match = uniformLocationCache.find(hash);
		if(match != uniformLocationCache.end()) // Name of uniform already known, use it.
		{
			return match->second;
		}
		else
		{
			const int location = glGetUniformLocation(PROGRAM, name.data());
			sge_CHECK_GL_ERROR();
			if(location < 0) { sge_ERROR("Trying to get the location of a non existent uniform!"); }
			uniformLocationCache.insert({ hash, location }); // Add the new entry.
			return location;
		}
	}
	void Shader::SetInt(const std::string_view name, const int32_t value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform1i(location, value);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetFloat(const std::string_view name, const float value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform1f(location, value);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetVec3(const std::string_view name, const glm::vec3 value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniform3fv(location, 1, &value[0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetMat4(const std::string_view name, const glm::mat4& value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::SetMat3(const std::string_view name, const glm::mat3& value)
	{
		glUseProgram(PROGRAM);
		const int location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
		sge_CHECK_GL_ERROR();
	}
	void Shader::Bind() const
	{
		if(IsValid())
		{
			glUseProgram(PROGRAM);
		}
		else
		{
			sge_ERROR("Trying to use invalid shader!");
		}
	}
}//!sge