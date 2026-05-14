#pragma once

class GameObject;

class IBTBlackBoardData
{

public:

	IBTBlackBoardData         () = default;
	virtual ~IBTBlackBoardData() = default;

	virtual uint32_t GetDataTypeID() const = 0;
	virtual uint32_t GetTypeID    () const = 0;

	virtual void Init        () { /* 必要に応じてオーバーライドしてください */ }
	virtual void PostLoadInit() { /* 必要に応じてオーバーライドしてください */ }

	virtual void ImGuiSpawnDataInspector () { /* 必要に応じてオーバーライドしてください */ }
	virtual void ImGuiPrefabDataInspector() { /* 必要に応じてオーバーライドしてください */ }

	virtual void DeserializeSpawnData (const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }
	virtual void DeserializePrefabData(const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }

	virtual nlohmann::json SerializeSpawnData () { return nlohmann::json(); }
	virtual nlohmann::json SerializePrefabData() { return nlohmann::json(); }
};