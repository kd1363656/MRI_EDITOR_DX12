#include "FWKWindow.h"

bool FWK::Window::Create(const FWK::CommonStruct::Dimension2D& a_size, const std::string& a_titleName, const std::string& a_windowClassName)
{
	// 現在の実行ファイルのハンドルを返す
	HINSTANCE l_hInst = GetInstanceHandle();

	// マルチばいと文字列に文字列を変更
	const std::wstring& l_wndClassName = sjis_to_wide(a_windowClassName);

	// ウィンドウクラスの定義
	WNDCLASSEX l_wc    = {};
	l_wc.cbSize        = sizeof(WNDCLASSEX);				        // 構造体のサイズを設定
	l_wc.style         = k_windowStyleDefault;			        // ウィンドウの動作スタイル(サイズ変更時に再描画など)			
	l_wc.lpfnWndProc   = &FWK::Window::CallWindowProcedure;     // 関数を指定し指定した関数がウィンドウに届いたすべてのメッセージを処理
	l_wc.cbClsExtra    = k_classExtraBytes;						// ウィンドウクラス用に追加で確保するメモリサイズ
	l_wc.cbWndExtra    = k_windowExtraBytes;						// ウィンドウごとに追加で確保するメモリサイズ
	l_wc.hInstance     = l_hInst;								// アプリケーションのインスタンスハンドル
	l_wc.hIcon         = LoadIcon  (nullptr , IDI_APPLICATION); // ウィンドウに表示するアイコン
	l_wc.hIconSm       = LoadIcon  (nullptr , IDI_APPLICATION); // タスクバーやタイトルバーに表示されるアイコン
	l_wc.hCursor       = LoadCursor(nullptr , IDC_ARROW);		// ウィンドウ上で使うマウスカーソルを指定
	l_wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// ウィンドウの背景色を指定
	l_wc.lpszMenuName  = nullptr;								// メニューリソースの名前
	l_wc.lpszClassName = l_wndClassName.c_str();				// ウィンドウクラス名

		// ウィンドウの作成
	m_hWND = CreateWindow(l_wndClassName.c_str()                   ,	 // 登録済みのウィンドウクラス名
						  sjis_to_wide(a_titleName.data()).c_str() , // ウィンドウのタイトル(ワイド文字列に変換したもの)
						  WS_OVERLAPPEDWINDOW - WS_THICKFRAME      , // 標準ウィンドウからサイズ変更用の太い枠を除いたスタイル
						  k_defaultWindowPosX					   , // ウィンドウ座標"X"
						  k_defaultWindowPosY					   , // ウィンドウ座標"Y"
						  a_size.width							   , // クライアント領域の幅
						  a_size.height							   , // クライアント領域の高さ
						  nullptr								   , // 親ウィンドウがない
					      nullptr								   , // メニューなし
						  l_hInst								   , // アプリケーションのインスタンスハンドル
						  this);									 // 作製するウィンドウに"this"ポインタを渡す、"WM_CREATE"などで取り出せる

	if (!m_hWND)
	{
		// メモリリーク防止
		UnregisterClass(l_wndClassName.c_str(), l_hInst);
		return false;
	}

	// クライアントのサイズを設定
	SetClientSize(a_size);

	// ウィンドウの表示
	ShowWindow(m_hWND , SW_SHOW);
	// ウィンドウのクライアント領域をすぐに再描画
	UpdateWindow(m_hWND);

	// "timeGetTime"関数の精度を"1ms"に設定
	timeBeginPeriod(k_timerResolutionMS);

	return true;
}

void FWK::Window::Release()
{
	// もしウィンドウハンドルがインスタンス化されていたら解放
	if (!m_hWND) { return; }

	// 指定したウィンドウを破棄
	DestroyWindow(m_hWND);
	m_hWND = nullptr;	
}

void FWK::Window::SetClientSize(const FWK::CommonStruct::Dimension2D& a_size)
{
	if (!m_hWND) { return; }

	RECT l_rcWND    = {};
	RECT l_rcClient = {};

	// ウィンドウ全体のサイズ、クライアント領域のサイズを取得
	GetWindowRect(m_hWND , &l_rcWND);
	GetClientRect(m_hWND , &l_rcClient);

	MoveWindow(m_hWND																			       , // 対象のウィンドウ 
			   l_rcWND.left																		       , // 左上の"X"座標
			   l_rcWND.top																		       , // 左上の"Y"座標
			   a_size.width  + (l_rcWND.right  - l_rcWND.left) - (l_rcClient.right  - l_rcClient.left) , // ウィンドウ全体の幅(枠含む)
			   a_size.height + (l_rcWND.bottom - l_rcWND.top)  - (l_rcClient.bottom - l_rcClient.top)  , // ウィンドウ全体の高さ(枠含む)
			   TRUE);	
}

LRESULT CALLBACK FWK::Window::CallWindowProcedure(const HWND   a_hWND    ,
												  const UINT   a_message , 
											      const WPARAM a_wParam  , 
												  const LPARAM a_lParam)
{
	// "a_hWnd"に関連付けて保存されているクラスのインスタンスのポインタを取得
	auto* l_this = static_cast<FWK::Window*>(GetProp(a_hWND , k_windowInstancePropName));

	if (!l_this)
	{
		// ウィンドウが作製されたタイミングで送られるメッセージを受け取ったら実行
		if (a_message == WM_CREATE)
		{
			auto* l_createStruct = (CREATESTRUCT*)a_lParam;						 // "CreateWindowEX"で指定した情報が詰まっている
			auto* l_window       = (FWK::Window*)l_createStruct->lpCreateParams; // "CreateWindowEX"から"this"ポインタを取得

			SetProp(a_hWND , k_windowInstancePropName , l_window);

			// 正常に終了したことを伝える
			return k_messageHandled;
		}
		else
		{
			return DefWindowProc(a_hWND    , 
								 a_message , 
								 a_wParam  , 
								 a_lParam);
		}
	}

	// インスタンス側の"Window"関数を実行
	return l_this->WindowProcedure(a_hWND    ,
								   a_message ,
								   a_wParam  ,
								   a_lParam);
}

LRESULT FWK::Window::WindowProcedure(const HWND   a_hWND    , 
								     const UINT   a_message , 
									 const WPARAM a_wParam  , 
									 const LPARAM a_lParam)
{
	// メッセージによって処理を選択
	switch (a_message)
	{
		// "X"ボタンが押された
		case WM_CLOSE:
			Release();
		break;

		// ウィンドウ破棄直前
		case WM_DESTROY:
			// ウィンドウに紐づけたクラスポインタを削除
			RemoveProp     (a_hWND , k_windowInstancePropName);
			// アプリケーション終了の合図を送信
			PostQuitMessage(NULL);
		break;

		default:
			// メッセージのデフォルト処理
			return DefWindowProc(a_hWND    , 
								 a_message ,
								 a_wParam  ,
								 a_lParam);
		break;
	}

	// 正常に終了したことを伝える
	return 0;
}