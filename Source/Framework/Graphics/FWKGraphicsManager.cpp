#include "FWKGraphicsManager.h"

bool FWK::Graphics::GraphicsManager::Init()
{
	// スワップチェーンやデバイス作成に使うファクトリーを作成
	if (!CreateFactory())
	{
		assert(false && "ファクトリーの作成に失敗。");
		return false;
	}

	// デスクリプタやコマンドリストを作製するために作製
	if (!CreateDevice())
	{
		assert(false && "デバイスの作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::GraphicsManager::CreateFactory()
{
	UINT l_flagsDXGI = 0U;

#if defined (_DEBUG)
	// "D3D12"のデバックレイヤーを有効化
	EnableDebugLayer();
	l_flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;	// "DXGI"関係のログ出力を可能にするフラグ
#endif

	// "DXGI"ファクトリーの作成 ("IDXGIFactoy7"使用)
	HRESULT l_hr = CreateDXGIFactory2(l_flagsDXGI , IID_PPV_ARGS(&m_dxgiFactory));	// "CreateDXGIFactory2"はデバックモードかどうかを選べるだけ

	if (FAILED(l_hr))
	{
		return false;
	}

	return true;
}

bool FWK::Graphics::GraphicsManager::CreateDevice()
{
	// ファクトリーが生成されていなければ"false"を返す
	if (!m_dxgiFactory)
	{
		assert(false && "\"dxgiFacotry\"が作製されていません。デバイスを作成出来ません。");
		return false;
	}

	ComPtr<IDXGIAdapter4> l_adapter       = nullptr;
	bool				  l_deviceCreated = false;

	// フィーチャーレベルは"GPU"がサポートする機能の段階を示し、アプリケーションが利用できるグラフィックス機能の範囲を示す。
	constexpr D3D_FEATURE_LEVEL l_featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2 , 
		D3D_FEATURE_LEVEL_12_1 , 
		D3D_FEATURE_LEVEL_12_0 , 
		D3D_FEATURE_LEVEL_11_1 , 
		D3D_FEATURE_LEVEL_11_0 , 
	};

	// 高パフォーマンス"GPU"優先でアダプターを列挙
	// 使用するグラフィックスカードは一つだけ
	for (UINT l_i = 0U; m_dxgiFactory->EnumAdapterByGpuPreference(l_i , DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE , IID_PPV_ARGS(&l_adapter)) != DXGI_ERROR_NOT_FOUND; ++l_i)
	{
		DXGI_ADAPTER_DESC3 l_desc;
		if (FAILED(l_adapter->GetDesc3(&l_desc))) { continue; }

		// ソフトウェアアダプター("WARP")は後で処理
		if (l_desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) { continue; }

#if defined (_DEBUG)
		// アダプター名をデバックログに出力
		OutputDebugStringW(k_debugSelectedGPUText);
		OutputDebugStringW(l_desc.Description);
		OutputDebugStringW(k_lineBreak);
#endif

		// 一致するフィーチャーレベルを最高レベルからレベルを下げていって探索
		for (const auto& l_level : l_featureLevels)
		{
			if (!SUCCEEDED(D3D12CreateDevice(l_adapter.Get() , l_level , IID_PPV_ARGS(&m_device)))) { continue; }
			
#if defined (_DEBUG)
		// アダプター名をデバックログに出力
			OutputDebugStringW(k_debugSucceeded);
#endif
			l_deviceCreated = true;
			break;
		}

		// 選択された高性能"GPU"を使う
		if (l_deviceCreated) { break; }
	}

	// どの"GPU"でも失敗した場合に使用
	return l_deviceCreated;
}

#if defined (_DEBUG)
void FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	ComPtr<ID3D12Debug6> l_debugController;
	if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&l_debugController)))) { return; }
	
	l_debugController->EnableDebugLayer();
}
#endif