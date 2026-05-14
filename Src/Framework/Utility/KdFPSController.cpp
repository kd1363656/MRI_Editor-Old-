#include "KdFPSController.h"

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// FPSの制御コントローラー + DeltaTime(可変長フレームレート)
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
void KdFPSController::Init()
{
	m_maxFps = NORMAL_FPS;
	m_nowFps = 0;

	m_deltaTime = 0.0f;
	m_timeScale = MAX_TIMESCALE;

	m_scaledDeltaTime = 0.0f;

	// 初期化時に過去の時間を計測
	m_previousTime = std::chrono::steady_clock::now();

	// モニタリング用の時刻も過去の時間に収める
	m_frameBeginTime = m_previousTime;
}

void KdFPSController::UpdateStartTime()
{
	m_frameBeginTime = std::chrono::steady_clock::now();

	// デルタタイム(ゲームループでさっき取得した一フレーム前の時間と今の時間を引く)
	std::chrono::duration<float> elapsed_ = m_frameBeginTime - m_previousTime;
	// 秒単位で取得(ミリ秒で取得する必要がある)
	m_deltaTime = elapsed_.count();

	// "TimeScale"の影響を受けるデルタタイム
	m_scaledDeltaTime = m_deltaTime * m_timeScale;
}

void KdFPSController::Update()
{
	Control   ();
	Monitoring();

	// Updateが終わるつまり前回の計測時刻として現在の時刻を入れる
	m_previousTime = m_frameBeginTime;
}

// FPS制御
void KdFPSController::Control()
{
	// 現在の時刻を取得
	std::chrono::steady_clock::time_point currentTime_ = std::chrono::steady_clock::now();
	// 1000ミリ秒 / 60フレーム
	std::chrono::milliseconds             frameTime_   = std::chrono::milliseconds(MILLI_SECOND / m_maxFps);
	// 現在の時刻を過去の時間と引いてやることで経過時間を算出
	std::chrono::milliseconds elapsedTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime_ - m_frameBeginTime);

	if(elapsedTime_ < frameTime_)
	{
		// 超過した時間分スリープをかます
		std::this_thread::sleep_for(frameTime_ - elapsedTime_);
	}
}

// 現在のFPS計測
void KdFPSController::Monitoring()
{
	std::chrono::steady_clock::time_point currentTime_ = std::chrono::steady_clock::now();
	std::chrono::milliseconds             elapsedTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime_ - m_previousTime);

	if(m_deltaTime > 0.0f)
	{
		// Fps = 1.0(秒) / DeltaTime(ミリ秒)
		m_nowFps = static_cast<int>(SECOND / m_deltaTime);
	}
	else
	{
		m_nowFps = m_maxFps;
	}
}