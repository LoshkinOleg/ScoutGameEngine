#pragma once

#include "Texture.h"
#include "Resources.h"

namespace sge
{
	class Material: public I_Validateable
	{
		friend class Renderer;

	public:
		class Definition: public I_Validateable
		{
		public:
			std::vector<Texture::Definition> texDefs;
			std::vector<glm::vec3> vec3s = {};
			std::vector<float> floats = {};
			ShadingMode shadingMode = ShadingMode::INVALID;

			Hash ComputeHash() const;

			bool IsValid() const override;
		};

		std::vector<UniqueResourceHandle<Texture>> textures = {};
		std::vector<glm::vec3> vec3s = {};
		std::vector<float> floats = {};
		ShadingMode shadingMode = ShadingMode::INVALID;

		void Bind() const;

		bool IsValid() const override;

	private:
		void Init_(const Definition& def);
	};
}//!sge