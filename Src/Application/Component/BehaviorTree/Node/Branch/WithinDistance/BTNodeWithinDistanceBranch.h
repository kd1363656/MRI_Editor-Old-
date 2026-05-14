#pragma once
#include "../BTNodeBranchBase.h"

class BTNodeWithinDistanceBranch final : public BTNodeBranchBase
{

public:

	BTNodeWithinDistanceBranch ()          = default;
	~BTNodeWithinDistanceBranch() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeWithinDistanceBranch>(); }

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	float m_thresholdDistance = 0.0f;

protected:

	bool IsConditionMatched() override;

};