#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <vector>
#include <utility>

#include "globals.h"
#include "ResourcesAbstracts.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Engine.h"

namespace sge
{
	struct IndexedMesh
	{
		struct Definition
		{
			using ElementSize = uint32_t;
			using ComponentSize = uint32_t;

			std::vector<std::tuple<VertexBuffer::Definition, ElementSize, ComponentSize>> bufferDefinitions;
		};

		uint32_t VAO = 0;
		Handle<VertexBuffer> indexVBO = {};
		std::vector<Handle<VertexBuffer>> vertexBuffers = {};
		std::vector<Handle<Texture>> textures = {};

		void Init(const Definition& def)
		{
			auto& renderer = Engine::Get().GetRenderer();

			const uint32_t nrOfBuffers = def.bufferDefinitions.size();
			for(uint32_t i = 0; i < nrOfBuffers; i++)
			{
				def.bufferDefinitions[i].
				renderer.CreateVertexBuffer();
			}
		}
		void Update()
		{
			
		}
		void Destroy()
		{

		}

		inline bool IsValid() const
		{
			return VAO && vertexBuffers.size() > 0;
		}
		inline void Reset()
		{
			*this = {};
		}
	};
}//!sge