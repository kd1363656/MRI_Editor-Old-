#pragma once

class GameObject;

class Prefab
{

public:

	Prefab () = default;
	~Prefab() = default;

	std::weak_ptr<GameObject> GetGameObject() const { return m_gameObject; }

	const nlohmann::json& GetJsonData() const { return m_jsonData; }

	void SetGameObject(std::weak_ptr<GameObject> Set) { m_gameObject = Set; }

	void SetJsonData(const nlohmann::json& Set) { m_jsonData = Set; }

private:

	std::weak_ptr<GameObject> m_gameObject;

	nlohmann::json m_jsonData = nlohmann::json();
};