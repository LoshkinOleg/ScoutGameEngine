#pragma once

#include <map>

#include "VertexBuffer.h"
#include "Material.h"

namespace sge
{
	class TransformsBuffer;

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
			Mutability mutability = Mutability::INVALID;
			ShadingMode shadingModes = ShadingMode::INVALID; // Here for sanity check during initialization of mesh.

			Hash ComputeHash() const;

			bool IsValid() const override;
		};

		bool IsValid() const override;

	private:
		uint32_t VAO_ = 0;
		HashableHandle<VertexBuffer> indexVBO_ = {};
		std::vector<HashableHandle<VertexBuffer>> vertexBuffers_ = {};
		std::map<const ShadingMode, const UniqueResourceHandle<Material>> materials_ = {}; // at most 1 per shading mode
		Mutability mutability_ = Mutability::INVALID;
		uint32_t nrOfVertices_ = 0;
		float radius_ = 0.0f;

		void Init_(const Definition& def);
		void Draw_(const HashlessHandle<TransformsBuffer>& transforms, const DrawingPrimitive primitive, const ShadingMode mode);
	};

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
			Mutability mutability = Mutability::INVALID;
			ShadingMode shadingModes = ShadingMode::INVALID; // Here for sanity check during initialization of mesh.

			Hash ComputeHash() const;

			bool IsValid() const override;
		};

		bool IsValid() const override;

	private:
		uint32_t VAO_ = 0;
		std::vector<HashableHandle<VertexBuffer>> vertexBuffers_ = {};
		std::map<ShadingMode, HashableHandle<Material>> materials_ = {}; // at most 1 per shading mode
		Mutability mutability_ = Mutability::INVALID;
		uint32_t nrOfVertices_ = 0;
		float radius_ = 0.0f;

		void Init_(const Definition& def);
		void Draw_(const HashlessHandle<TransformsBuffer>& transforms, const DrawingPrimitive primitive, const ShadingMode mode);
		void UpdateRadius_();
	};
}//!sge