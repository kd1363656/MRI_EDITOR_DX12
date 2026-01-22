#pragma once

namespace FWK::Graphics
{
	class Pipeline final
	{
	public:

		Pipeline () = default;
		~Pipeline() = default;

		void Init(const std::vector<ComPtr<ID3DBlob>>& a_bufferList , const ComPtr<ID3D12RootSignature>& a_rootSignature);

		auto& GetWorkPipelineState() { return m_pipelineState; }

		const auto& GetViewPort   () const { return m_viewPort;    }
		const auto& GetScissorrect() const { return m_scissorrect; }

	private:

		ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;

		D3D12_VIEWPORT m_viewPort    = {};
		D3D12_RECT     m_scissorrect = {};
	};
}