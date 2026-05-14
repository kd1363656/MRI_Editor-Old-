#include "CameraComponent.h"
#include "../Transform/TransformComponent.h"

#include "../../GameObject/GameObject.h"

void CameraComponent::Init()
{
	if (!m_camera)
	{
		m_camera = std::make_shared<KdCamera>();
	}

	m_fieldOfView = CommonConstant::CAMERA_DEFAULT_FIELD_OF_VIEW;
}
void CameraComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }
	
	m_transformComponentCache = owner_->GetComponent<TransformComponent>();
	
	if (m_camera)
	{
		m_camera->SetProjectionMatrix(m_fieldOfView);
	}
}

void CameraComponent::PreDraw()
{
	auto transformCache_ = m_transformComponentCache.lock();

	if (!transformCache_) { return; }
	if (!m_camera)        { return; }

	m_camera->SetCameraMatrix(transformCache_->GetMatrix());
	m_camera->SetToShader();

	// 視錐台のキャッシュ作成、描画された後にすぐに行わないと
	// 後の描画のカリングに使えず意味がないからここで実行
	CreateFrustum();
}

void CameraComponent::ImGuiSpawnDataInspector()
{
	ImGui::DragFloat("FieldOfView", &m_fieldOfView, 0.1f, 1.0f, 89.0f);
}

void CameraComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_fieldOfView = Json.value("FieldOfView", CommonConstant::CAMERA_DEFAULT_FIELD_OF_VIEW);
}

nlohmann::json CameraComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	json_["FieldOfView"] = m_fieldOfView;

	return json_;
}

void CameraComponent::CreateFrustum()
{
	auto transformComponentCache_ = m_transformComponentCache.lock();

	if (!transformComponentCache_) { return; }
	if (!m_camera)				   { return; }

	// 射影行列から視錐台を初期化
	DirectX::BoundingFrustum::CreateFromMatrix(m_boundingFrustum , m_camera->GetProjMatrix());

	// カメラの行列から位置・回転・スケールを一括適用(安全)
	m_boundingFrustum.Transform(m_boundingFrustum , transformComponentCache_->GetMatrix());
}