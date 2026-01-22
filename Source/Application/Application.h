#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute    ();
	void EndGameLoop();

	HWND GetHWND() const { return m_window.GetHWND(); }

	int GetNowFPS    () const { return m_fpsController.GetNowFPS    (); }
	int GetMaxFPS    () const { return m_fpsController.GetMaxFPS    (); }
	int GetDefaultFPS() const { return m_fpsController.GetDefaultFPS(); }

	float GetScaledDeltaTime() const { return m_fpsController.GetScaledDeltaTime(); }

private:

	bool Init   (const FWK::CommonStruct::Dimension2D& a_size);
	void Release();

	void LoadWindowSize();
	void SaveWindowSize();

	void UpdateWindowTitleBar() const;

	std::string GenerateWindowTitleText() const;

	static constexpr std::string_view k_titleName = "MRI_FRAMEWORK_DX12";
	
	static constexpr const char* const k_failCaption			 = "エラー";
	static constexpr const char* const k_windowCreateFailMessage = "ウィンドウの作成に失敗";
	static constexpr const char* const k_dx12InitFailMessage     = "\"DX12\"の初期化に失敗";
	
	const std::string k_windowSizeFileIOPath    = "Asset/Data/Setting/Window/WindowConfig.json";
	const std::string k_windowClassName         = "Window";
	
	FWK::FPSController m_fpsController = {};

	FWK::Window m_window = {};

	FWK::CommonStruct::Dimension2D m_windowSize = FWK::CommonConstant::k_defaultWindowSize;

	bool m_isEndGameLoop = false;

	//===============================
	// シングルトン
	//===============================
	friend class FWK::SingletonBase<Application>;

	Application ()          = default;
	~Application() override = default;
};