#pragma once

namespace FWK::Graphics
{
	class Pipeline final
	{
	public:

		Pipeline () = default;
		~Pipeline() = default;

		void Init(const std::vector<ComPtr<ID3DBlob>>& a_bufferList , const ComPtr<ID3D12RootSignature>& a_rootSignature);
	};
}