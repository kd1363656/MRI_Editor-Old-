#include "BTBlackBoard.h"

#include "Data/IBTBlackBoardData.h"

#include "../../../GameObject/GameObject.h"

#include "../../../Utility/Json/JsonUtility.h"
#include "../../../Utility/String/StringUtility.h"

void BTBlackBoard::PostLoadInit()
{
	for (const auto& [key_ , value_] : m_sharedInfoList)
	{
		if (!value_.classData) { continue; }

		value_.classData->PostLoadInit();
	}
}

void BTBlackBoard::ImGuiSpawnDataInspector()
{
	if(ImGui::CollapsingHeader("BTBlackBoardSpawnData"))
	{
		for (const auto& [key_, value_] : m_sharedInfoList)
		{
			if (!value_.classData) { continue; }

			ImGui::PushID(&value_);

			if (ImGui::TreeNode(value_.name.c_str()))
			{
				value_.classData->ImGuiSpawnDataInspector();
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}
}
void BTBlackBoard::ImGuiPrefabDataInspector()
{
	if (ImGui::CollapsingHeader("BTBlackBoardPrefabData"))
	{
		std::string						   className_ = CommonConstant::STRING_UNKNOWN;
		std::shared_ptr<IBTBlackBoardData> instance_  = nullptr;

		ImGuiUtility::BTBlackBoardDataSelector("Select BlackBoardData" , className_ , instance_);

		// もしセレクターで選択されてインスタンスがしっかりと生成されていればリストに格納
		if (instance_)
		{
			AddSharedInfo(className_ , instance_);
		}
		
		for (const auto& [key_, value_] : m_sharedInfoList)
		{
			if (!value_.classData) { continue; }

			ImGui::PushID(&value_);

			if (ImGui::TreeNode(value_.name.c_str()))
			{
				value_.classData->ImGuiPrefabDataInspector();
				ImGui::TreePop();
			}
			
			ImGui::PopID();
		}
	}
}

void BTBlackBoard::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (!m_sharedInfoCacheList) { return; }

	if (JsonUtility::CheckIsContainAndArray(Json , "SharedInfoList"))
	{
		for (const auto& jsonArray_ : Json["SharedInfoList"])
		{
			const std::string typeName_ = jsonArray_.value("TypeName", CommonConstant::STRING_UNKNOWN);

			// もし見つからなかったら次の"Json"に当てはまる要素を探す
			auto itr_ = m_sharedInfoCacheList->find(typeName_);
			if (itr_ == m_sharedInfoCacheList->end()) { continue; }

			auto data_ = itr_->second.lock();
			if (!data_) { continue; }

			if (!jsonArray_.contains("Data")) { continue; }
			data_->DeserializeSpawnData(jsonArray_["Data"]);
		}
	}

	// もう使う必要がないのでメモリを解放
	m_sharedInfoCacheList.reset();
	m_sharedInfoCacheList = nullptr;
}
void BTBlackBoard::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	// スポーンデータで名前が合致した"Json"データをデシリアライズするため
	if (!m_sharedInfoCacheList)
	{
		m_sharedInfoCacheList = std::make_unique<std::unordered_map<std::string, std::weak_ptr<IBTBlackBoardData> , CommonStruct::StringHash , std::equal_to<>>>();
	}

	if (JsonUtility::CheckIsContainAndArray(Json , "SharedInfoList"))
	{
		for (const auto& jsonArray_ : Json["SharedInfoList"])
		{
			const std::string typeName_ = jsonArray_.value("TypeName" , CommonConstant::STRING_UNKNOWN);

			// しっかりインスタンスが生成されたらデシリアライズして格納
			auto created_ = BTBlackBoardDataFactory::GetInstance().Create(typeName_);
			if (created_)
			{
				// 初期化
				created_->Init();

				if (jsonArray_.contains("Data"))
				{
					created_->DeserializePrefabData(jsonArray_["Data"]);
				}

				AddSharedInfo(typeName_ , created_);
				m_sharedInfoCacheList->try_emplace(typeName_ , created_);
			}
		}
	}
}

nlohmann::json BTBlackBoard::SerializeSpawnData()
{
	auto rootJson_  = nlohmann::json();
	auto jsonArray_ = nlohmann::json::array();

	for (const auto& [key_ , value_] : m_sharedInfoList)
	{
		if (!value_.classData) { continue; }

		auto json_ = nlohmann::json();

		json_["TypeName"] = value_.name;

		auto dataJson_ = value_.classData->SerializeSpawnData();
		if (!dataJson_.is_null())
		{
			json_["Data"] = dataJson_;
		}

		jsonArray_.emplace_back(json_);
	}

	rootJson_["SharedInfoList"] = jsonArray_;

	return rootJson_;
}
nlohmann::json BTBlackBoard::SerializePrefabData()
{
	auto rootJson_  = nlohmann::json();
	auto jsonArray_ = nlohmann::json::array();

	for (const auto& [key_ , value_] : m_sharedInfoList)
	{
		if (!value_.classData) { continue; }

		auto json_ = nlohmann::json();

		json_["TypeName"] = value_.name;

		auto dataJson_ = value_.classData->SerializePrefabData();
		if (!dataJson_.is_null())
		{
			json_["Data"] = dataJson_;
		}

		jsonArray_.emplace_back(json_);
	}

	rootJson_["SharedInfoList"] = jsonArray_;

	return rootJson_;
}

void BTBlackBoard::AddSharedInfo(const std::string& ClassName, std::shared_ptr<IBTBlackBoardData> Data)
{
	CommonStruct::GenericClassData<IBTBlackBoardData> classData_ = {};

	const uint32_t id_ = Data->GetTypeID();

	classData_.classData = Data;
	classData_.name      = ClassName;

	KdDebugGUI::Instance().AddLog("In BTBlackBoard : %s is created completely\n" , ClassName.c_str());

	m_sharedInfoList.try_emplace(id_ , classData_);
}