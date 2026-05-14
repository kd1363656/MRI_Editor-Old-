#pragma once
#include "../Utility/Singleton/SingletonBase.h"
#include "../Utility/String/StringUtility.h"
#include "../Utility/Common/CommonStruct.h"

class GameObject;

template <typename DerivedFactory , typename BaseClass>
class GenericFactoryBase : public SingletonBase<DerivedFactory>
{

public:

	virtual void Init() { /* 必要に応じてオーバーライドしてください */ }

	template <typename DerivedClass>
		requires std::derived_from<DerivedClass , BaseClass>
	void RegisterFactoryMethod()
	{
		// クラス名を取得
		std::string derivedName_ = StringUtility::StripClassPrefix<DerivedClass>();

		const auto factory_ = []() -> std::shared_ptr<DerivedClass>
		{
			return std::make_shared<DerivedClass>();
		};

		m_factoryMethodList.try_emplace(derivedName_, factory_);

#ifdef _DEBUG
		KdDebugGUI::Instance().AddLog("Register completed : %s\n", derivedName_.c_str());
#endif
	}

	std::shared_ptr<BaseClass>Create(const std::string& CreateClassName) const
	{
		auto itr_ = m_factoryMethodList.find(CreateClassName);

		if (itr_ == m_factoryMethodList.end())
		{
			KdDebugGUI::Instance().AddLog("Factory method not found: %s\n", CreateClassName.c_str());
			return std::shared_ptr<BaseClass>();
		}

		return itr_->second();
	}

	const auto& GetFactoryMethodList() const { return m_factoryMethodList; }

private:

	std::unordered_map <std::string, std::function<std::shared_ptr<BaseClass>()>, CommonStruct::StringHash, std::equal_to<>> m_factoryMethodList;

protected:

	// "CRTP"で行っているためインスタンス化は派生クラスからのみ行われる
	// 派生クスでデストラクタを定義しても問題ないように,"virtual"にしておく
	GenericFactoryBase         () = default;
	virtual ~GenericFactoryBase() = default;

};