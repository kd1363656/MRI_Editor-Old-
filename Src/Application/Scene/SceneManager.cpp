#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"

#include "Changer/SceneChanger.h"

#include "../../System/ImGui/ImGuiManager.h"
#include "../../System/ImGui/Config/SceneView/SceneViewImGuiConfig.h"

void SceneManager::EarlyUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	// 入力処理を検知
	m_currentScene->EarlyUpdate();
}
void SceneManager::Update()
{
	m_currentScene->Update();

	// シーン遷移の更新
	if (m_nowSceneChanger)
	{
		m_nowSceneChanger->Update();
	}
}
void SceneManager::LateUpdate() const
{
	m_currentScene->LateUpdate();
}

void SceneManager::PreDraw()
{
	const auto  sceneViewImGuiConfig_   = ImGuiManager::GetInstance().GetImGuiConfig<SceneViewImGuiConfig>().lock();
	const bool  canShowDebugWindowFlag_ = KdDebugGUI::Instance().GetCanShowDebugWindowFlag						 ();
	
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		// 前バックバッファの情報をクリア
		sceneViewImGuiConfig_->GetRenderTargetPack().ClearTexture();
		// デバック用のシーンビューに今のバックバッファに描画されているものを描画
		sceneViewImGuiConfig_->GetRenderTargetChanger().ChangeRenderTarget(sceneViewImGuiConfig_->GetRenderTargetPack());
	}

	m_currentScene->PreDraw();

	// 描画をこのバックバッファに戻す
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		sceneViewImGuiConfig_->GetRenderTargetChanger().UndoRenderTarget();
	}
}

void SceneManager::Draw()
{
	const auto  sceneViewImGuiConfig_   = ImGuiManager::GetInstance().GetImGuiConfig<SceneViewImGuiConfig>().lock();
	const bool  canShowDebugWindowFlag_ = KdDebugGUI::Instance().GetCanShowDebugWindowFlag						 ();

	// 前情報のバッファはクリアしないなぜならこの関数より前に呼び出されるのが"PreDraw"関数だから
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		// デバック用のシーンビューに今のバックバッファに描画されているものを描画
		sceneViewImGuiConfig_->GetRenderTargetChanger().ChangeRenderTarget(sceneViewImGuiConfig_->GetRenderTargetPack());
	}

	m_currentScene->Draw();

	// 描画をこのバックバッファに戻す
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		sceneViewImGuiConfig_->GetRenderTargetChanger().UndoRenderTarget();
	}
}

void SceneManager::DrawSprite()
{
	const auto  sceneViewImGuiConfig_   = ImGuiManager::GetInstance().GetImGuiConfig<SceneViewImGuiConfig>().lock();
	const bool  canShowDebugWindowFlag_ = KdDebugGUI::Instance().GetCanShowDebugWindowFlag						 ();

	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		// デバック用のシーンビューに今のバックバッファに描画されているものを描画
		sceneViewImGuiConfig_->GetRenderTargetChanger().ChangeRenderTarget(sceneViewImGuiConfig_->GetRenderTargetPack());
	}

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 2Dの描画はこの間で行う
	// シーンチェンジもスプライトなのでここで"DrawCall"を行う(処理負荷を軽減するため)
	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		m_currentScene->DrawSprite();

		// シーン遷移の描画
		if (m_nowSceneChanger)
		{
			m_nowSceneChanger->Draw();
		}
	}
	KdShaderManager::Instance().m_spriteShader.End();

	// 描画をこのバックバッファに戻す
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		sceneViewImGuiConfig_->GetRenderTargetChanger().UndoRenderTarget();
	}
}

void SceneManager::DrawDebug()
{
	const auto  sceneViewImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<SceneViewImGuiConfig>().lock();
	const bool  canShowDebugWindowFlag_ = KdDebugGUI::Instance().GetCanShowDebugWindowFlag					   ();

	// 前情報のバッファはクリアしないなぜならこの関数より前に呼び出されるのが"PreDraw"関数だから
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		// デバック用のシーンビューに今のバックバッファに描画されているものを描画
		sceneViewImGuiConfig_->GetRenderTargetChanger().ChangeRenderTarget(sceneViewImGuiConfig_->GetRenderTargetPack());
	}

	m_currentScene->DrawDebug();

	// 描画をこのバックバッファに戻す
	if (sceneViewImGuiConfig_ && canShowDebugWindowFlag_)
	{
		sceneViewImGuiConfig_->GetRenderTargetChanger().UndoRenderTarget();
	}
}

void SceneManager::ChangeScene(SceneType _sceneType)
{
	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
		case SceneType::Title:
			m_currentScene = std::make_shared<TitleScene>();
		break;

		case SceneType::Game:
			m_currentScene = std::make_shared<GameScene>();
		break;
	}

	// シーンの初期化
	m_currentScene->Init();

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;
}