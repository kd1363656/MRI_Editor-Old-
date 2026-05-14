#include "GUIDManagerBase.h"

#include "../../System/FileSystem/FileSystem.h"

void GUIDManagerBase::LoadRegistry()
{
	auto json_ = FileSystem::LoadJsonFile(m_saveAndLoadFilePath);
	if (json_.is_null()) { return; }

	// 念のため衝突検知用"std::unordered_set<uint64_t>"は空にしておく
	m_usedGUIDList.clear();

	m_nextGUID = json_.value("NextGUID" , FIRST_GUID);
}

void GUIDManagerBase::SaveRegistry()
{
	auto json_ = nlohmann::json();

	json_["NextGUID"] = m_nextGUID;
	
	FileSystem::SaveJsonFile(json_ , m_saveAndLoadFilePath);
}

uint64_t GUIDManagerBase::GenerateGUID()
{
	// 既に使われている"ID"をスキップ
	while (m_usedGUIDList.contains(m_nextGUID))
	{
		++m_nextGUID;
	}

	// "0"は未登録
	if (m_nextGUID == CommonConstant::UNREGISTERD_GUID)
	{
		++m_nextGUID;
	}

	// 最新の"id"を使われている"Id"のリストに登録し次の生成する"ID"を進める
	const uint64_t id_ = m_nextGUID;
	m_usedGUIDList.insert(id_);
	++m_nextGUID;

	return id_;
}

void GUIDManagerBase::RegisterGUID(const uint64_t GUID)
{
	if (GUID == CommonConstant::UNREGISTERD_GUID)
	{
		KdDebugGUI::Instance().AddLog("In GUIDManagerBase : GUID is invalid 0\n");
		return;
	}

	auto itr_ = m_usedGUIDList.find(GUID);
	if (itr_ != m_usedGUIDList.end())
	{
		KdDebugGUI::Instance().AddLog("In GUIDManagerBase detected conflict\nID : %llu\n" , GUID);
		return;
	}

	// 未使用なので登録
	m_usedGUIDList.insert(GUID);

	// "next"を進める
	if (GUID >= m_nextGUID)
	{
		m_nextGUID = GUID + CommonConstant::INCREMENT_GUID;

		if (m_nextGUID == CommonConstant::UNREGISTERD_GUID)
		{
			m_nextGUID = FIRST_GUID;
		}
	}
}

void GUIDManagerBase::UnRegisterGUID(const uint64_t GUID)
{
	KdDebugGUI::Instance().AddLog("Unregister GUID : %llu\n" , GUID);
	m_usedGUIDList.erase(GUID);
}

bool GUIDManagerBase::IsUseGUID(const uint64_t GUID) const
{
	return m_usedGUIDList.contains(GUID);
}