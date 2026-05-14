#include "NotifyToComponentBase.h"

#include "../../../Utility/ImGui/ImGuiUtility.h"

void NotifyToComponentBase::Init()
{
	m_tagID = CommonConstant::INVALID_STATIC_ID;

	m_beginNotifyTime = 0.0f;
}

void NotifyToComponentBase::ImGuiInspector(float NotifyTimeLimit)
{
	ImGuiUtility::TagIDRadioButtonSelector("TagID" , m_tagID);

	ImGui::DragFloat("BeginNotifyTime" , &m_beginNotifyTime , 0.1f , 0.0f , NotifyTimeLimit);
}

void NotifyToComponentBase::DeserializeData(const nlohmann::json Json)
{
	if (Json.is_null()) { return; }

	m_tagID = JsonUtility::JsonToComponentEventTagID(Json , "TagName");

	m_beginNotifyTime = Json.value("BeginNotifyTime" , 0.0f);
}

nlohmann::json NotifyToComponentBase::SerializeData() const
{
	auto json_ = nlohmann::json();

	JsonUtility::ComponentEventTagIDToJson(json_ , m_tagID , "TagName");

	json_["BeginNotifyTime"] = m_beginNotifyTime;

	return json_;
}