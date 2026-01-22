#pragma once

namespace FWK::Graphics
{
	class RootSignature final
	{
	public:

		RootSignature () = default;
		~RootSignature() = default;

		void Init();

		auto& GetWorkRootSignature() { return m_rootSignature; }
		auto& GetWorkVBView       () { return m_vbView;        }

	private:

		ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

		D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	};
}