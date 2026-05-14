#pragma once
#include "../NotifyToComponentBase.h"

class MomentNotifyToComponent : public NotifyToComponentBase
{

public:

	MomentNotifyToComponent ()          = default;
	~MomentNotifyToComponent() override = default;

	void DispatchNotify(float ElapsedTime) override;

	void ResetNotify() override;

	void ImGuiInspector(float NotifyTimeLimit) override;

public:

	bool m_isNotifiedOnce = false;

};