#include "Application.h"

int WINAPI WinMain(_In_     HINSTANCE ,
				   _In_opt_ HINSTANCE ,
				   _In_     LPSTR     ,
				   _In_     int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr , COINIT_MULTITHREADED)))
	{
		CoUninitialize();
		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL , "japanese");

	Application::GetInstance().Execute();

	// COM解放
	CoUninitialize();
	return 0;
}

void Application::Execute()
{
	// ウィンドウの解像度を読み取る
	LoadWindowSize();

	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	if (!Application::GetInstance().Init(m_windowSize))
	{
		return;
	}

	// ゲームループ
	m_fpsController.Init();

	FWK::Graphics::Mesh l_mesh = {};
	l_mesh.Create();

	FWK::Graphics::Shader l_shader = {};
	l_shader.Init();

	auto& l_pipeline = l_shader.GetWorkPipeline();
	if (!l_pipeline)
	{
		assert(false && "パイプラインが初期化されていません。");
	}

	auto& l_rootSignature = l_shader.GetWorkRootSignature();
	if (!l_rootSignature)
	{
		assert(false && "ルートシグネチャが初期化されていません。");
	}

	while (true)
	{
		// "FPS"の計測
		m_fpsController.UpdateStartTime();

		// ウィンドウメッセージの処理
		m_window.ProcessMessage();

		// ウィンドウが破棄されているか"Escape"キーが押されていたらゲームループ終了
		if (!m_window.IsCreated() || GetAsyncKeyState(VK_ESCAPE))
		{
			EndGameLoop();
		}

		// もしゲームループ終了フラグが立っていたら"break"
		if (m_isEndGameLoop) { break; }

		// アプリケーション描画更新
		FWK::Graphics::GraphicsManager::GetInstance().BeginDraw(l_pipeline->GetWorkPipelineState() , l_rootSignature->GetWorkRootSignature() , l_pipeline->GetViewPort() , l_pipeline->GetScissorrect() , l_mesh.GetWorkVertexBufferView());
		FWK::Graphics::GraphicsManager::GetInstance().EndDraw  ();

		// フレームレート制御
		m_fpsController.Update();

		// ウィンドウのタイトルバーを更新
		UpdateWindowTitleBar();
	}

	// ウィンドウの解像度を保存(ウィンドウサイズの設定は保存しておくべきだから)
	SaveWindowSize();

	// アプリケーション開放
	Release();
}

void Application::EndGameLoop()
{
	m_isEndGameLoop = true;
}

bool Application::Init(const FWK::CommonStruct::Dimension2D& a_size)
{
	const std::string& l_titleBar = GenerateWindowTitleText();

	// ウィンドウ作成
	if (!m_window.Create(a_size , l_titleBar , k_windowClassName)) 
	{
		MessageBoxA(nullptr                   , 
					k_windowCreateFailMessage ,
					k_failCaption			  ,
					MB_OK);

		return false;
	}

	const HWND l_hWND = GetHWND();

	// タイトル名 + "FPS"の表示
	SetWindowTextA(l_hWND , l_titleBar.c_str());

	// "DirectX12"初期化
	if (!FWK::Graphics::GraphicsManager::GetInstance().Init(l_hWND , a_size))
	{
		MessageBoxA(nullptr               , 
					k_dx12InitFailMessage ,
					k_failCaption		  ,
					MB_OK);

		return false;
	}

	m_isEndGameLoop = false;

	return true;
}

void Application::Release()
{
	// ウィンドウ削除
	m_window.Release();
}

void Application::LoadWindowSize()
{
	auto l_rootJson = FWK::FileIOUtility::LoadJsonFile(k_windowSizeFileIOPath);
	if (l_rootJson.is_null()) { return; }

	m_windowSize.height = l_rootJson.value("Height" , FWK::CommonConstant::k_defaultWindowSize.height);
	m_windowSize.width  = l_rootJson.value("Width"  , FWK::CommonConstant::k_defaultWindowSize.width);
}
void Application::SaveWindowSize()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["Height"] = m_windowSize.height;
	l_rootJson["Width"]  = m_windowSize.width;

	FWK::FileIOUtility::SaveJsonFile(l_rootJson , k_windowSizeFileIOPath);
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + "FPS"の表示
	// 一時オブジェクトは"const"参照で有効期限を延ばせる
	const std::string& l_titleBar = GenerateWindowTitleText();
	SetWindowTextA                                         (GetHWND() , l_titleBar.c_str());
}

std::string Application::GenerateWindowTitleText() const
{
	return std::format("{} : {}" , k_titleName , GetNowFPS());
}