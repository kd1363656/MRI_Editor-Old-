#pragma once

class FileSaveAndLoadBase
{

public:

	FileSaveAndLoadBase         () = default;
	virtual ~FileSaveAndLoadBase() = default;

	virtual void Init() { m_saveAndLoadFilePath.clear(); }

	virtual void SaveFile() { /* 必要に応じてオーバーライドしてください */ }
	virtual void LoadFile() { /* 必要に応じてオーバーライドしてください */ }

	void SetSaveAndLoadFilePath(const std::string& Set) { m_saveAndLoadFilePath = Set; }

protected:

	std::string m_saveAndLoadFilePath = "";

};