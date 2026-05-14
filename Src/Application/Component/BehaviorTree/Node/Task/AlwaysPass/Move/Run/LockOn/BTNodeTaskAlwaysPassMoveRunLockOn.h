#pragma once
#include "../../../BTNodeTaskAlwaysPassBase.h"

class InterpolatorModifierBase;
class CommonCoreMoveInput;

class BTNodeTaskAlwaysPassMoveRunLockOn final : public BTNodeTaskAlwaysPassBase
{

public:

	BTNodeTaskAlwaysPassMoveRunLockOn ()          = default;
	~BTNodeTaskAlwaysPassMoveRunLockOn() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeTaskAlwaysPassMoveRunLockOn>(); }

	void Init        () override;
	void PostLoadInit() override;
	void Tick        () override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	std::shared_ptr<CommonCoreMoveInput> m_commonCoreMoveInput = nullptr;

	CommonStruct::GenericClassData<InterpolatorModifierBase> m_interpolatorModifier;

};