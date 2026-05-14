#pragma once
#include "../FileSaveAndLoadBase.h"

class GameObject;

class GameObjectFileSaveAndLoad : public FileSaveAndLoadBase
{

public:

	GameObjectFileSaveAndLoad ()          = default;
	~GameObjectFileSaveAndLoad() override = default;

	void SaveFile() override;
	void LoadFile() override;

	void LoadGameObjectData(const std::string_view PrefabName , const nlohmann::json& SpawnJsonData, const nlohmann::json& PrefabJsonData) const;
};