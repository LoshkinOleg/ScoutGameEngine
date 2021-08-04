#pragma once

#include <vector>
#include <map>

namespace sge
{
	class Renderer
	{
		using GpuLocation_VAO_ = uint32_t;
		using GpuLocation_VBO_ = uint32_t;
		using GpuLocation_EBO_ = uint32_t;
		using GpuLocation_TEX_ = uint32_t;
		using GpuLocation_PROGRAM_ = uint32_t;
		using UniformLocation_ = uint32_t;
		using Hash_ = uint32_t;

		std::vector<GpuLocation_VAO_> VAOs_ = {};
		std::vector<GpuLocation_VBO_> VBOs_ = {};
		std::vector<GpuLocation_EBO_> EBOs_ = {};
		std::vector<GpuLocation_TEX_> TEXs_ = {};
		std::vector<GpuLocation_PROGRAM_> PROGRAMs_ = {};
		std::map<Hash_, UniformLocation_> uniformsCache_ = {};

	public:
		void Shutdown()
		{

		}
	};
}//!sge
