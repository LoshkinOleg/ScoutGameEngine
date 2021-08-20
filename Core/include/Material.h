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

		void Bind() const;

		bool IsValid() const override;

	private:
		std::vector<HashableHandle<Texture>> textures_ = {};
		std::vector<glm::vec3> vec3s_ = {};
		std::vector<float> floats_ = {};
		ShadingMode shadingMode_ = ShadingMode::INVALID;

		void Init_(const Definition& def);
	};
}//!sge