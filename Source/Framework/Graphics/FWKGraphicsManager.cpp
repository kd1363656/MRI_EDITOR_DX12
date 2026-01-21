#include "FWKGraphicsManager.h"

bool FWK::Graphics::GraphicsManager::Init(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size)
{
	// デバック時のみ有効化(なぜなら処理が重いから)
#if defined (_DEBUG)
	EnableDebugLayer(); // "D3D12"のデバックレイヤーを有効化
#endif

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

	// "RTVDescriptorHeap"の作成
	if (!CreateRTVDescriptorHeap())
	{
		assert(false && "\"RTVDescriptorHeap\"作成に失敗しました。");
		return false;
	}

	// スワップチェイン用"RTV"の作成
	if (!CreateSwapChainRTV())
	{
		assert(false && "スワップチェーン用\"RTV\"作成に失敗しました。");
		return false;
	}

	// フェンスの作成
	if (!CreateFence())
	{
		assert(false && "フェンスの作成に失敗しました。");
		return false;
	}

	return true;
}

void FWK::Graphics::GraphicsManager::BeginDraw()
{
	if (!m_commandAllocator || !m_commandList || !m_rtvDescriptorHeap || !m_swapChain) { return; }

	// アロケーターとコマンドリストをクリア
	m_commandAllocator->Reset();
	m_commandList->Reset     (m_commandAllocator.Get() ,  nullptr);
	
	// 裏画面のバッファーのインデックスを取得
	const UINT l_bbIDX = m_swapChain->GetCurrentBackBufferIndex();

	// 現在のバックバッファーの"RTV"用の"CPU"ハンドルを取得
	auto l_handle = m_rtvDescriptorHeap->GetRTVCPUHandle(l_bbIDX);

	// "GPU"にリソースの状態の遷移を伝える
	SetResourceBarrier(m_swapChainBuffers[l_bbIDX].Get() , D3D12_RESOURCE_STATE_RENDER_TARGET , D3D12_RESOURCE_STATE_PRESENT);

	// 第一引数 : 描画先レンダーターゲットの数(通常は"1")、第二引数 : 描画先レンダーターゲットのハンドルのポインタ、第三引数 : "true"で連続したディスクリプタヒープ内の範囲"false"で複数の非連続ハンドルを指定、第四引数 : デプスステンシルビュー
	m_commandList->OMSetRenderTargets(k_renderTargetDescriptorNum , &l_handle , true , nullptr);

	// レンダーターゲットをバックバッファーの色でクリア
	m_commandList->ClearRenderTargetView(l_handle , k_clearBackBufferColor , 0 ,  nullptr);
}
void FWK::Graphics::GraphicsManager::EndDraw()
{
	if (!m_commandList || !m_commandQueue || !m_commandAllocator)
	{
		return;
	}

	// 描画命令をすべて書き終えたら"Close"する、"Close"しないと描画の実行ができない
	m_commandList->Close();

	// コマンドリストの型を合わせる
	ID3D12CommandList* l_cmdList[] = { m_commandList.Get() };
	
	// 第一引数 : 実行するコマンドリストの数、第二引数 : 実行するコマンドリストの配列。
	m_commandQueue->ExecuteCommandLists(k_executeCommandListNum , l_cmdList);

	// "GPU"と"CPU"の同期をとる
	WaitForSyncCommandQueue();

	// アロケーターとコマンドリストをクリア
	m_commandAllocator->Reset();
	m_commandList->Reset     (m_commandAllocator.Get() ,  nullptr);

	// スクリーンフリップ処理
	// 第一引数 : 同期間隔(60"FPS"想定)、第二引数 : オプションフラグ
	m_swapChain->Present(k_defaultSyncInterval ,  0U);
}

bool FWK::Graphics::GraphicsManager::CreateFactory()
{
	UINT l_flagsDXGI = 0U;

#if defined (_DEBUG)
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
		OutputDebugStringW(k_lineBreakSTR);
#endif

		// 一致するフィーチャーレベルを最高レベルからレベルを下げていって探索
		for (const auto& l_level : l_featureLevels)
		{
			// 第一引数 : デバイスの作成に使用するグラフィックカードのポインター、第二引数 : デバイス作成を成功させるために必要な"D3D_FEATURE_LEVEL"、第三引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
			if (!SUCCEEDED(D3D12CreateDevice(l_adapter.Get() , l_level , IID_PPV_ARGS(&m_device)))) { continue; }
			
#if defined (_DEBUG)
		// アダプター名をデバックログに出力
			OutputDebugStringW(k_debugSucceededText);
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

	// コマンドアロケーターの作成
	// 第一引数 : 作製するコマンドアロケーターの種類、第二引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	auto l_hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT , IID_PPV_ARGS(&m_commandAllocator));

	// コマンドアロケータ作成に失敗したら"assert"
	if (FAILED(l_hr))
	{
		assert(false && "コマンドアロケータの作成に失敗しました。");
		return false;
	}

	// コマンドキュー作成
	D3D12_COMMAND_QUEUE_DESC l_queueDesc = {};
	l_queueDesc.Type					 = D3D12_COMMAND_LIST_TYPE_DIRECT;		     // どの種類のコマンドを渡すキューか
	l_queueDesc.Priority				 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;      // キューの優先度を指定
	l_queueDesc.Flags					 = D3D12_COMMAND_QUEUE_FLAG_NONE;		     // コマンドキューに対するオプション(現在格納しているフラグは"GPU"のタイムアウトを無効化)
	l_queueDesc.NodeMask				 = FWK::CommonConstant::k_singleGPUNodeMask;	 // マルチアダプターを指定
	
	// 第一引数 : 作製するキューのパラメータ、第二引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	l_hr = m_device->CreateCommandQueue(&l_queueDesc , IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(l_hr))
	{
		assert(false && "コマンドキューの作成に失敗しました。");
		return false;
	}

	// コマンドリスト作成
	l_hr = m_device->CreateCommandList1(FWK::CommonConstant::k_singleGPUNodeMask	 ,	// 第一引数 : 複数の"GPU"を使うかどうか
										D3D12_COMMAND_LIST_TYPE_DIRECT           ,	// 第二引数 : 作製するコマンドリストの種類
										D3D12_COMMAND_LIST_FLAG_NONE             ,	// 第三引数 : 作製フラグ
									    IID_PPV_ARGS(&m_commandList));				// 第四引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ

	if (FAILED(l_hr))
	{
		assert(false && "グラフィックスコマンドリストの作成に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Graphics::GraphicsManager::CreateSwapChain(const HWND a_hWND , const FWK::CommonStruct::Dimension2D& a_size)
{
	if (!m_dxgiFactory || !m_commandQueue)
	{
		assert(false && "\"DXGI\"ファクトリーまたはコマンドキューが初期化されていません。");
		return false;
	}

	// スワップチェーンのパラメータを設定
	DXGI_SWAP_CHAIN_DESC1 l_swapChainDesc = {};
	l_swapChainDesc.BufferCount			  = FWK::CommonConstant::k_defaultBackBufferNum;		// スワップチェーン内のバッファ数(バックバッファ数は"2")
	l_swapChainDesc.Width				  = a_size.width;									// 解像度の幅
	l_swapChainDesc.Height				  = a_size.height;									// 解像度の高さ
	l_swapChainDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;						// 表示形式(ピクセルフォーマット)
	l_swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;				// バックバッファーの使用目的
	l_swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;					// バッファの切り替え
	l_swapChainDesc.SampleDesc.Count      = k_sampleCount;									// マルチサンプルエイリアス
	l_swapChainDesc.AlphaMode			  = DXGI_ALPHA_MODE_UNSPECIFIED;					// アルファブレンドの方法
	l_swapChainDesc.Scaling               = DXGI_SCALING_STRETCH;							// ウィンドウサイズとバックバッファサイズが異なる場合のスケーリング方法。
	l_swapChainDesc.Stereo				  = FALSE;											// 全画面表示モードまたはスワップチェーンバックバッファーがステレオかどうかを指定

	ComPtr<IDXGISwapChain1> l_swapChainCache = nullptr;

	auto l_hr = m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get() ,	// 第一引数 : コマンドキューのポインタ
													  a_hWND				   ,	// 第二引数 : ウィンドウハンドル
													  &l_swapChainDesc     ,	// 第三引数 : スワップチェーンのパラメーター
													  nullptr              ,	// 第四引数 : フルスクリーン時の設定
													  nullptr			   ,	// 第五引数 : モニター指定("nullptr"で自動選択)
													  &l_swapChainCache);		// 第六引数 : 結果格納変数	
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
bool FWK::Graphics::GraphicsManager::CreateRTVDescriptorHeap()
{
	if (!m_swapChain || !m_device)
	{
		assert(false && "スワップチェーンまたはデバイスが初期化されていません。");
		return false;
	}

	// バックバッファ数を取得
	const UINT l_bufferCount = FWK::CommonConstant::k_defaultBackBufferNum;

	// "RTV"ディスクリプタヒープを初期化
	if (!m_rtvDescriptorHeap)
	{
		m_rtvDescriptorHeap = std::make_unique<FWK::Graphics::RTVDescriptorHeap>();
	}

	// バックバッファの数を格納つまり"RTV"の数は二つ
	if (!m_rtvDescriptorHeap->Init(l_bufferCount))
	{
		assert (false && "\"RTV\"ディスクリプタヒープの初期化に失敗しました。");
		return false;
	}

	return true;
}
bool FWK::Graphics::GraphicsManager::CreateSwapChainRTV()
{
	if (!m_swapChain || !m_rtvDescriptorHeap)
	{
		assert(false && "スワップチェーンまたは\"RTVHeap\"が初期化されていません。");
		return false;
	}

	// バックバッファ数を取得
	const UINT l_bufferCount = FWK::CommonConstant::k_defaultBackBufferNum;

	// スワップチェーンの各バッファーに対して"RTV"を生成
	for (UINT l_i = 0U; l_i < l_bufferCount; ++l_i)
	{
		// "for"文がバッファーの数を超えていたら"break"
		if (l_i >= m_swapChainBuffers.size()) 
		{
#if defined (_DEBUG)
			OutputDebugStringW(k_overBackBufferText);
#endif
			break; 
		}

		// スワップチェーンのバックバッファーとビューを結びつける
		if (FAILED(m_swapChain->GetBuffer(l_i , IID_PPV_ARGS(&m_swapChainBuffers[l_i]))))
		{
			assert(false && "バックバッファの取得に失敗しました。");
			return false;
		}

		// "RTV"作製(内部的にインデックス管理を行ってくれる)
		m_rtvDescriptorHeap->CreateRTV(m_swapChainBuffers[l_i]);
	}

	return true;
}

bool FWK::Graphics::GraphicsManager::CreateFence()
{
	// 第一引数 : 初期フェンス値、第二引数 : フェンスのオプション、第三引数 : 取得するインターフェースの"ID"と、結果を受け取るポインタ
	if (FAILED(m_device->CreateFence(m_fenceVal , D3D12_FENCE_FLAG_NONE , IID_PPV_ARGS(&m_fence))))
	{
		assert(false && "フェンスの作成に失敗");
		return false;
	}

	return true;
}

void FWK::Graphics::GraphicsManager::WaitForSyncCommandQueue()
{
	if (!m_commandQueue || !m_fence) { return; }

	// 第一引数 : 値を送る対象のフェンス、第二引数 : "GPU"が到達したことを示すフェンスの値
	m_commandQueue->Signal(m_fence.Get() , ++m_fenceVal);

	// "GPU"がまだこのフェンス地に到達していなければ実行しない
	if (m_fence->GetCompletedValue() != m_fenceVal)
	{
		// イベントオブジェクトを作成
		// 
		m_fenceEvent = CreateEvent(nullptr,		// セキュリティ属性の指定
								   FALSE  ,		// 自動リセットイベント
								   FALSE  ,		// 初期状態は日シグナル
								   nullptr);	// 名前をつけない匿名イベント
		if (!m_fenceEvent)
		{
			assert(false && "フェンスイベントの作成に失敗");
			return;
		}

		m_fence->SetEventOnCompletion(m_fenceVal   , m_fenceEvent);	 // "GPU"が"m_fenceVal"に到達したら、イベントをシグナル状態にするように登録
		WaitForSingleObject          (m_fenceEvent , INFINITE);		 // イベント発生まで待ち続ける
		CloseHandle                  (m_fenceEvent);				 // イベントハンドルを閉じる 
	}
}

#if defined (_DEBUG)
void FWK::Graphics::GraphicsManager::EnableDebugLayer() const
{
	ComPtr<ID3D12Debug6> l_debugController;
	if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&l_debugController)))) { return; }
	
	l_debugController->EnableDebugLayer();
}
#endif

void FWK::Graphics::GraphicsManager::SetResourceBarrier(const ComPtr<ID3D12Resource>& a_resource , D3D12_RESOURCE_STATES a_befor , D3D12_RESOURCE_STATES a_after) const
{
	if (!a_resource)
	{
		assert(false && "スワップチェーン用のバッファーが作製されていません");
		return;
	}

	if (!m_commandList)
	{
		assert(false && "コマンドリストが作製されていません");
		return;
	}
	
	// リソースバリアのパラメーター
	D3D12_RESOURCE_BARRIER l_barrier = {};										
	l_barrier.Type					 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;  // リソースバリアの種類(トランジションバリア(あるリソースのステートを別のステートに変えるもの))
	l_barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;		// オプションフラグ
	l_barrier.Transition.pResource   = a_resource.Get();						// ステートを変えたいリソースのポインタ
	l_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; // どのサブリソースに対してバリアを掛けるか
	l_barrier.Transition.StateBefore = a_befor;									// 現在のリソースのステート
	l_barrier.Transition.StateAfter  = a_after;									// このバリア処理で移行したいステート

	m_commandList->ResourceBarrier(k_setBarrierNum , &l_barrier);
}

FWK::Graphics::GraphicsManager::GraphicsManager() = default;
FWK::Graphics::GraphicsManager::~GraphicsManager()
{
	WaitForSyncCommandQueue();
}