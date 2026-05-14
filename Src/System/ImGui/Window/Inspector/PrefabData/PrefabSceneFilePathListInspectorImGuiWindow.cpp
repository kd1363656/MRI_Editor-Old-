#include "PrefabSceneFilePathListInspectorImGuiWindow.h"

#include "../Src/Application/Scene/BaseScene/BaseScene.h"
#include "../Src/Application/Scene/SceneManager.h"

#include "../Src/Application/FileSaveAndLoad/Prefab/PrefabFileSaveAndLoad.h"

#include "../Src/System/ImGui/ImGuiManager.h"
#include "../Src/System/ImGui/Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"

void PrefabSceneFilePathListInspectorImGuiWindow::Update()
{
	auto& sceneManager_ = SceneManager::GetInstance();
	auto  scene_ = sceneManager_.GetCurrentScene().lock();

	// ヌルチェック
	if (!scene_) { return; }

	auto prefabFileSaveAndLoad_ = scene_->GetPrefabFileSaveAndLoad().lock();
	if (!prefabFileSaveAndLoad_) { return; }

	// シーンで読み込むプレハブのリストの一部を削除
	std::erase_if(prefabFileSaveAndLoad_->GetPrefabFilePathList(), [](const auto& Pair) -> bool
	{
		return Pair.second.deleteRequested;
	});

	if (ImGui::Begin("PrefabSceneFilePathListInspector"))
	{
		UpdateAddScenePrefabFilePath          ();
		UpdateScenePrefabFilePathListInspector();
	}
	ImGui::End();
}

void PrefabSceneFilePathListInspectorImGuiWindow::UpdateAddScenePrefabFilePath()
{
	auto& sceneManager_ = SceneManager::GetInstance();
	auto  scene_ = sceneManager_.GetCurrentScene().lock();
	auto  imGuiPrefabRegistrationDataList_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();

	// ヌルチェック
	if (!scene_ || !imGuiPrefabRegistrationDataList_) { return; }

	auto prefabFileSaveAndLoad_ = scene_->GetPrefabFileSaveAndLoad().lock();
	if (!prefabFileSaveAndLoad_) { return; }

	// シーンでロードするプレハブデータを選ぶ
	if (ImGui::BeginCombo("##PrefabFileSaveAndLoad", m_selectedSceneLoadPrefabName.c_str()))
	{
		for (const auto& [key_, value_] : imGuiPrefabRegistrationDataList_->GetPrefabRegistrationDataList())
		{
			bool isSelected_ = m_selectedSceneLoadPrefabName == key_;

			// 選択したプレハブ名のファイルパスを格納
			if (ImGui::Selectable(key_.c_str(), &isSelected_))
			{
				m_selectedSceneLoadPrefabName = key_;

				CommonStruct::PrefabSaveAndLoadFilePath saveAndLoadFilePath_ = {};
				saveAndLoadFilePath_.filePath = value_.prefabFilePath;

				prefabFileSaveAndLoad_->AddPrefabFilePath(key_, saveAndLoadFilePath_);
			}

			// 選択されていたら反映
			if (isSelected_)
			{
				// コンボボックスを開いたとき、キーボード操作（↑↓）で選択を開始できるように、
				// 現在選択されている項目に初期フォーカスを設定する
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
}

void PrefabSceneFilePathListInspectorImGuiWindow::UpdateScenePrefabFilePathListInspector()
{
	auto& sceneManager_ = SceneManager::GetInstance();
	auto  scene_ = sceneManager_.GetCurrentScene().lock();

	// ヌルチェック
	if (!scene_) { return; }

	auto prefabFileSaveAndLoad_ = scene_->GetPrefabFileSaveAndLoad().lock();
	if (!prefabFileSaveAndLoad_) { return; }

	ImGui::Separator();

	// リスト内のプレハブ名とファイルパスを調べる
	for (auto& [key_ , value_] : prefabFileSaveAndLoad_->GetPrefabFilePathList())
	{
		ImGui::PushID(&value_);

		ImGui::Text("PrefabName : %s" , key_.c_str           ());
		ImGui::SameLine();

		if(ImGui::SmallButton("Delete"))
		{
			value_.deleteRequested = true;
		}

		ImGui::Text("FilePath   : %s" , value_.filePath.c_str());

		ImGui::Separator();
		ImGui::PopID();
	}
}