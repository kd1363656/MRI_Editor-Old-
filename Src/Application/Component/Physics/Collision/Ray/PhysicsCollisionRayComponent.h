#pragma once

class TransformComponent;
class PhysicsCollisionDebugWireComponent;
class PhysicsGravityComponent;

class CommonCorePhysicsCollision;

class PhysicsCollisionRayComponent : public ComponentBase
{

public:

	struct RayCollisionData
	{
		Math::Vector3 offset    = Math::Vector3::Zero;
		Math::Vector3 direction = Math::Vector3::Down;
		float         range     = 0.0f;
	};

	PhysicsCollisionRayComponent ()          = default;
	~PhysicsCollisionRayComponent() override = default;

	bool IsAllowMultiple() const override { return true; }

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<PhysicsCollisionRayComponent>(); }

	void Init		 ()	override;
	void PostLoadInit() override;

	void Update	   () override;
	void LateUpdate() override;

	void ImGuiPrefabDataInspector() override;

	void           DeserializePrefabData(const nlohmann::json& Json) override;
	nlohmann::json SerializePrefabData  ()                           override;

	const RayCollisionData& GetRayCollisionData() const { return m_rayCollisionData; }

private:

	std::weak_ptr<TransformComponent>	   m_parentTransformComponent;
	std::weak_ptr<PhysicsGravityComponent> m_physicsGravityComponent;

	std::shared_ptr<CommonCorePhysicsCollision> m_commonCorePhysicsCollision = nullptr;

	RayCollisionData m_rayCollisionData;
};