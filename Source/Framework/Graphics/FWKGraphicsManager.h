#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public FWK::SingletonBase<FWK::Graphics::GraphicsManager>
	{
	public:

		bool Init();
	
	private:

		bool CreateFactory       ();
		bool CreateDevice        ();
		bool CreateCommandObjects();

#if defined (_DEBUG)
		void EnableDebugLayer() const;
#endif

#if defined (_DEBUG)
		static constexpr const wchar_t* const k_debugSelectedGPUText = L"選択候補 GPU";
		static constexpr const wchar_t* const k_debugSucceeded		 = L" -> デバイス作成に成功\n";
		static constexpr const wchar_t* const k_lineBreak			 = L"\n";
#endif

		static constexpr UINT k_singleGPUNodeMask = 0U;	// 単一"GPU"を使用するノードマスク。マルチアダプター非対応。

		ComPtr<ID3D12Device10>  m_device      = nullptr;	// "DirectX12"においてオブジェクトの生成などに使用
		ComPtr<IDXGIFactory7>   m_dxgiFactory = nullptr;	// 使用するグラフィックカードの設定やスワップチェーンの作製に使用
		ComPtr<IDXGISwapChain4> m_swapChain   = nullptr;	// ダブルバッファリングにおける描画バッファの切り替えに使用するスワップチェーン

		ComPtr<ID3D12GraphicsCommandList7> m_graphicsCommandList = nullptr;
		ComPtr<ID3D12CommandQueue>         m_commandQueue        = nullptr;
		ComPtr<ID3D12CommandAllocator>     m_commandAllocator    = nullptr;

		//===============================
		// シングルトン
		//===============================
		friend class FWK::SingletonBase<FWK::Graphics::GraphicsManager>;

		GraphicsManager ()          = default;
		~GraphicsManager() override = default;
	};
}