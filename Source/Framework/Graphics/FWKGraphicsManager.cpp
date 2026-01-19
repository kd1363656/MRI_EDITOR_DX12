#include "FWKGraphicsManager.h"

bool FWK::Graphics::GraphicsManager::Init(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size)
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

	// コマンドリスト、コマンドアロケーター、コマンドキューの作成
	if (!CreateCommandObjects())
	{
		assert(false && "コマンド関連オブジェクトの作成に失敗しました。");
		return false;
	}

	// スワップチェーン作製
	if (!CreateSwapChain(a_hWND , a_size))
	{
		assert(false && "スワップチェーン作成に失敗しました。");
		return false;
	}

	return true;
}

bool FWK::Graphics::GraphicsManager::CreateFactory()
{
	UINT l_flagsDXGI = 0U;

#if defined (_DEBUG)
	EnableDebugLayer();							// "D3D12"のデバックレイヤーを有効化
	l_flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;	// "DXGI"関係のログ出力を可能にするフラグ
#endif

	// "DXGI"ファクトリーの作成 
	// 第一引数 :デバックモードフラグを"On"にするか"Off"にするか 、第二引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	HRESULT l_hr = CreateDXGIFactory2(l_flagsDXGI , IID_PPV_ARGS(&m_dxgiFactory));	// "CreateDXGIFactory2"はデバックモードかどうかを選べる

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

	ComPtr<IDXGIAdapter4> l_adapter       = nullptr;	// アダプターの列挙に使用
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

	// 高パフォーマンス"GPU"優先でアダプターを列挙、使用するグラフィックスカードは一つだけ
	// 第一引数 : 列挙するアダプターのインデックス、第二引数 : アプリの"GPU"、第三引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	for (UINT l_i = 0U; m_dxgiFactory->EnumAdapterByGpuPreference(l_i , DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE , IID_PPV_ARGS(&l_adapter)) != DXGI_ERROR_NOT_FOUND; ++l_i)
	{
		DXGI_ADAPTER_DESC3 l_desc;

		// "GPU"アダプターの表際情報の取得に成功したかどうか
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
			// 第一引数 : デバイスの作成に使用するグラフィックカードのポインター、第二引数 : デバイス作成を成功させるために必要な"D3D_FEATURE_LEVEL"、第三引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
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
bool FWK::Graphics::GraphicsManager::CreateCommandObjects()
{
	if (!m_device)
	{
		assert(false && "デバイスの作製ができていません。");
		return false;
	}

	// コマンドアロケーターの作成(第一引数 : 作製するコマンドアロケーターの種類、第二引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	auto l_hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT , IID_PPV_ARGS(&m_commandAllocator));

	// コマンドアロケータ作成に失敗したら"assert"
	if (FAILED(l_hr))
	{
		assert(false && "コマンドアロケータの作成に失敗しました。");
		return false;
	}

	// コマンドキュー作成
	D3D12_COMMAND_QUEUE_DESC l_queueDesc = {};
	l_queueDesc.Type					 = D3D12_COMMAND_LIST_TYPE_DIRECT;		// どの種類のコマンドを渡すキューか
	l_queueDesc.Priority				 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // キューの優先度を指定
	l_queueDesc.Flags					 = D3D12_COMMAND_QUEUE_FLAG_NONE;		// コマンドキューに対するオプション(現在格納しているフラグは"GPU"のタイムアウトを無効化)
	l_queueDesc.NodeMask				 = k_singleGPUNodeMask;					// マルチアダプターを指定
	
	// 第一引数 : 作製するキューのパラメータ、第二引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	l_hr = m_device->CreateCommandQueue(&l_queueDesc , IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(l_hr))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	// コマンドリスト作成
	l_hr = m_device->CreateCommandList1(0U							        ,	// 第一引数 : 複数の"GPU"を使うかどうか
										D3D12_COMMAND_LIST_TYPE_DIRECT      ,	// 第二引数 : 作製するコマンドリストの種類
										D3D12_COMMAND_LIST_FLAG_NONE        ,	// 第三引数 : 作製フラグ
									    IID_PPV_ARGS(&m_graphicsCommandList));	// 第四引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ

	if (FAILED(l_hr))
	{
		assert(false && "グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	// 作製直後のコマンドリストを明示的に"Close"
	m_graphicsCommandList->Close();
	return true;
}
bool FWK::Graphics::GraphicsManager::CreateSwapChain(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size)
{
	if (!m_dxgiFactory || !m_commandQueue)
	{
		assert(false && "\"DXGI\"ファクトリーまたはコマンドキューが初期化されていません。");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC1 l_swapChainDesc = {};
	l_swapChainDesc.BufferCount			  = k_defaultBackBufferNum;
	l_swapChainDesc.Width				  = a_size.width;
	l_swapChainDesc.Height				  = a_size.height;
	l_swapChainDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
	l_swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	l_swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	l_swapChainDesc.SampleDesc.Count      = k_sampleCount;
	l_swapChainDesc.AlphaMode			  = DXGI_ALPHA_MODE_UNSPECIFIED;
	l_swapChainDesc.Scaling               = DXGI_SCALING_STRETCH;
	l_swapChainDesc.Stereo				  = FALSE;

	ComPtr<IDXGISwapChain1> l_swapChainCache = nullptr;

	auto l_hr = m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get() ,	// 第一引数 : コマンドキューのポインタ
													  a_hWND				   ,	// 第二引数 : ウィンドウハンドル
													  &l_swapChainDesc     ,	// 第三引数 : スワップチェーンのパラメーター
													  nullptr              ,	// 第四引数 : フルスクリーン時の設定
													  nullptr			   ,	// 第五引数 : モニター指定("nullptr"で自動選択)
													  &l_swapChainCache);		// 結果格納変数	
	if (FAILED(l_hr))
	{
		assert(false && "スワップチェーンの作製に失敗しました。");
		return false;
	}

	if (!l_swapChainCache)
	{
		assert(false && "スワップチェーンの作製に失敗しました。");
		return false;
	}

	// "IDXGISwapChain4"にキャストして保持
	l_hr = l_swapChainCache.As(&m_swapChain);
	if (FAILED(l_hr))
	{
		assert(false && "スワップチェーンの型変換に失敗しました。");
		return false;
	}

	return true;
}

#if defined (_DEBUG)
void FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	ComPtr<ID3D12Debug6> l_debugController;
	if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&l_debugController)))) { return; }
	
	l_debugController->EnableDebugLayer();
}
#endif