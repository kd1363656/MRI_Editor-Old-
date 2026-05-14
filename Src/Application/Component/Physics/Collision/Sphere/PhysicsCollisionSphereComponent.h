#pragma once

class TransformComponent;
class PhysicsCollisionDebugWireComponent;
class CommonCorePhysicsCollision;

class PhysicsCollisionSphereComponent : public ComponentBase
{

public:

	struct SphereCollisionData
	{
		Math::Vector3 offset = Math::Vector3::Zero;
		float         radius = 0.0f;
	};

	PhysicsCollisionSphereComponent ()          = default;
	~PhysicsCollisionSphereComponent() override = default;

	bool IsAllowMultiple() const override { return true; }

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<PhysicsCollisionSphereComponent>(); }

	void Init		 () override;
	void PostLoadInit() override;

	void Update    () override;
	void LateUpdate() override;

	void ImGuiPrefabDataInspector() override;

	void           DeserializePrefabData(const nlohmann::json& Json) override;
	nlohmann::json SerializePrefabData  ()                           override;

	const SphereCollisionData& GetSphereCollisionData() const { return m_sphereCollisionData; }

private:

	std::shared_ptr<CommonCorePhysicsCollision> m_commonCorePhysicsCollision = nullptr;

	SphereCollisionData m_sphereCollisionData;

};