#pragma once
#include "Data/IBTBlackBoardData.h"

class GameObject;

class BTBlackBoard
{

public:

	BTBlackBoard () = default;
	~BTBlackBoard() = default;

	void PostLoadInit();

	void DeserializeSpawnData (const nlohmann::json& Json);
	void DeserializePrefabData(const nlohmann::json& Json);

	nlohmann::json SerializeSpawnData ();
	nlohmann::json SerializePrefabData();

	void ImGuiSpawnDataInspector ();
	void ImGuiPrefabDataInspector();

	template <typename BlackBoardDataType , typename TypeInfo>
		requires std::derived_from <BlackBoardDataType , IBTBlackBoardData>
	std::weak_ptr<BlackBoardDataType> GetSharedInfo()
	{
		const uint32_t typeID_     = StaticID<IBTBlackBoardData>::GetTypeID<BlackBoardDataType>  ();
		const uint32_t dataTypeID_ = StaticID<IBTBlackBoardData>::GetUnrestrictedTypeID<TypeInfo>();

		// もしリストに格納されていなければ"return"
		auto itr_ = m_sharedInfoList.find(typeID_);
		if (itr_ == m_sharedInfoList.end())
		{
			return std::shared_ptr<BlackBoardDataType>();
		}

		// もし"nullptr"なら"return"
		auto classData_ = itr_->second.classData;
		if (!classData_) 
		{
			return std::shared_ptr<BlackBoardDataType>();
		}

		// 取得する型情報が一致しなければ"return"
		if (dataTypeID_ != classData_->GetDataTypeID())
		{
			return std::shared_ptr<BlackBoardDataType>();
		}

		return std::static_pointer_cast<BlackBoardDataType>(itr_->second.classData);
	}

	std::weak_ptr<GameObject> GetOwnerCache() const { return m_ownerCache; }

	template <typename BlackBoardDataType , typename TypeInfo>
		requires std::derived_from <BlackBoardDataType , IBTBlackBoardData>
	void SetRefSharedInfo(const TypeInfo& Set)
	{
		const uint32_t typeID_     = StaticID<IBTBlackBoardData>::GetTypeID<BlackBoardDataType>  ();
		const uint32_t dataTypeID_ = StaticID<IBTBlackBoardData>::GetUnrestrictedTypeID<TypeInfo>();

		// 無事型と一致するポインタの取得に成功したら値を格納
	 	auto classData_ = GetSharedInfo<BlackBoardDataType , TypeInfo>().lock();
		if (classData_)
		{
			classData_->SetRef(Set);
		}
	}

	template <typename BlackBoardDataType , typename TypeInfo>
		requires std::derived_from <BlackBoardDataType , IBTBlackBoardData>
	void SetSharedInfo(const TypeInfo Set)
	{
		const uint32_t typeID_     = StaticID<IBTBlackBoardData>::GetTypeID<BlackBoardDataType>  ();
		const uint32_t dataTypeID_ = StaticID<IBTBlackBoardData>::GetUnrestrictedTypeID<TypeInfo>();

		// 無事型と一致するポインタの取得に成功したら値を格納
	 	auto classData_ = GetSharedInfo<BlackBoardDataType , TypeInfo>().lock();
		if (classData_)
		{
			classData_->Set(Set);
		}
	}

	void SetOwnerCache(std::weak_ptr<GameObject> Set) { m_ownerCache = Set; }

private:

	void AddSharedInfo(const std::string& ClassName , std::shared_ptr<IBTBlackBoardData> Data);

	std::weak_ptr<GameObject> m_ownerCache;

	std::unordered_map<uint32_t    , CommonStruct::GenericClassData<IBTBlackBoardData>>												 m_sharedInfoList;
	std::unique_ptr<std::unordered_map<std::string , std::weak_ptr<IBTBlackBoardData> , CommonStruct::StringHash , std::equal_to<>>> m_sharedInfoCacheList;
	
};