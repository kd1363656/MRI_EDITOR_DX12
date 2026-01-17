#pragma once

namespace FWK
{
	class Window final
	{
	public:

		Window () = default;
		~Window() = default;

		bool Create(const FWK::CommonStruct::Dimension2D& a_size , const std::string& a_titleName , const std::string& a_windowClassName);

		bool ProcessMessage() const;

		void Release();

		bool IsCreated() const { return m_hWND ? true : false; }

		HWND GetHWND() const { return m_hWND; }

		// 現在実行中のウィンドウの実行ファイル(".exe")のインスタンスハンドルを返す
		// どのアプリケーションがこのウィンドウを作成したかを識別することができる
		HINSTANCE GetInstanceHandle() const { return GetModuleHandle(nullptr); }

	private:

		void SetClientSize(const FWK::CommonStruct::Dimension2D& a_size);

		// "CALLBACK"マクロで"__stdcall"に設定
		static LRESULT CALLBACK CallWindowProcedure(const HWND   a_hWND    , 
													const UINT   a_message , 
													const WPARAM a_wParam  , 
													const LPARAM a_lParam);
		
		LRESULT CALLBACK WindowProcedure(const HWND   a_hWND    , 
										 const UINT   a_message , 
										 const WPARAM a_wParam  , 
										 const LPARAM a_lParam);

		static constexpr LRESULT k_messageHandled = 0;

		static constexpr const wchar_t* const k_windowInstancePropName = L"GameWindowInstance";

		static constexpr UINT k_windowStyleDefault = 0U;

		static constexpr UINT k_timerResolutionMS = 1U;

		static constexpr UINT k_messageFilterNone = 0U;

		static constexpr int k_classExtraBytes  = 0;
		static constexpr int k_windowExtraBytes = 0;

		static constexpr int k_defaultWindowPosX = 0;
		static constexpr int k_defaultWindowPosY = 0;

		HWND m_hWND = nullptr;
	};
}