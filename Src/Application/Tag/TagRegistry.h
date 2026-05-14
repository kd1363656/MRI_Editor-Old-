#pragma once
#include "../Utility/Singleton/SingletonBase.h"
#include "../Utility/String/StringUtility.h"

template <typename BaseType>
class TagRegistry : public SingletonBase<TagRegistry<BaseType>>
{

public:

	template <typename TagType>
		requires std::derived_from<TagType , BaseType>
	void Registration()
	{
		//使っている関数がテンプレートなら関数を使う前に指定する必要がある
		const std::string key_ = StringUtility::template StripStructPrefix<TagType>();
		const uint32_t    id_  = StaticID<BaseType>::template GetTypeID<TagType>();

		m_tagIDList.try_emplace  (key_ , id_);
		m_tagNameList.try_emplace(id_  , key_);

		KdDebugGUI::Instance().AddLog("Register Tag completed : %s\n", key_.c_str());
	}

	uint32_t GetTagID(const std::string& TagName)
	{
		auto itr_ = m_tagIDList.find(TagName);
		if (itr_ == m_tagIDList.end()) { return CommonConstant::INVALID_STATIC_ID; }

		return itr_->second;
	}

	std::string GetTagName(const uint32_t ID)
	{
		auto itr_ = m_tagNameList.find(ID);
		if (itr_ == m_tagNameList.end()) { return CommonConstant::STRING_UNKNOWN; }

		return itr_->second;
	}

	const auto& GetTagIDList() const { return m_tagIDList; }

private:

	std::unordered_map <std::string , uint32_t , CommonStruct::StringHash , std::equal_to<>> m_tagIDList;
	std::unordered_map <uint32_t    , std::string>											 m_tagNameList;

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<TagRegistry<BaseType>>;

};