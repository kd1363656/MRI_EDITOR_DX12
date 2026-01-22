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

	private:

		ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
	};
}