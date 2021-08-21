#pragma once

#include <map>

#include "VertexBuffer.h"
#include "Material.h"

namespace sge
{
	class TransformsBuffer;

	class IndexedMesh
	{
		friend class Renderer;
		friend class Model;

	public:
		class Definition
		{
		public:
			std::vector<VertexBuffer::Definition> vboDefs = {};
			VertexBuffer::Definition eboDef = {};
			std::vector<Material::Definition> matDefs = {}; // At most 1 material per shading mode.
			E_Mutability mutability = E_Mutability::INVALID;
			F_ShadingMode shadingModes = F_ShadingMode::INVALID; // Here for sanity check during initialization of mesh.

			Hash ComputeHash() const;

			bool IsValid() const;
		};

		bool IsValid() const;

	private:
		// 0
		std::vector<HashableHandle<VertexBuffer>> vertexBuffers_ = {}; // 32 bytes
		// 32
		std::map<F_ShadingMode, HashableHandle<Material>> materials_ = {}; // 24 bytes
		// 56
		uint32_t VAO_ = 0; // 4 bytes
		// 60
		E_Mutability mutability_ = E_Mutability::INVALID; // 4 bytes
		// 64
		HashableHandle<VertexBuffer> indexVBO_ = {}; // 16 bytes
		// 80
		uint32_t nrOfVertices_ = 0; // 4 bytes
		// 84
		float radius_ = 0.0f; // 4 bytes
		// 88
		// 8 bytes padding
		// 96

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