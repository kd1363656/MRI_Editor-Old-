#include "ImGuiManager.h"

#include "Window/GUID/GUIDImGuiWindow.h"
#include "Window/Hierarchy/HierarchyImGuiWindow.h"
#include "Window/Inspector/Config/PrefabRegistrationDataList/PrefabRegistrationDataListInspectorImGuiWindow.h"
#include "Window/Inspector/SpawnData/SpawnDataInspectorImGuiWindow.h"
#include "Window/Inspector/PrefabData/PrefabDataInspectorImGuiWindow.h"
#include "Window/Inspector/PrefabData/PrefabSceneFilePathListInspectorImGuiWindow.h"
#include "Window/MenuBar/MenuBarImGuiWindow.h"
#include "Window/BehaviorTree/BehaviorTreeImGuiWindow.h"
#include "Window/SceneView/SceneViewImGuiWindow.h"
#include "Window/InputCheck/InputCheckImGuiWindow.h"
#include "Window/TerrainCreate/TerrainCreateImGuiWindow.h"

#include "Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"
#include "Config/SceneView/SceneViewImGuiConfig.h"

#include "../../Application/Scene/BaseScene/BaseScene.h"
#include "../../Application/Scene/SceneManager.h"

#include "../../Application/GameObject/GameObject.h"

#include "../../Application/FileSaveAndLoad/GameObject/GameObjectFileSaveAndLoad.h"

#include "../../Application/Utility/Common/CommonStruct.h"
#include "../../Application/Utility/BitShift/BitShiftUtility.h"
#include "../FileSystem/FileSystem.h"

void ImGuiManager::Init()
{
	// コンフィグの生成と初期化
	CreateImGuiConfig<PrefabRegistrationDataListImGuiConfig>();
	CreateImGuiConfig<SceneViewImGuiConfig>                 ();

	for (const auto& [typeID_, config_] : m_imGuiConfigList)
	{
		config_->Init();
	}

	// ウィンドウの生成と初期化
	CreateImGuiWindow<SceneViewImGuiWindow>							 ();
	CreateImGuiWindow<MenuBarImGuiWindow>                            ();
	CreateImGuiWindow<GUIDImGuiWindow>								 ();
	CreateImGuiWindow<HierarchyImGuiWindow>                          ();
	CreateImGuiWindow<PrefabDataInspectorImGuiWindow>                ();
	CreateImGuiWindow<PrefabRegistrationDataListInspectorImGuiWindow>();
	CreateImGuiWindow<PrefabSceneFilePathListInspectorImGuiWindow>   ();
	CreateImGuiWindow<SpawnDataInspectorImGuiWindow>                 ();
	CreateImGuiWindow<InputCheckImGuiWindow>                         ();
	CreateImGuiWindow<TerrainCreateImGuiWindow>                      ();
	CreateImGuiWindow<BehaviorTreeImGuiWindow>					     ();

	for (const auto& [typeID_ , window_] : m_imGuiWindowList)
	{
		window_->Init();
	}
}

void ImGuiManager::Update()
{
	//ImGui::ShowDemoWindow();

	for (const auto& [typeID_, window_] : m_imGuiWindowList)
	{
		window_->Update();
	}

	for (const auto& [typeID_ , config_] : m_imGuiConfigList)
	{
		config_->Update();
	}

	UpdatePopUpFlag();
}

bool ImGuiManager::IsCursorInScreen()
{
	auto sceneViewImGuiWindow_ = GetImGuiWindow<SceneViewImGuiWindow>().lock();
	
	if (!sceneViewImGuiWindow_) 
	{
		return false; 
	}

	return sceneViewImGuiWindow_->GetIsCursorInScreen();
}

void ImGuiManager::UpdatePopUpFlag()
{
	UpdatePopUpFlag(ImGuiManager::PopUpFlg::Save   , "Save   Completed");
	UpdatePopUpFlag(ImGuiManager::PopUpFlg::ReLoad , "ReLoad Completed");
}

void ImGuiManager::UpdatePopUpFlag(const ImGuiManager::PopUpFlg StandFlg, const char* const Label)
{
	if (BitShiftUtility::IsStandFlag(StandFlg , m_popUpFlag))
	{
		ImGui::OpenPopup(Label);
		BitShiftUtility::DisableFlag(StandFlg , m_popUpFlag);
	}

	// セーブ完了モーダルポップアップの描画
	if (ImGui::BeginPopupModal(Label , nullptr , ImGuiWindowFlags_AlwaysAutoResize))
	{
		if(ImGui::Button("/=========Ok==========/"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}