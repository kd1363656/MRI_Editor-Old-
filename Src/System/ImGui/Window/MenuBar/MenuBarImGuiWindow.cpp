#include "MenuBarImGuiWindow.h"

#include "../../ImGuiManager.h"
#include "../../Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"
#include "../Inspector/PrefabData/PrefabDataInspectorImGuiWindow.h"

#include "../../../../Application/Scene/SceneManager.h"
#include "../../../../Application/Scene/BaseScene/BaseScene.h"

#include "../../../../Application/Component/Physics/DebugWire/PhysicsDebugWireComponent.h"

#include "../../../../Application/FileSaveAndLoad/GameObject/GameObjectFileSaveAndLoad.h"
#include "../../../../Application/FileSaveAndLoad/Prefab/PrefabFileSaveAndLoad.h"

#include "../../../../Application/GameObject/GameObject.h"

#include "../../../../Application/Utility/BitShift/BitShiftUtility.h"

void MenuBarImGuiWindow::Update()
{
	if (ImGui::Begin("MenuBar", nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save File"))
				{
					UpdateSaveAllDataWindow();
				}
				if (ImGui::MenuItem("ReLoad File"))
				{
					UpdateReLoadAllDataWindow();
				}

				ImGui::EndMenu();
			}
		}

		if (ImGui::BeginMenu("DebugWire"))
		{
			if (ImGui::MenuItem("DrawDebug"))
			{
				UpdateDrawDebugWireSelectWindow();
			}
			ImGui::EndMenu();
		}

		// メニューバーの終了
		ImGui::EndMenuBar();
	}
	ImGui::End();
}

void MenuBarImGuiWindow::UpdateSaveAllDataWindow() const
{
	auto& imGuiManager_ = ImGuiManager::GetInstance();
	auto  scene_        = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	// "GUID"の保存
	if (auto gameObjectGUID_ = scene_->GetGUIDGameObjectManager().lock())
	{
		gameObjectGUID_->SaveRegistry();
	}
	if (auto behaviorTreeGUID_ = scene_->GetGUIDBehaviorTreeManager().lock())
	{
		behaviorTreeGUID_->SaveRegistry();
	}

	// "SpawnData"と"PrefabList"データを保存
	if (auto fileSaveAndLoad_ = scene_->GetGameObjectFileSaveAndLoad().lock())
	{
		fileSaveAndLoad_->SaveFile();
	}
	if (auto fileSaveAndLoad_ = scene_->GetPrefabFileSaveAndLoad().lock())
	{
		fileSaveAndLoad_->SaveFile();
	}

	// "PrefabData"を保存
	if (auto prefabDataInspectorImGuiWindow_ = imGuiManager_.GetImGuiWindow<PrefabDataInspectorImGuiWindow>().lock())
	{
		prefabDataInspectorImGuiWindow_->SavePrefabData();
	}

	// "ImGui"を使う際の設定ファイルを保存
	for (const auto& [key_ , value_] : imGuiManager_.GetImGuiConfigList())
	{
		if (value_)
		{
			value_->SaveFile();
		}
	}

	BitShiftUtility::EnableFlag(ImGuiManager::PopUpFlg::Save, imGuiManager_.GetPopUpFlag());
}

void MenuBarImGuiWindow::UpdateReLoadAllDataWindow()
{
	auto& imGuiManager_ = ImGuiManager::GetInstance();
	auto& sceneManager_ = SceneManager::GetInstance();

	sceneManager_.ChangeScene(sceneManager_.GetCurrentSceneType());

	BitShiftUtility::EnableFlag(ImGuiManager::PopUpFlg::ReLoad , imGuiManager_.GetPopUpFlag());
}

void MenuBarImGuiWindow::UpdateDrawDebugWireSelectWindow()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	for (const auto& list_ : scene_->GetGameObjectList())
	{
		if (auto debugWire_ = list_->GetComponent<PhysicsDebugWireComponent>().lock())
		{
			if (!debugWire_->IsEnable())
			{
				debugWire_->Enable();
			}
			else
			{
				debugWire_->Disable();
			}
		}
	}
}