#pragma once

class Prefab;
class GameObject;

class PrefabController
{

public:

	PrefabController () = default;
	~PrefabController() = default;

	auto& GetPrefabList() { return m_prefabList; }

	void AddPrefab(const std::string& PrefabKey , const std::shared_ptr<Prefab> Prefab);

	std::weak_ptr<Prefab> GetPrefabData(const std::string& PrefabName);

private:

	std::unordered_map <std::string, std::shared_ptr<Prefab>, CommonStruct::StringHash, std::equal_to<>> m_prefabList;

};