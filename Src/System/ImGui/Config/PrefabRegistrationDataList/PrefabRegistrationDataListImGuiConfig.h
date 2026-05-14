#pragma once
#include "../ImGuiConfigBase.h"

#include "../Src/Application/Utility/Common/CommonStruct.h"

class PrefabRegistrationDataListImGuiConfig : public ImGuiConfigBase
{

public:

	PrefabRegistrationDataListImGuiConfig ()          = default;
	~PrefabRegistrationDataListImGuiConfig() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiConfigBase>::GetTypeID<PrefabRegistrationDataListImGuiConfig>(); }

	void Init() override;

	void LoadFile() override;

	void SaveFile() override;

	void AddPrefabRegistrationData(const CommonStruct::PrefabRegistrationData& PrefabRegistrationData);

	auto& GetPrefabRegistrationDataList() { return m_prefabRegistrationDataList; }

private:

	const std::string PREFAB_REGISTRATION_DATA_LIST_SAVE_AND_LOAD_FILE_PATH = "Asset/Data/Config/ImGui/PrefabRegistrationDataList/PrefabRegistrationDataListConfig";

	std::unordered_map<std::string , CommonStruct::PrefabRegistrationData , CommonStruct::StringHash , std::equal_to<>> m_prefabRegistrationDataList;

};