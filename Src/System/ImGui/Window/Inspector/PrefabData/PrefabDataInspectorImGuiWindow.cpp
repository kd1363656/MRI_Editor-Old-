#include "PrefabDataInspectorImGuiWindow.h"

#include "../../../../FileSystem/FileSystem.h"

#include "../../../ImGuiManager.h"
#include "../../../Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"
#include "../Config/PrefabRegistrationDataList/PrefabRegistrationDataListInspectorImGuiWindow.h"

#include "../Src/Application/Scene/SceneManager.h"
#include "../Src/Application/Scene/BaseScene/BaseScene.h"

#include "../Src/Application/GameObject/GameObject.h"

#include "../Src/Application/FileSaveAndLoad/Prefab/PrefabFileSaveAndLoad.h"

#include "../Src/Application/Observer/Component/ComponentObserver.h"

#include "../Src/Application/Resource/ResourceController.h"
#include "../Src/Application/Resource/Prefab/PrefabController.h"
#include "../Src/Application/Resource/Prefab/Prefab.h"

#include "../Src/Application/Utility/ImGui/ImGuiUtility.h"

void PrefabDataInspectorImGuiWindow::Update()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	UpdateAddPrefabData();

	// ゲームオブジェクトのプレハブ
	if (ImGui::Begin("PrefabDataInspector"))
	{
		for(const auto& [key_ , value_] : prefabController_->GetPrefabList())
		{
			if (auto gameObject_ = value_->GetGameObject().lock())
			{
				UpdateComponentInspector(gameObject_);
			}
		}
	}
	ImGui::End();
}

void PrefabDataInspectorImGuiWindow::SavePrefabData()
{
	auto scene_             = SceneManager::GetInstance().GetCurrentScene().lock();
	auto prefabImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();

	if (!scene_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	// コンフィグからプレハブの保存すべき場所にプレハブを保存する
	for(const auto& [prefabKey_ , prefabValue_] : prefabController_->GetPrefabList())
	{
		// ヌルチェック
		if (!prefabValue_) { continue; }

		auto gameObject_ = prefabValue_->GetGameObject().lock();
		if (!gameObject_) { continue; }

		for(const auto& [configKey_ , configValue_] : prefabImGuiConfig_->GetPrefabRegistrationDataList())
		{
			if (prefabKey_ != configKey_) { continue; }
			
			const nlohmann::json json_ = gameObject_->SerializePrefabData();
			FileSystem::SaveJsonFile(json_ , configValue_.prefabFilePath);
		}
	}
}

void PrefabDataInspectorImGuiWindow::UpdateAddPrefabData() const
{
	auto scene_                    = SceneManager::GetInstance().GetCurrentScene().lock                                               ();
	auto prefabImGuiConfig_        = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock         ();
	auto prefabImGuiConfigWindow_  = ImGuiManager::GetInstance().GetImGuiWindow<PrefabRegistrationDataListInspectorImGuiWindow>().lock();
	
	// ヌルチェックしてるだけ
	if (!scene_ || !prefabImGuiConfig_ || !prefabImGuiConfigWindow_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	for(const auto& [key_ , value_] : prefabImGuiConfig_->GetPrefabRegistrationDataList())
	{
		auto itr_ = prefabController_->GetPrefabList().find(key_);

		// もしプレハブのリストにレジストリリストの要素がなければプレハブを追加
		if (itr_ == prefabController_->GetPrefabList().end())
		{	
			auto prefab_ = std::make_shared<Prefab>();

			prefabController_->GetPrefabList().try_emplace(key_ , prefab_);
		}
	}

	// 追加したプレハブの変更結果を受けるゲームオブジェクトを一つ選定
	// プレハブの保存を行うとほかのゲームオブジェクトにも設定内容が反映される仕組み
	for(const auto& [key_ , value_] : prefabController_->GetPrefabList())
	{
		for(const auto& gameObjectList_ : scene_->GetGameObjectList())
		{
			if (value_->GetGameObject().lock()) { continue; }

			// プレハブ名が同じならそのゲームオブジェクトのポインタを渡す
			if(gameObjectList_->GetPrefabName() == key_)
			{
				value_->SetGameObject(gameObjectList_);
			}
		}
	}
}

void PrefabDataInspectorImGuiWindow::UpdateComponentInspector(std::shared_ptr<GameObject> GameObject) const
{
	if (!GameObject) { return; }

	ImGui::Separator();

	if (ImGui::TreeNodeEx(GameObject->GetPrefabName().data()))
	{
		// ユニークコンポーネントの"ImGui"
		ImGui::Text("Unique");

		for (const auto& [key_, value_] : GameObject->GetUniqueComponentMap())
		{
			auto component_ = value_.lock();
			if (!component_) { continue; }

			ImGui::PushID(component_.get());

			// コンポーネント内にあるインスペクターを表示
			if (ImGui::TreeNodeEx(component_->GetTypeName().data()))
			{
				component_->ImGuiPrefabDataInspector();
				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		// マルチコンポーネントの"ImGui"
		ImGui::Text("Multi");

		// マルチコンポーネントの"ImGui"
		for (const auto& [key_ , value_] : GameObject->GetMultiComponentMap())
		{
			for (const auto& list_ : value_)
			{
				auto component_ = list_.lock();
				if (!component_) { continue; }

				ImGui::PushID(component_.get());

				// コンポーネント内にあるインスペクターを表示
				if(ImGui::TreeNode(component_->GetTypeName().data()))
				{
					component_->ImGuiPrefabDataInspector();
					ImGui::TreePop();
				}

				ImGui::PopID();
			}
		}

		ImGui::Text("ComponentObserver");
		if (auto componentObserver_ = GameObject->GetComponentObserver().lock())
		{
			componentObserver_->ImGuiPrefabDataInspector();
		}

		ImGui::TreePop();
	}

	ImGui::Separator();
}