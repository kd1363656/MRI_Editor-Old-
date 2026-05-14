#pragma once

// ※注意 管理が特に重要なクラス
// ここに"GUID"に必要な責任をすべて置く
// "SRP"に違反するが密結合にしておいた方が持たせる機能はそこまでないのでデバック時にいいと感じたから
class GUIDManagerBase
{

public:
	
	GUIDManagerBase         () = default;
	virtual ~GUIDManagerBase() = default;

	void SetSaveAndLoadFilePath(const std::string& Set) { m_saveAndLoadFilePath = Set; }

	void LoadRegistry();
	void SaveRegistry();

	uint64_t GenerateGUID();

	void RegisterGUID  (const uint64_t GUID);
	void UnRegisterGUID(const uint64_t GUID);

	const std::unordered_set<uint64_t>& GetUsedGUIDList() const { return m_usedGUIDList; }

	bool IsUseGUID(const uint64_t GUID) const;

private:

	static constexpr uint64_t FIRST_GUID = 1ull;

	uint64_t					 m_nextGUID = CommonConstant::UNREGISTERD_GUID;
	std::unordered_set<uint64_t> m_usedGUIDList;

	std::string m_saveAndLoadFilePath = "";
};