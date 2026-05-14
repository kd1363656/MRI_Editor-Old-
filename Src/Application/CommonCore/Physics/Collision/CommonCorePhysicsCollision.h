#pragma once

class ComponentObserver;

class PhysicsColliderComponent;
class PhysicsDebugWireComponent;

class CommonCorePhysicsCollision : public CommonCoreBase
{
public:

	CommonCorePhysicsCollision ()          = default;
	~CommonCorePhysicsCollision() override = default;

	void Init		 ()									 override;
	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	void Update();

	void ImGuiPrefabDataInspector() override;

	void           DeserializePrefabData(const nlohmann::json& Json) override;
	nlohmann::json SerializePrefabData  ()                           override;

	void SetIsCollisionEnabled(bool Set) { m_isCollisionEnabled = Set; }

	std::weak_ptr<TransformComponent>        GetSelfTransformComponentCache   () const { return m_selfTransformComponentCache;    }
	std::weak_ptr<PhysicsDebugWireComponent> GetPhysicsDebugWireComponentCache() const { return m_physicsDebugWireComponentCache; }

	UINT GetCollisionTypeFlags() const { return m_collisionTypeFlags; }

	const std::list<std::weak_ptr<PhysicsColliderComponent>>& GetWorkCollisionTargetList() const { return m_collisionTargetList; }

	uint32_t GetReceiveEventTagID() const { return m_receiveEventTagID; }

	const Math::Vector3& GetFacingDirectionOffsetMagnification() const { return m_facingDirectionOffsetMagnification; }

	uint32_t GetUseFacingDirectionAxisFlags() const { return m_useFacingDirectionAxisFlags; }

	bool GetIsCollisionEnabled() const { return m_isCollisionEnabled; }

private:

	std::weak_ptr<ComponentObserver>		 m_componentObserverCache;
	std::weak_ptr<TransformComponent>		 m_selfTransformComponentCache;
	std::weak_ptr<PhysicsDebugWireComponent> m_physicsDebugWireComponentCache;

	UINT m_collisionTypeFlags = KdCollider::Type::None;

	std::list<std::weak_ptr<PhysicsColliderComponent>> m_collisionTargetList;

	uint32_t m_receiveEventTagID = CommonConstant::INVALID_STATIC_ID;

	Math::Vector3 m_facingDirectionOffsetMagnification = Math::Vector3::Zero;

	uint32_t m_useFacingDirectionAxisFlags = static_cast<uint32_t>(CommonEnum::UseFacingDirectionAxisFlags::None);

	bool m_initCollisionEnabled = false;
	bool m_isCollisionEnabled   = false;
	
};