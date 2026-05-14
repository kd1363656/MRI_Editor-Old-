#pragma once
#include "../../Application/Utility/Singleton/SingletonBase.h"
#include "Window/ImGuiWindowBase.h"
#include "Config/ImGuiConfigBase.h"

//
class GameObject;

class ImGuiManager : public SingletonBase<ImGuiManager>
{

public:

	enum class PopUpFlg
	{
		Save   = 1 << 0 ,
		ReLoad = 1 << 1 ,
	};

	void Init();
	
	void Update();

	uint32_t& GetPopUpFlag() { return m_popUpFlag; }

	template <class ImGuiWindowType>
		requires std::derived_from<ImGuiWindowType , ImGuiWindowBase>
	void CreateImGuiWindow()
	{
		auto window_ = std::make_shared<ImGuiWindowType>();

		const uint32_t typeID_ = window_->GetTypeID();
		
		m_imGuiWindowList.try_emplace(typeID_ , window_);
	}

	template <class ImGuiConfigType>
		requires std::derived_from<ImGuiConfigType, ImGuiConfigBase>
	void CreateImGuiConfig()
	{
		auto config_ = std::make_shared<ImGuiConfigType>();

		const uint32_t typeID_ = config_->GetTypeID();

		m_imGuiConfigList.try_emplace(typeID_, config_);
	}

	template <class ImGuiWindowType>
		requires std::derived_from<ImGuiWindowType, ImGuiWindowBase>
	std::weak_ptr<ImGuiWindowType> GetImGuiWindow()
	{
		const uint32_t typeID_ = StaticID<ImGuiWindowBase>::GetTypeID<ImGuiWindowType>();

		auto itr_ = m_imGuiWindowList.find(typeID_);

		if (itr_ != m_imGuiWindowList.end())
		{
			if (itr_->second)
			{
				return std::static_pointer_cast<ImGuiWindowType>(itr_->second);
			}
		}

		return std::weak_ptr<ImGuiWindowType>();
	}

	template <class ImGuiConfigType>
		requires std::derived_from<ImGuiConfigType, ImGuiConfigBase>
	std::weak_ptr<ImGuiConfigType> GetImGuiConfig()
	{
		const uint32_t typeID_ = StaticID<ImGuiConfigBase>::GetTypeID<ImGuiConfigType>();

		auto itr_ = m_imGuiConfigList.find(typeID_);

		if (itr_ != m_imGuiConfigList.end())
		{
			if (itr_->second)
			{
				return std::static_pointer_cast<ImGuiConfigType>(itr_->second);
			}
		}

		return std::weak_ptr<ImGuiConfigType>();
	}

	bool IsCursorInScreen();

	const auto& GetImGuiConfigList() const { return m_imGuiConfigList; }

private:

	void UpdatePopUpFlag();
	void UpdatePopUpFlag(const ImGuiManager::PopUpFlg StandFlg, const char* const Label);
	
	uint32_t m_popUpFlag = 0u;

	std::unordered_map<uint32_t , std::shared_ptr<ImGuiWindowBase>> m_imGuiWindowList;
	std::unordered_map<uint32_t , std::shared_ptr<ImGuiConfigBase>> m_imGuiConfigList;

	// ========================
	// Singleton
	// ========================
	friend class SingletonBase<ImGuiManager>;

	ImGuiManager ()          = default;
	~ImGuiManager() override = default;

};