#include "SceneViewImGuiWindow.h"

#include "../../ImGuiManager.h"

#include "../../Config/SceneView/SceneViewImGuiConfig.h"

#include "../../../../Application/main.h"
#include "../../../../Application/Utility/Common/CommonStruct.h"
#include "../../../../Application/Utility/ImGui/ImGuiUtility.h"

#include "../../../../Application/GameObject/GameObject.h"

#include "../../../../Application/Scene/SceneManager.h"

void SceneViewImGuiWindow::Update()
{
	auto& imGuiManager_ = ImGuiManager::GetInstance();

	if (ImGui::Begin("SceneView"))
	{
		ImGuiUtility::EnumRadioButtonSelector("SceneSelector", SceneManager::GetInstance().GetWorkNextSceneType(), ImGuiUtility::SCENE_TYPE_LIST);

		if (auto sceneViewImGuiConfig_ = imGuiManager_.GetImGuiConfig<SceneViewImGuiConfig>().lock())
		{
			UpdateSceneViewWindow(sceneViewImGuiConfig_);
		}
	}
	ImGui::End();
}

void SceneViewImGuiWindow::UpdateSceneViewWindow(std::shared_ptr<SceneViewImGuiConfig> SceneViewImGuiConfig)
{
	if (!SceneViewImGuiConfig) { return; }

	const ImTextureID textureID_ = (ImTextureID)(SceneViewImGuiConfig->GetRenderTargetPack().m_RTTexture->WorkSRView());
	const ImVec2      imageSize_ = { static_cast<float>(SceneViewImGuiConfig->GetSceneViewWindowSize().width) , static_cast<float>(SceneViewImGuiConfig->GetSceneViewWindowSize().height) };

	const ImVec2 imagePosition_ = ImGui::GetCursorScreenPos();
	// レンダーターゲットで取得したゲームシーンをテクスチャとして描画
	ImGui::Image(textureID_ , imageSize_);

	// "ImVec2"はコンストラクタで値を代入している都合上変数として持てないため
	// ここでマウスの情報を書き込む
	const ImVec2 cursorPos_ = ImGui::GetIO().MousePos;

	// クライアント座標との差異が"X,Y"両方"0.0f"以上で"imageSize_.x , imageSize_.y"よりも小さくなければ
	// レンダーターゲットで小さくなったスクリーンをタッチしているとみなされないということ
	const ImVec2 difference_ = cursorPos_ - imagePosition_;

	const bool isMouseInScreenX_ = difference_.x >= 0.0f && difference_.x < imageSize_.x;
	const bool isMouseInScreenY_ = difference_.y >= 0.0f && difference_.y < imageSize_.y;

	m_isCursorInScreen = false;

	// マウスがゲームシーンの上にあるかどうかを判定
	// 仕様上ここでしかゲームシーンウィンドウ内にカーソルがあるかどうかを判定できないからここで判定
	if (ImGui::IsItemHovered() && isMouseInScreenX_ && isMouseInScreenY_)
	{
		m_isCursorInScreen = true;
		
		// マウスの真ん中がクリックされたら座標をログに追加
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		{
			const auto&         application_ = Application::Instance();
			const Math::Vector2 windowSize_  = { static_cast<float>(application_.GetWindowSize().width) , static_cast<float>(application_.GetWindowSize().height) };
 
			// レンダーターゲットで縮小したスクリーンのサイズと本来のスクリーンのサイズを割ってやることで補正倍率を取得("1280.0f / 640.0f = 2.0f")
			// レンダーターゲット上でのスクリーン座標が "{ 100.0f , 0.0f }"だった場合補正倍率("2.0f")をかけてやることで"{ 200.0f , 0.0f }"となり本来のスクリーン座標ではどの位置を
			// 指しているのかを求めることができる
			const ImVec2 correctionValue_ = { windowSize_.x / imageSize_.x       , windowSize_.y / imageSize_.y       };
			const ImVec2 mousePosition_   = { difference_.x * correctionValue_.x , difference_.y * correctionValue_.y };

			POINT clickedCursorPosition_ = { static_cast<LONG>(mousePosition_.x) , static_cast<LONG>(mousePosition_.y) };

			KdDebugGUI::Instance().AddLog("\nMousePosition\nX : %d\nY : %d\n" , clickedCursorPosition_.x , clickedCursorPosition_.y);
		}
	}
}