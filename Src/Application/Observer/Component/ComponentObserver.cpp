#include "ComponentObserver.h"

#include "../../Utility/Json/JsonUtility.h"
#include "../../Utility/ImGui/ImGuiUtility.h"

#include "../../Tag/TagRegistry.h"
#include "../../Tag/ComponentEvent/ComponentEventTag.h"

void ComponentObserver::BeginFrame()
{
	// フレーム初めにフラグを初期化
	// 重くなったらダーティーフラグを使う
	for (auto& [key_ , value_] : m_eventList)
	{
		value_.moment = CommonEnum::BoolFlag::False;
	}
}

void ComponentObserver::RegisterEvent(const std::string& TagName)
{
	auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

	// 登録できない"ID"が帰ってきたら実行しない
	uint32_t tagID_ = tagRegistry_.GetTagID(TagName);
	if (tagID_ == CommonConstant::INVALID_STATIC_ID) { return; }

	// イベントに適したキーを格納
	CommonStruct::EventTypeData data_ = {};
	m_eventList.try_emplace(tagID_ , data_);
}

void ComponentObserver::ImGuiPrefabDataInspector()
{
	auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

#ifdef _DEBUG
	ImGuiUtility::TagIDRadioButtonSelector("TagIDList" , m_addTagName);
#endif

#ifdef _DEBUG
	if (ImGui::Button("Add Event"))
	{
		RegisterEvent(m_addTagName);
	}
#endif // _DEBUG

	if (ImGui::TreeNode("EventList"))
	{
		// 二十ループであまりよろしくないが簡易的なデバック表示
		for (const auto& [key_ , value_] : m_eventList)
		{
			ImGui::PushID(&key_);

			for(const auto& [tagKey_ , tagValue_] : tagRegistry_.GetTagIDList())
			{
			 	bool isSame_ = key_ == tagValue_;
			
				if (!isSame_) { continue; }
				
				if(ImGui::TreeNode(tagKey_.c_str()))
				{
					ImGui::Text("KeepEvent   : %s" , ImGuiUtility::BoolToString(value_.keep)  );
					ImGui::Text("MomentEvent : %s" , ImGuiUtility::BoolToString(value_.moment));

					ImGui::TreePop();
				}
			}

			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Delete"))
	{
		auto itr_ = m_eventList.begin();

		while (itr_ != m_eventList.end())
		{
			ImGui::PushID(&itr_->second);
			
			for (const auto& [tagKey_, tagValue_] : tagRegistry_.GetTagIDList())
			{
				if (itr_->first == tagValue_)
				{
					ImGui::Text(tagKey_.c_str());
				}
			}

			ImGui::SameLine();
			if (ImGui::SmallButton("Delete"))
			{
				itr_ = m_eventList.erase(itr_);
			}
			else
			{
				itr_++;
			}
			
			ImGui::PopID();
		}

		ImGui::TreePop();
	}
}

void ComponentObserver::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (JsonUtility::CheckIsContainAndArray(Json , "EventList"))
	{
		for (const auto& jsonArray_ : Json["EventList"])
		{
			const std::string eventTagName_ = jsonArray_.value("EventTagName" , CommonConstant::STRING_UNKNOWN);

			RegisterEvent(eventTagName_);
		}
	}
}

nlohmann::json ComponentObserver::SerializePrefabData()
{
	auto& tagRegistry_ = TagRegistry<ComponentEventTagBase>::GetInstance();

	auto rootJson_  = nlohmann::json	   ();
	auto jsonArray_ = nlohmann::json::array();

	for (const auto& [key_ , value_] : m_eventList)
	{
		auto json_ = nlohmann::json();

		JsonUtility::ComponentEventTagIDToJson(json_ , key_ , "EventTagName");

		jsonArray_.emplace_back(json_);
	}

	rootJson_["EventList"] = jsonArray_;

	return rootJson_;
}

void ComponentObserver::NotifyEvent(uint32_t EventTagID , CommonEnum::EventLane Lane, CommonEnum::BoolFlag Set)
{
	auto itr_ = m_eventList.find(EventTagID);
	if (itr_ == m_eventList.end()) { return; }

	if (Lane == CommonEnum::EventLane::Moment)
	{
		itr_->second.moment = Set;
	}
	else if (Lane == CommonEnum::EventLane::Keep)
	{
		itr_->second.keep = Set;
	}
}

bool ComponentObserver::IsEventMatching(uint32_t EventTagID , CommonEnum::EventLane Lane, CommonEnum::BoolFlag WantResult)
{
	CommonEnum::BoolFlag flag_ = GetNotification(EventTagID , Lane);
	return flag_ == WantResult;
}

CommonStruct::EventTypeData ComponentObserver::GetEventData(uint32_t ReceiveEventTagID) const
{
	auto itr_ = m_eventList.find(ReceiveEventTagID);
	if (itr_ == m_eventList.end()) { return CommonStruct::EventTypeData(); }

	return itr_->second;
}

CommonEnum::BoolFlag ComponentObserver::GetNotification(uint32_t ReceiveEventTagID, CommonEnum::EventLane EventLane) const
{
	auto itr_ = m_eventList.find(ReceiveEventTagID);
	if (itr_ == m_eventList.end()) { return CommonEnum::BoolFlag::None; }

	if (EventLane == CommonEnum::EventLane::Moment)
	{
		return itr_->second.moment;
	}
	else if (EventLane == CommonEnum::EventLane::Keep)
	{
		return itr_->second.keep;
	}

	return CommonEnum::BoolFlag::None;
}