#pragma once

class GameObject;

class CommonCoreBase
{

public:

	CommonCoreBase         () = default;
	virtual ~CommonCoreBase() = default;

	virtual void Init        ()                                  { /* 必要に応じてオーバーライドしてください */ }
	virtual void PostLoadInit(std::shared_ptr<GameObject> Owner) { /* 必要に応じてオーバーライドしてください */ }

	virtual void ImGuiSpawnDataInspector () { /* 必要に応じてオーバーライドしてください */ }
	virtual void ImGuiPrefabDataInspector() { /* 必要に応じてオーバーライドしてください */ }

	virtual void DeserializeSpawnData (const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }
	virtual void DeserializePrefabData(const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }

	virtual nlohmann::json SerializeSpawnData (){ return nlohmann::json(); }
	virtual nlohmann::json SerializePrefabData(){ return nlohmann::json(); }
	
};