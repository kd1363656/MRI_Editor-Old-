#pragma once
#include "../NotifyToComponentBase.h"

class KeepNotifyToComponent : public NotifyToComponentBase
{

public:

	KeepNotifyToComponent ()          = default;
	~KeepNotifyToComponent() override = default;

	void Init() override;

	void DispatchNotify(float ElapsedTime) override;

	void ResetNotify() override;

	void ImGuiInspector(float NotifyTimeLimit) override;

	void DeserializeData(const nlohmann::json Json) override;

	nlohmann::json SerializeData() const override;

private:

	CommonEnum::BoolFlag m_notifyFlag = CommonEnum::BoolFlag::None;

	float m_endNotifyTime = 0.0f;
};