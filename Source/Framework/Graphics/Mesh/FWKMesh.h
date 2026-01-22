#pragma once

namespace FWK::Graphics
{
	class Mesh final
	{
	public:

		Mesh () = default;
		~Mesh() = default;

		void Create();

		auto& GetWorkVertexBufferView() { return m_vertexBufferView; }

	private:

		std::array<Math::Vector3 , 3> m_vertices;		  // 頂点座標

		ComPtr<ID3D12Resource2> m_vertexBuffer = nullptr; // 頂点バッファー

		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {}; // 頂点情報を扱うもの
	};
}