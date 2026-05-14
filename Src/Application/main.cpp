#include "main.h"

#include "../Application/Component/Transform/TransformComponent.h"

#include "../Application/Utility/Include/IncludeStrategyUtility.h"

#include "Scene/SceneManager.h"

#include "../Application/Factory/Comopnent/ComponentFactory.h"
#include "../Application/Factory/Strategy/StrategyFactory.h"
#include "../Application/Factory/Notify/Component/NotifyToComponentFactory.h"
#include "../Application/Factory/Modifier/Interpolator/InterpolatorFactory.h"
#include "../Application/Factory/BehaviorTree/Node/BTNodeFactory.h"
#include "../Application/Factory/RenderShaderParam/RenderShaderParamFactory.h"

#include "../Application/Observer/Component/Notify/Keep/KeepNotifyToComponent.h"
#include "../Application/Observer/Component/Notify/Moment/MomentNotifyToComponent.h"

#include "../Application/Tag/ComponentEvent/ComponentEventTag.h"
#include "../Application/Tag/TagRegistry.h"

#include "../System/Input/RawInputManager.h"

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// エントリーポイント
// アプリケーションはこの関数から進行する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR , _In_ int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		CoUninitialize();

		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginUpdate()
{
	// 入力状況の更新
	KdInputManager::Instance().Update();

	// 空間環境の更新
	KdShaderManager::Instance().WorkAmbientController().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostUpdate()
{
	// 3DSoundListnerの行列を更新
	KdAudioManager::Instance().SetListnerMatrix(KdShaderManager::Instance().GetCameraCB().mView.Invert());
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 入力や"AI"による処理要求を行う
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::EarlyUpdate()
{
	// 入力通知のクリアと更新
	m_globalObserver.BeginFrame    ();
	m_globalObserver.UpdateKeyEvent();

	SceneManager::GetInstance().EarlyUpdate();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 処理を行う
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////void Application::Update()
void Application::Update()
{
	SceneManager::GetInstance().Update();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 回転処理、 当たり判定、行列確定処理を行う(行列確定処理は"TransformComponent"が担う)
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::LateUpdate()
{
	SceneManager::GetInstance().LateUpdate();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginDraw(bool usePostProcess)
{
	KdDirect3D::Instance().ClearBackBuffer();

	KdShaderManager::Instance().WorkAmbientController().Draw();

	if (!usePostProcess) return;
	KdShaderManager::Instance().m_postProcessShader.Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostDraw()
{
	// Imguiのレンダリング
	KdDebugGUI::Instance().GuiProcess();

	// BackBuffer -> 画面表示
	KdDirect3D::Instance().WorkSwapChain()->Present(0, 0);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の前処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PreDraw()
{
	SceneManager::GetInstance().PreDraw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Draw()
{
	SceneManager::GetInstance().Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画の後処理
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::PostDraw()
{
	// 画面のぼかしや被写界深度処理の実施
	KdShaderManager::Instance().m_postProcessShader.PostEffectProcess();

	// 現在のシーンのデバッグ描画
	SceneManager::GetInstance().DrawDebug();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 2Dスプライトの描画
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::DrawSprite()
{
	SceneManager::GetInstance().DrawSprite();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション初期設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool Application::Init(int w, int h)
{
	// タイトル名 + Fpsの表示
	std::string TitleBar = "PROTO TYPE FPS : " + std::to_string(m_fpsController.GetNowFPS());

	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, TitleBar.c_str(), "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
//	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
//		bFullScreen = true;
//	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif

	// Direct3D初期化
	std::string errorMsg;
	if (KdDirect3D::Instance().Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		HRESULT hr;

		hr = KdDirect3D::Instance().SetFullscreenState(TRUE, 0);
		if (FAILED(hr))
		{
			MessageBoxA(m_window.GetWndHandle(), "フルスクリーン設定失敗", "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	//===================================================================
	// シェーダー初期化
	//===================================================================
	KdShaderManager::Instance().Init();

	//===================================================================
	// オーディオ初期化
	//===================================================================
	KdAudioManager::Instance().Init();

	//===================================================================
	// 入力の初期化
	//===================================================================
	RawInputManager::GetInstance().Init();

	//===================================================================
	// プレイヤーの入力コンフィグの初期化
	//===================================================================
	m_playerInputConfig.Init();

	//===================================================================
	// オブザーバーの初期化
	//===================================================================
	m_globalObserver.Init();

	//===================================================================
	// imGui初期化
	//===================================================================
	KdDebugGUI::Instance().GuiInit();

	//===================================================================
	// ファクトリーの初期化
	//===================================================================
	InitFactory();

	//===================================================================
	// タグの初期化
	//===================================================================
	InitTag();

	//===================================================================
	// エフェクシアマネージャーの初期化
	//===================================================================
	KdEffekseerManager::GetInstance().Create(w , h);

	//===================================================================
	// シーンマネージャーの初期化
	//===================================================================
	SceneManager::GetInstance().Init();
	return true;
}

void Application::InitFactory()
{
	// コンポーネントの登録
	ComponentFactory::GetInstance().Init();

	// 通知クラスの登録
	NotifyToComponentFactory::GetInstance().Init();

	// モディファイアーの登録
	InterpolatorModifierFactory::GetInstance().Init();

	// ビヘイビアノードの登録
	BTNodeFactory::GetInstance().Init();

	// ブラックボードデータの登録
	BTBlackBoardDataFactory::GetInstance().Init();

	// シェーダーデータの登録
	RenderShaderParamFactory::GetInstance().Init();

	// ストラテジーの登録
	StrategyFactory<TransformComponent>::GetInstance().RegisterFactoryMethod<TransformStrategyCreateLocalSRT>         ();
	StrategyFactory<TransformComponent>::GetInstance().RegisterFactoryMethod<TransformStrategyCreateLocalSTRParentSRT>();
}

void Application::InitTag()
{
	// タグの登録
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<AtkComboStartActiveTag>();

	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<AtkComboFirstActiveTag> ();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<AtkComboSecondActiveTag>();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<AtkComboThirdActiveTag> ();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<AtkComboFourthActiveTag>();

	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<MoveInputBasedOnTargetActiveTag>();

	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<MoveInputAtkComboFirstActiveTag> ();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<MoveInputAtkComboSecondActiveTag>();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<MoveInputAtkComboThirdActiveTag> ();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<MoveInputAtkComboFourthActiveTag>();
	
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<RotationInputSmoothBasedOnTargetActiveTag> ();
	TagRegistry<ComponentEventTagBase>::GetInstance().Registration<RotationInputInstantBasedOnTargetActiveTag>();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション実行
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Execute()
{
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);

	m_windowSize = { atoi(sizeData[0].c_str()), atoi(sizeData[1].c_str()) };

	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (Application::Instance().Init(m_windowSize.width , m_windowSize.height) == false) {
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間
	m_fpsController.Init();

	// ループ
	while (1)
	{
		// 処理開始時間Get
		m_fpsController.UpdateStartTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
//			if (MessageBoxA(m_window.GetWndHandle(), "本当にゲームを終了しますか？",
//				"終了確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
			{
				End();
			}
		}

		//=========================================
		//
		// アプリケーション更新処理
		//
		//=========================================

		KdBeginUpdate();
		{
			EarlyUpdate();

			Update();
			
			LateUpdate();
		}
		KdPostUpdate();

		//=========================================
		//
		// アプリケーション描画処理
		//
		//=========================================

		KdBeginDraw();
		{
			PreDraw();

			Draw();

			PostDraw();

			DrawSprite();
		}
		KdPostDraw();

		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		m_fpsController.Update();

		//=========================================
		//
		// 一フレーム前の入力の確認
		//
		//=========================================
		RawInputManager::GetInstance().BackUpInputState();
		
		// タイトル名 + Fpsの表示
		std::string TitleBar = "PROTO TYPE FPS : " + std::to_string(m_fpsController.GetNowFPS());
		SetWindowTextA(m_window.GetWndHandle(), TitleBar.c_str());
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}

// アプリケーション終了
void Application::Release()
{
	KdInputManager::Instance().Release();

	KdShaderManager::Instance().Release();

	KdAudioManager::Instance().Release();

	KdDirect3D::Instance().Release();

	// ウィンドウ削除
	m_window.Release();
}
