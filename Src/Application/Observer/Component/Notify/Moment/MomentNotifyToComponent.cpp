#include "MomentNotifyToComponent.h"

#include "../../ComponentObserver.h"

void MomentNotifyToComponent::DispatchNotify(float ElapsedTime)
{
	auto componentObserverCache_ = GetWorkComponentObserverCache().lock();
	if (!componentObserverCache_) { return; }

	// もうすでに通知しているか通知時間が通知開始時間に達していなければ実行しない
	if (m_isNotifiedOnce) { return; }

	if (ElapsedTime >= GetBeginNotifyTime())
	{	
		// 通知を行ったことを伝える
		m_isNotifiedOnce = true;

		// "Moment"は"1"フレームだけしか情報を保持できない
		// つまり"True"以外伝える必要がない
		componentObserverCache_->NotifyEvent(GetTagID() , CommonEnum::EventLane::Moment , CommonEnum::BoolFlag::True);
	}
}

void MomentNotifyToComponent::ResetNotify()
{
	m_isNotifiedOnce = false;
}

void MomentNotifyToComponent::ImGuiInspector(float NotifyTimeLimit)
{
	// フラグが立っているか以内かを文字列で確認
	ImGui::Text("IsNotifiedOnce : %s", ImGuiUtility::BoolToString(m_isNotifiedOnce));

	NotifyToComponentBase::ImGuiInspector(NotifyTimeLimit);
}