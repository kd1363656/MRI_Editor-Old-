#pragma once

class PhysicsColliderComponent;
class PhysicsDebugWireComponent;

class SpringArmComponent : public ComponentBase
{

public:

	SpringArmComponent ()          = default;
	~SpringArmComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<SpringArmComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void OnPostCollisionResolved() override;
	
	void ImGuiPrefabDataInspector() override;

	void DeserializePrefabData(const nlohmann::json& Json) override;

	nlohmann::json SerializePrefabData() override;

private:

	std::weak_ptr<TransformComponent> m_selfTransformComponent;
	std::weak_ptr<TransformComponent> m_parentTransformComponent;

	std::weak_ptr<PhysicsDebugWireComponent> m_physicsDebugWireComponent;
	
	std::list<std::weak_ptr<PhysicsColliderComponent>> m_hitObjectList;

	Math::Vector3 m_standardOffsetPos = Math::Vector3::Zero;
	
	float m_springSpeed		  = 0.0f;
	float m_bumpMagnification = 0.0f;

	UINT m_collisionType = static_cast<UINT>(KdCollider::Type::None);
};