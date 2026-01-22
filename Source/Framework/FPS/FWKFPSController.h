#pragma once

//=======================================================================
// アプリケーションの"FPS"制御 + 測定
// ゲーム内の全オブジェクトがフレームレートに依存しなくていいようになる
//=======================================================================
namespace FWK
{
	class FPSController final
	{
	public:
	
		FPSController () = default;
		~FPSController() = default;
	
		void Init           ();
		void UpdateStartTime();
		void Update         ();
	
		float GetDeltaTime      () const { return m_deltaTime;       }
		float GetScaledDeltaTime() const { return m_scaledDeltaTime; }
		
		float GetTimeScale() const { return m_timeScale;  }
		
		int GetMaxFPS    () const { return m_maxFps;     }
		int GetNowFPS    () const { return m_nowFps;     }
		int GetDefaultFPS() const { return k_defaultFPS; }
	
		void SetTimeScale(const float a_set) { m_timeScale = a_set; }
	
	private:
	
		void Control   () const;
		void Monitoring();

		static constexpr float k_maxTimeScale = 1.0F;
		static constexpr float k_second	      = 1.0F;
		static constexpr int   k_defaultFPS   = 60;
		static constexpr int   k_milliSecond  = 1000;

		std::chrono::steady_clock::time_point m_previousTime   = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point m_frameBeginTime = std::chrono::steady_clock::now();
	
		float m_deltaTime       = 0.0F;
		float m_timeScale       = 0.0F;
		float m_scaledDeltaTime = 0.0F;
	
		int m_maxFps = 0;
		int m_nowFps = 0;
	};
}