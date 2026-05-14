#pragma once
#include "Application/Utility/Common/CommonStruct.h"
#include "Application/PlayerInputConfig/PlayerInputConfig.h"
#include "Application/Observer/Global/GlobalObserver.h"

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End()							{ m_endFlag = true; }

	const KdWindow& GetWindow() const { return m_window; }

	const PlayerInputConfig& GetPlayerInputConfig() const { return m_playerInputConfig; }
	PlayerInputConfig&       GetPlayerInputConfig()       { return m_playerInputConfig; }

	GlobalObserver& GetGlobalObserver() { return m_globalObserver; }

	const CommonStruct::WindowSize& GetWindowSize() const { return m_windowSize; }

	HWND GetWindowHandle()		const	{ return m_window.GetWndHandle();     }
	int  GetMouseWheelValue()	const	{ return m_window.GetMouseWheelVal(); }

	int GetNowFPS()				const	{ return m_fpsController.GetNowFPS(); }
	int GetMaxFPS()				const	{ return m_fpsController.GetMaxFPS(); }

	float GetNormalFPS      () const { return m_fpsController.GetNormalFPS(); }
	float GetRawDeltaTime   () const { return m_fpsController.GetDeltaTime(); }
	float GetScaledDeltaTime() const { return m_fpsController.GetScaledDeltaTime(); }
	float GetTimeScale      () const { return m_fpsController.GetTimeScale(); }

private:

	void KdBeginUpdate();
	void EarlyUpdate  ();
	void Update       ();
	void LateUpdate   ();
	void KdPostUpdate ();

	void KdBeginDraw(bool usePostProcess = true);
	void PreDraw();
	void Draw();
	void PostDraw();
	void DrawSprite();
	void KdPostDraw();

	// アプリケーション初期化
	bool Init(int w, int h);
	void InitFactory();
	void InitTag    ();

	// アプリケーション解放
	void Release();

	// ゲームウィンドウクラス
	KdWindow		m_window;

	// FPSコントローラー
	KdFPSController	m_fpsController;

	// キーコンフィグ
	PlayerInputConfig m_playerInputConfig;
	GlobalObserver    m_globalObserver;

	CommonStruct::WindowSize m_windowSize = {};

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;

//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Application() {}

public:
	static Application &Instance(){
		static Application Instance;
		return Instance;
	}
};
