#pragma once

class TransformComponent;

class CameraComponent : public ComponentBase
{

public:

	CameraComponent ()          = default;
	~CameraComponent() override = default;

	uint32_t GetTypeID() const override { return StaticID<ComponentBase>::GetTypeID<CameraComponent>(); }

	void Init        () override;
	void PostLoadInit() override;

	void PreDraw();

	void ImGuiSpawnDataInspector() override;
	
	void		   DeserializeSpawnData(const nlohmann::json& Json) override;
	nlohmann::json SerializeSpawnData  ()							override;
	
private:

	void CreateFrustum();

	std::weak_ptr<TransformComponent> m_transformComponentCache;

	std::shared_ptr<KdCamera> m_camera = nullptr;

	DirectX::BoundingFrustum m_boundingFrustum = {};

	float m_fieldOfView = 0.0f;

};