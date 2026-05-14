#include "PrefabController.h"
#include "Prefab.h"

void PrefabController::AddPrefab(const std::string& PrefabKey, const std::shared_ptr<Prefab> Prefab)
{
	m_prefabList.try_emplace(PrefabKey , Prefab);
}

std::weak_ptr<Prefab> PrefabController::GetPrefabData(const std::string& PrefabName)
{
	auto itr_ = m_prefabList.find(PrefabName);

	if (itr_ != m_prefabList.end())
	{
		// ヌルチェック
		if (itr_->second)
		{
			return itr_->second;
		}
	}

	return std::weak_ptr<Prefab>();
}