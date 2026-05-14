#pragma once
#include "IBTBlackBoardData.h"

template <typename DerivedClass , typename TypeInfo>
class BTBlackBoardDataBase : public IBTBlackBoardData
{

public:

	BTBlackBoardDataBase ()          = default;
	~BTBlackBoardDataBase() override = default;

	// ※注意 : いつもの静的"ID"取得とは違う
	uint32_t GetDataTypeID() const override { return StaticID<IBTBlackBoardData>::GetUnrestrictedTypeID<TypeInfo>(); }
	uint32_t GetTypeID    () const override { return StaticID<IBTBlackBoardData>::GetTypeID<DerivedClass>		 (); }

	void Init        () override { /* 必要に応じてオーバーライドしてください */ }
	void PostLoadInit() override { /* 必要に応じてオーバーライドしてください */ }

	void ImGuiSpawnDataInspector () override { /* 必要に応じてオーバーライドしてください */ }
	void ImGuiPrefabDataInspector() override { /* 必要に応じてオーバーライドしてください */ }

	void DeserializeSpawnData (const nlohmann::json& Json) override { /* 必要に応じてオーバーライドしてください */ }
	void DeserializePrefabData(const nlohmann::json& Json) override { /* 必要に応じてオーバーライドしてください */ }

	nlohmann::json SerializeSpawnData () override { return nlohmann::json(); }
	nlohmann::json SerializePrefabData() override { return nlohmann::json(); }

	void Set   (TypeInfo        Set) { m_value = Set; }
	void SetRef(const TypeInfo& Set) { m_value = Set; }

	TypeInfo        Get   () const { return m_value; }
	const TypeInfo& GetRef() const { return m_value; }

private:

	TypeInfo m_value = {};

};