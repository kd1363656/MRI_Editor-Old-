#include "PrefabRegistrationDataListImGuiConfig.h"

#include "../../../FileSystem/FileSystem.h"

void PrefabRegistrationDataListImGuiConfig::Init()
{
	LoadFile();
}

void PrefabRegistrationDataListImGuiConfig::LoadFile()
{
	auto json_ = FileSystem::LoadJsonFile(PREFAB_REGISTRATION_DATA_LIST_SAVE_AND_LOAD_FILE_PATH);

	if (json_.is_null()) { return; }

	if (json_.contains("PrefabRegistrationDataList"))
	{
		for (const auto& array_ : json_["PrefabRegistrationDataList"])
		{
			CommonStruct::PrefabRegistrationData prefabRegistrationData_ = {};

			prefabRegistrationData_.prefabName     = array_.value("PrefabName"     , CommonConstant::STRING_UNKNOWN   );
			prefabRegistrationData_.prefabFilePath = array_.value("PrefabFilePath" , CommonConstant::ASSET_FOLDER_PATH);
			prefabRegistrationData_.categoryType   = array_.value("CategoryType"   , 0u                               );
			
			m_prefabRegistrationDataList.try_emplace(prefabRegistrationData_.prefabName , prefabRegistrationData_);
		}
	}
}

void PrefabRegistrationDataListImGuiConfig::SaveFile()
{
	auto rootJson_  = nlohmann::json       ();
	auto arrayJson_ = nlohmann::json::array();

	for (const auto& [key_ , value_] : m_prefabRegistrationDataList)
	{
		auto gameObjectTypeInfoJson_ = nlohmann::json();

		gameObjectTypeInfoJson_["PrefabName"    ] = value_.prefabName;
		gameObjectTypeInfoJson_["PrefabFilePath"] = value_.prefabFilePath;
		gameObjectTypeInfoJson_["CategoryType"  ] = value_.categoryType;

		arrayJson_.emplace_back(gameObjectTypeInfoJson_);
	}

	rootJson_["PrefabRegistrationDataList"] = arrayJson_;

	FileSystem::SaveJsonFile(rootJson_ , PREFAB_REGISTRATION_DATA_LIST_SAVE_AND_LOAD_FILE_PATH);
}

void PrefabRegistrationDataListImGuiConfig::AddPrefabRegistrationData(const CommonStruct::PrefabRegistrationData& PrefabRegistrationData)
{
	if (PrefabRegistrationData.prefabName.empty()) { return; }

	m_prefabRegistrationDataList.try_emplace(PrefabRegistrationData.prefabName, PrefabRegistrationData);
}