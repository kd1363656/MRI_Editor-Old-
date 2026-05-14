#include "KeepNotifyToComponent.h"

#include "../../ComponentObserver.h"

void KeepNotifyToComponent::Init()
{
	NotifyToComponentBase::Init();

	m_endNotifyTime = 0.0f;
}

void KeepNotifyToComponent::DispatchNotify(float ElapsedTime)
{
	auto componentObserverCache_ = GetWorkComponentObserverCache().lock();
	if (!componentObserverCache_) { return; }

	const bool isElapsedNotifyStartTime = ElapsedTime >= GetBeginNotifyTime();
	const bool isElapsedNotifyEndTime   = ElapsedTime >= m_endNotifyTime;
	
	// 通知開始時間を経過しているかつ通知終了時間を超えていなれ蹴れば通知開始
	if (isElapsedNotifyStartTime && !isElapsedNotifyEndTime)
	{
		componentObserverCache_->NotifyEvent(GetTagID(), CommonEnum::EventLane::Keep, m_notifyFlag);
	}
	// 通知終了時間を経過していれば通知終了
	else if (isElapsedNotifyEndTime)
	{
		componentObserverCache_->NotifyEvent(GetTagID(), CommonEnum::EventLane::Keep, CommonEnum::InvertBoolFlag(m_notifyFlag));	
	}
}

void KeepNotifyToComponent::ResetNotify()
{
	if (auto componentObserverCache_ = GetWorkComponentObserverCache().lock())
	{
		// 通知する予定のフラグと反対のフラグを渡す (通知をやめるということ)
		componentObserverCache_->NotifyEvent(GetTagID() , CommonEnum::EventLane::Keep , CommonEnum::InvertBoolFlag(m_notifyFlag));
	}
}

void KeepNotifyToComponent::ImGuiInspector(float NotifyTimeLimit)
{
	ImGuiUtility::EnumRadioButtonSelector("NotifyFlag", m_notifyFlag, ImGuiUtility::EVENT_NOTIFY_FLAG_LIST);

	ImGui::DragFloat("EndNotifyTime" , &m_endNotifyTime , 0.1f , 0.0f , NotifyTimeLimit);

	NotifyToComponentBase::ImGuiInspector(NotifyTimeLimit);
}

void KeepNotifyToComponent::DeserializeData(const nlohmann::json Json)
{
	if (Json.is_null()) { return; }

	NotifyToComponentBase::DeserializeData(Json);

	m_notifyFlag = JsonUtility::JsonToEnum<int>(CommonEnum::BoolFlag::None , Json, "NotifyFlag");

	m_endNotifyTime = Json.value("EndNotifyTime" , 0.0f);
}

nlohmann::json KeepNotifyToComponent::SerializeData() const
{
	auto json_ = nlohmann::json();

	json_.update(NotifyToComponentBase::SerializeData());

	json_["NotifyFlag"] = static_cast<int>(m_notifyFlag);

	json_["EndNotifyTime"] = m_endNotifyTime;

	return json_;
}