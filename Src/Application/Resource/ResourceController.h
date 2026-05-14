#pragma once

class PrefabController;

// フライウェイトで管理しているリソースの開放、プレハブの管理、音の管理といったリソースを一括で管理するクラス
class ResourceController
{

public:

	ResourceController         () = default;
	virtual ~ResourceController() = default;

	void ClearCachedAssets();

	void Init();

	std::weak_ptr<PrefabController> GetPrefabController() const { return m_prefabController; }

protected:

	std::shared_ptr<PrefabController> m_prefabController = nullptr;
};