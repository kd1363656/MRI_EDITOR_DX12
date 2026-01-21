#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public FWK::SingletonBase<FWK::Graphics::GraphicsManager>
	{
	public:

		bool Init(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size);
	
		const ComPtr<ID3D12Device10>& GetDevice() const { return m_device; }

	private:

		bool CreateFactory           ();
		bool CreateDevice            ();
		bool CreateCommandObjects    ();
		bool CreateSwapChain         (const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size);
		bool CreateRTVDescriptorHeap();
		bool CreateSwapChainRTV     ();
		bool CreateFence            ();

#if defined (_DEBUG)
		void EnableDebugLayer() const;
#endif

#if defined (_DEBUG)
		static constexpr const wchar_t* const k_debugSelectedGPUText = L"選択候補 GPU";
		static constexpr const wchar_t* const k_debugSucceededText	 = L" -> デバイス作成に成功\n";
		static constexpr const wchar_t* const k_overBackBufferText	 = L"\n";
		static constexpr const wchar_t* const k_lineBreakSTR		 = L"バックバッファーの容量を超えました。";
#endif

		void SetResourceBarrier(const ComPtr<ID3D12Resource>& a_resource , D3D12_RESOURCE_STATES a_befor , D3D12_RESOURCE_STATES a_after) const ;

		static constexpr UINT k_sampleCount   = 1U;
		static constexpr UINT k_setBarrierNum = 1U;

		std::array<ComPtr<ID3D12Resource2> , FWK::CommonConstant::k_defaultBackBufferNum> m_swapChainBuffers;

		ComPtr<ID3D12Device10>  m_device      = nullptr;	// "DirectX12"においてオブジェクトの生成などに使用
		ComPtr<IDXGIFactory7>   m_dxgiFactory = nullptr;	// 使用するグラフィックカードの設定やスワップチェーンの作製に使用
		ComPtr<IDXGISwapChain4> m_swapChain   = nullptr;	// ダブルバッファリングにおける描画バッファの切り替えに使用するスワップチェーン

		ComPtr<ID3D12GraphicsCommandList7> m_commandList      = nullptr;
		ComPtr<ID3D12CommandQueue>         m_commandQueue     = nullptr;
		ComPtr<ID3D12CommandAllocator>     m_commandAllocator = nullptr;

		ComPtr<ID3D12Fence1> m_fence = nullptr;	// "CPU"と"GPU"間の同期をとるのに必要

		std::unique_ptr<FWK::Graphics::RTVDescriptorHeap> m_rtvDescriptorHeap = nullptr;

		UINT64 m_fenceVal   = 0ULL;
		HANDLE m_fenceEvent = nullptr;

		//===============================
		// シングルトン
		//===============================
		friend class FWK::SingletonBase<FWK::Graphics::GraphicsManager>;

		GraphicsManager ()          = default;
		~GraphicsManager() override = default;
	};
}