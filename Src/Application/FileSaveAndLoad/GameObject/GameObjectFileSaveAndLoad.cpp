#include "GameObjectFileSaveAndLoad.h"

#include "../../Scene/SceneManager.h"
#include "../../Scene/BaseScene/BaseScene.h"

#include "../../GameObject/GameObject.h"

#include "../../Resource/ResourceController.h"
#include "../../Resource/Prefab/Prefab.h"
#include "../../Resource/Prefab/PrefabController.h"

#include "../../Utility/Json/JsonUtility.h"
#include "../../Utility/BitShift/BitShiftUtility.h"

#include "../../../System/FileSystem/FileSystem.h"

void GameObjectFileSaveAndLoad::SaveFile()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto jsonArray_ = nlohmann::json::array();
	
	// とりあえずスポーン情報のみ保存
	for(const auto& gameObject_ : scene_->GetGameObjectList())
	{
		if (gameObject_->HasParent()) { continue; }

		jsonArray_.emplace_back(gameObject_->SerializeSpawnData());
	}

	FileSystem::SaveJsonFile(jsonArray_ , m_saveAndLoadFilePath);
}
void GameObjectFileSaveAndLoad::LoadFile()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	// シーンで読み込むゲームオブジェクトのファイルを決めているので"Jsonファイルが読み込めなければ
	// "SceneBase"を継承しているゲームシーンが怪しい
	if (m_saveAndLoadFilePath.empty())
	{
		KdDebugGUI::Instance().AddLog("ゲームオブジェクトのリストファイル読み込みに失敗しました , シーンで文字列を読み込めていません");
		return;
	}

	auto spawnJsonData_ = FileSystem::LoadJsonFile(m_saveAndLoadFilePath);

	if(spawnJsonData_.is_null())
	{
		KdDebugGUI::Instance().AddLog("Jsonファイルの読み込みに失敗しました");
		return;
	}

	// "SpawnDataJson"と"PrefabDataJson"に同じプレハブの名前があった際に
	// ゲームオブジェクトを生成できる
	if (!spawnJsonData_.is_array())
	{
		KdDebugGUI::Instance().AddLog("Jsonファイルの読み込みに失敗しました");
		return;
	}

	for (const auto& spawnData_ : spawnJsonData_)
	{
		// 読み込みたいプレハブデータのキーとして使う
		const std::string prefabName_ = spawnData_.value("PrefabName", "");

		if (auto prefabDataJson_ = prefabController_->GetPrefabData(prefabName_).lock())
		{
			const nlohmann::json prefabData_ = prefabDataJson_->GetJsonData();

			LoadGameObjectData(prefabName_, spawnData_, prefabData_);
		}
	}

	// ImGui用にゲームオブジェクトのポインタを一つだけ格納する
	for (const auto& gameObject_ : scene_->GetGameObjectList())
	{
		if (!gameObject_) { continue; }

		auto prefab_ = prefabController_->GetPrefabData(gameObject_->GetPrefabName().data()).lock();
		if (!prefab_) { continue; }

		auto prefabGameObject_ = prefab_->GetGameObject().lock();

		if (!prefabGameObject_)
		{ 
			prefabGameObject_ = gameObject_;
		}
	}
}

void GameObjectFileSaveAndLoad::LoadGameObjectData(const std::string_view PrefabName, const nlohmann::json& SpawnJsonData, const nlohmann::json& PrefabJsonData) const
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	if (SpawnJsonData.is_null ()) { return; }
	if (PrefabJsonData.is_null()) { return; }

	auto gameObject_ = std::make_shared<GameObject>();
	gameObject_->Init();
	gameObject_->Deserialize(PrefabName , SpawnJsonData , PrefabJsonData);

	scene_->AddGameObject(gameObject_);
}