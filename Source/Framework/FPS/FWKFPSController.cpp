#include "FWKFPSController.h"

void FWK::FPSController::Init()
{
	// 初期化時に過去の時間を計測
	m_previousTime = std::chrono::steady_clock::now();

	// モニタリング用の時刻も過去の時間に収める
	m_frameBeginTime = m_previousTime;

	m_deltaTime       = 0.0F;
	m_timeScale       = k_maxTimeScale;
	m_scaledDeltaTime = 0.0F;

	m_maxFps = k_defaultFPS;
	m_nowFps = 0;

	// 初期化時に更新しおかしな値が入らないようにする
	Update();
}

void FWK::FPSController::UpdateStartTime()
{
	// 現在のフレームでの時間
	m_frameBeginTime = std::chrono::steady_clock::now();

	// デルタタイム(一フレーム前の時間と現フレームでの時間を引く)
	std::chrono::duration<float> l_elapsed = m_frameBeginTime - m_previousTime;

	// 秒単位で取得(ミリ秒で取得する必要がある)
	m_deltaTime = l_elapsed.count();

	// "TimeScale"の影響を受けるデルタタイム
	m_scaledDeltaTime = m_deltaTime * m_timeScale;
}

void FWK::FPSController::Update()
{
	Control   ();
	Monitoring();

	// Updateが終わるつまり前回の計測時刻として現在の時刻を入れる
	m_previousTime = m_frameBeginTime;
}

// FPS制御
void FWK::FPSController::Control() const
{
	// 現在の時刻を取得
	const std::chrono::steady_clock::time_point l_currentTime = std::chrono::steady_clock::now();

	// "1000"ミリ秒 / "60"フレーム
	const auto l_frameTime = std::chrono::milliseconds(k_milliSecond / m_maxFps);

	// 現在の時刻を過去の時間と引いてやることで経過時間を算出
	const auto l_elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(l_currentTime - m_frameBeginTime);

	if (l_elapsedTime < l_frameTime)
	{
		// 超過した時間分スリープをかます
		std::this_thread::sleep_for(l_frameTime - l_elapsedTime);
	}
}

// 現在のFPS計測
void FWK::FPSController::Monitoring()
{
	if (m_deltaTime > 0.0F)
	{
		// "Fps" = "1.0f"(秒) / DeltaTime(ミリ秒)
		m_nowFps = static_cast<int>(k_second / m_deltaTime);
	}
	else
	{
		m_nowFps = m_maxFps;
	}
}