#pragma once

#include "Texture.h"
#include "Resources.h"

namespace sge
{
	struct Material
	{
		struct Definition
		{
			std::vector<Texture::Definition> texDefs;
			std::vector<glm::vec3> vec3s = {};
			std::vector<float> floats = {};
			ShadingMode shadingMode = ShadingMode::INVALID;

			bool IsValid() const;
			Hash ComputeHash() const;
		};

		std::vector<Handle<Texture>> textures = {};
		std::vector<glm::vec3> vec3s = {};
		std::vector<float> floats = {};
		ShadingMode shadingMode = ShadingMode::INVALID;

		void Bind() const;

		bool IsValid() const;

	private:
		friend class Renderer;
		void Init_(const Definition& def);
	};
}//!sge