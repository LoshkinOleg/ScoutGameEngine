#pragma once

#include <map>

#include "VertexBuffer.h"
#include "Material.h"

namespace sge
{
	class TransformsBuffer;

	/*
	@brief: A mesh with non-interleaved data.
	*/
	class IndexedMesh: public I_Validateable
	{
		friend class Renderer;
		friend class Model;

	public:
		class Definition: public I_Validateable
		{
		public:
			std::vector<VertexBuffer::Definition> vboDefs = {};
			VertexBuffer::Definition eboDef = {};
			std::vector<Material::Definition> matDefs = {}; // At most 1 material per shading mode.

			Hash ComputeHash() const;

			bool IsValid() const override;
		};

		uint32_t VAO = 0;
		UniqueResourceHandle<VertexBuffer> indexVBO = {};
		std::vector<UniqueResourceHandle<VertexBuffer>> vertexBuffers = {};
		std::map<const ShadingMode, const UniqueResourceHandle<Material>> materials = {}; // at most 1 per shading mode
		uint32_t nrOfVertices = 0;
		float radius = 0.0f;

		void Update(const std::vector<std::pair<void*, uint32_t>>& dataAndByteLen) const;

		bool IsValid() const override;

	private:
		void Init_(const Definition& def);
		void Draw_(const HashlessHandle<TransformsBuffer>& transforms, const uint32_t primitive, const ShadingMode mode);
	};

	// Typically used for meshes whose geometry changes during runtime.
	class InterlacedMesh: public I_Validateable
	{
		friend class Renderer;
		friend class Model;

	public:
		class Definition: public I_Validateable
		{
		public:
			std::vector<VertexBuffer::Definition> vboDefs = {};
			std::vector<Material::Definition> matDefs = {}; // At most 1 material per shading mode.

			Hash ComputeHash() const;

			bool IsValid() const override;
		};

		uint32_t VAO = 0;
		std::vector<UniqueResourceHandle<VertexBuffer>> vertexBuffers = {};
		std::map<const ShadingMode, const UniqueResourceHandle<Material>> materials = {}; // at most 1 per shading mode
		uint32_t nrOfVertices = 0; // Shouldn't change.
		float radius = 0.0f; // Must be computed at each update of data.

		bool IsValid() const override;

	private:
		void Init_(const Definition& def);
		void Draw_(const HashlessHandle<TransformsBuffer>& transforms, const uint32_t primitive, const ShadingMode mode);
		void UpdateRadius_();
	};
}//!sge