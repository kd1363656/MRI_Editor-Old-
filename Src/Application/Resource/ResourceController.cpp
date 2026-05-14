
#include "ResourceController.h"

#include "../Scene/SceneManager.h"
#include "../Scene/BaseScene/BaseScene.h"

#include "Prefab/PrefabController.h"

#include "../FileSaveAndLoad/GameObject/GameObjectFileSaveAndLoad.h"
#include "../FileSaveAndLoad/Prefab/PrefabFileSaveAndLoad.h"

void ResourceController::ClearCachedAssets()
{
	KdAssets::Instance().ClearData(true);
}

void ResourceController::Init()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	// プレハブコントローラーの初期化
	{
		if (!m_prefabController)
		{
			m_prefabController = std::make_shared<PrefabController>();
		}
	}
}