#pragma once
#include "../../GenericFactoryBase.h"

#include "../../../Component/BehaviorTree/BlackBoard/BTBlackBoard.h"
#include "../../../Component/BehaviorTree/Node/BTNodeBase.h"

class BTNodeFactory : public GenericFactoryBase<BTNodeFactory, BTNodeBase>
{

public:

	void Init() override;
	
	template <typename BaseClass , typename DerivedClass>
		requires std::derived_from<DerivedClass, BaseClass> && std::derived_from<DerivedClass , BTNodeBase>
	void RegisterTypeToName()
	{
		// 基底クラスの"ID"を取得
		const uint32_t parentID_    = StaticID<BTNodeBase>::GetTypeID<BaseClass>();
		std::string    derivedName_ = StringUtility::StripClassPrefix<DerivedClass>();

		// もし登録されていなければ登録
		m_typeToNameList[parentID_].emplace_back(derivedName_);
		
#ifdef _DEBUG
		KdDebugGUI::Instance().AddLog("Register completed : %s\n", derivedName_.c_str());
#endif
	}

	void GetTypeToNameVector(uint32_t ID , std::vector<std::string>& Vector) const;
	
private:

	void InitTypeToName();

	std::unordered_map<uint32_t , std::vector<std::string>> m_typeToNameList;

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<BTNodeFactory>;

	BTNodeFactory ()          = default;
	~BTNodeFactory() override = default;

};