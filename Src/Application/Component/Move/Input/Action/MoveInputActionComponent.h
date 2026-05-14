#pragma once
#include "../../../../Utility/Common/CommonStruct.h"

class CommonCoreMoveInput;

class ComponentObserver;

class InterpolatorModifierBase;

class MoveInputActionComponent : public ComponentBase
{

public:

	MoveInputActionComponent ()          = default;
	~MoveInputActionComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<MoveInputActionComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void Update() override;

	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	void RegistryMoveInputActionData(const CommonStruct::MoveInputActionData& MoveInputActionData);

	std::weak_ptr<ComponentObserver>        m_componentObserverCache;
	
	std::vector<CommonStruct::MoveInputActionData> m_moveInputActionDataList;

	std::shared_ptr<CommonCoreMoveInput> m_commonCoreMoveInput = nullptr;

	std::weak_ptr<InterpolatorModifierBase> m_nowInterpolatorModifierCache;

	uint32_t m_debugAddEventTag = CommonConstant::INVALID_STATIC_ID;

};