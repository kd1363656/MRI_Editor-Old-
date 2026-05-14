#include "PrefabFileSaveAndLoad.h"

#include "../../GameObject/GameObject.h"

#include "../../Scene/BaseScene/BaseScene.h"
#include "../../Scene/SceneManager.h"
#include "../../Scene/Changer/SceneChanger.h"

#include "../../Resource/ResourceController.h"
#include "../../Resource/Prefab/PrefabController.h"
#include "../../Resource/Prefab/Prefab.h"

#include "../../Utility/Json/JsonUtility.h"

#include "../../../System/FileSystem/FileSystem.h"

void PrefabFileSaveAndLoad::SaveFile()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto rootJson_  = nlohmann::json();
	auto arrayJson_ = nlohmann::json::array();

	// そのシーンで使うプレハブの名前とファイルパスを保存
	for (const auto& [key_, value_] : m_prefabFilePathList)
	{
		auto json_ = nlohmann::json();

		json_["PrefabName"    ] = key_;
		json_["PrefabFilePath"] = value_.filePath;

		arrayJson_.emplace_back(json_);
	}

	rootJson_["PrefabFilePathList"] = arrayJson_;

	FileSystem::SaveJsonFile(rootJson_ , m_saveAndLoadFilePath);
}
void PrefabFileSaveAndLoad::LoadFile()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	// 必要な"Json"ファイルのパスを保存している"Json"ファイルを読み込む
	// こうすることで"Json"ファイルの保管場所を奇麗にまとめることが出来てプログラマ以外の人でもパラメータの操作がしやすい
	{
		const auto json_ = FileSystem::LoadJsonFile(m_saveAndLoadFilePath);
		if (json_.is_null()) { return; }

		if (JsonUtility::CheckIsContainAndArray(json_ , "PrefabFilePathList"))
		{
			for (const auto& array_ : json_["PrefabFilePathList"])
			{
				CommonStruct::PrefabSaveAndLoadFilePath prefabFilePath_ = {};

				const std::string prefabName_ = array_.value("PrefabName", CommonConstant::STRING_UNKNOWN);

				prefabFilePath_.filePath = array_.value("PrefabFilePath", CommonConstant::ASSET_FOLDER_PATH);

				// リストにゲームオブジェクトのプレハブ名とそのファイルパスを渡す
				m_prefabFilePathList.try_emplace(prefabName_, prefabFilePath_);
			}
		}
	}

	// 取得したファイルパスリストからプレハブデータを読み込み
	// プレハブを作製
	{
		for (const auto& [key_, value_] : m_prefabFilePathList)
		{
			auto prefab_ = std::make_shared<Prefab>();

			const auto json_ = FileSystem::LoadJsonFile(value_.filePath);

			if (!json_.is_null())
			{
				prefab_->SetJsonData(json_);
				prefabController_->AddPrefab(key_, prefab_);
			}
		}
	}
}

void PrefabFileSaveAndLoad::AddPrefabFilePath(const std::string& PrefabName, const CommonStruct::PrefabSaveAndLoadFilePath& PrefabSaveAndLoadFilePath)
{
	m_prefabFilePathList.try_emplace(PrefabName, PrefabSaveAndLoadFilePath);
}