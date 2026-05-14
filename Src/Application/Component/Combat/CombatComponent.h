#pragma once
#include "../../Utility/ImGui/ImGuiUtility.h"

class ComponentObserver;

class CombatComponent : public ComponentBase
{
public:

	struct AtkStatus
	{
		float multiplier = CommonConstant::FLOAT_ONE;
	};
	
	CombatComponent ()          = default;
	~CombatComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<CombatComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void TakeDamage(const float AtkPow);

	void LateUpdate() override;

	void OnCollision(const std::list<KdCollider::CollisionResult>& Result) override;
	
	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	std::weak_ptr<ComponentObserver> m_componentObserverCache;

	std::unordered_map<uint32_t , AtkStatus> m_atkStatusList;

	float m_atkPow = 0.0f;
	float m_defPow = 0.0f;
	float m_nowHp  = 0.0f;
	float m_maxHp  = 0.0f;

	bool m_hasAttackHit = false;

#ifdef _DEBUG
	uint32_t m_debugAddEventTag = CommonConstant::INVALID_STATIC_ID;
#endif
};