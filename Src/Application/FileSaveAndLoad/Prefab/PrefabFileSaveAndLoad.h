#pragma once
#include "../FileSaveAndLoadBase.h"

class PrefabFileSaveAndLoad : public FileSaveAndLoadBase
{

public:

	PrefabFileSaveAndLoad ()          = default;
	~PrefabFileSaveAndLoad() override = default;

	void SaveFile() override;
	void LoadFile() override;

	void AddPrefabFilePath(const std::string& PrefabName , const CommonStruct::PrefabSaveAndLoadFilePath& PrefabSaveAndLoadFilePath);

	auto& GetPrefabFilePathList() { return m_prefabFilePathList; }
	
private:

	std::unordered_map<std::string, CommonStruct::PrefabSaveAndLoadFilePath , CommonStruct::StringHash , std::equal_to<>> m_prefabFilePathList;
};