#pragma once

#include "Texture.h"
#include "Resources.h"

namespace sge
{
	class Material
	{
		friend class Renderer;

	public:
		class Definition
		{
		public:
			std::vector<Texture::Definition> texDefs;
			std::vector<glm::vec3> vec3s = {};
			std::vector<float> floats = {};
			F_ShadingMode shadingMode = F_ShadingMode::INVALID;

			Hash ComputeHash() const;

			bool IsValid() const;
		};

		void Bind() const;

		bool IsValid() const;

	private:
		std::vector<HashableHandle<Texture>> textures_ = {}; // 32 bytes
		std::vector<glm::vec3> vec3s_ = {}; // 32 bytes
		std::vector<float> floats_ = {}; // 32 bytes
		F_ShadingMode shadingMode_ = F_ShadingMode::INVALID; // 1 byte
		// 31 bytes padding

		void Init_(const Definition& def);
	};
}//!sge