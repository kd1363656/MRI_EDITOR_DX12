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

}

bool Application::Init(const FWK::CommonStruct::Dimension2D& a_size)
{

	return false;
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