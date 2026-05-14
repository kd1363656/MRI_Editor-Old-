#include "RotationInputBasedOnTargetInstantComponent.h"
#include "../../../../Transform/TransformComponent.h"

#include "../../../../../CommonCore/Rotation/CommonCoreRotation.h"

#include "../../../../../Observer/Component/ComponentObserver.h"
#include "../../../../../Tag/ComponentEvent/ComponentEventTag.h"
#include "../../../../../Tag/TagRegistry.h"

#include "../../../../../../System/Calculation/Calculation.h"

void RotationInputBasedOnTargetInstantComponent::Init()
{
	if (!m_commonCoreRotation)
	{
		m_commonCoreRotation = std::make_shared<CommonCoreRotation>();
	}

	m_commonCoreRotation->Init();

	m_boundTransformComponent = std::make_shared<CommonCoreGUIDBinder<TransformComponent>>();
}
void RotationInputBasedOnTargetInstantComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCoreRotation)
	{
		m_commonCoreRotation->PostLoadInit(owner_);
	}

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->PostLoadInit(owner_);
	}

	if (auto componentObserver_ = owner_->GetComponentObserver().lock())
	{
		m_componentObserverCache = componentObserver_;
	}
}

void RotationInputBasedOnTargetInstantComponent::Update()
{
	if (!m_commonCoreRotation) { return; }
	if (!m_boundTransformComponent)     { return; }

	// もしコンポーネント用のオブザーバーがあって
	// 一瞬だけの通知が送られてこなければ実行しない
	if (auto componentObserverCache_ = m_componentObserverCache.lock())
	{
		const bool isMatching_ = componentObserverCache_->IsEventMatching(StaticID<ComponentEventTagBase>::GetTypeID<RotationInputInstantBasedOnTargetActiveTag>(), CommonEnum::EventLane::Moment, CommonEnum::BoolFlag::False);
		if (isMatching_) { return; }
	}

	Math::Vector3 inputDirection_ = m_commonCoreRotation->GetRotationDirection();
	if (inputDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }

	auto selfTransformComponentCache_ = m_commonCoreRotation->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	auto boundTransformComponentCache_ = m_boundTransformComponent->GetBindComponentCache().lock();
	if (!boundTransformComponentCache_) { return; }

	// 入力ベクトルをXZ平面へ制限して正規化
	inputDirection_.y = 0.0f;
	if (inputDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }
	inputDirection_.Normalize();

	// 回転の基準となるゲームオブジェクトの回転行列を取得
	Math::Matrix boundMatrix_ = boundTransformComponentCache_->CreateRotationMatrix();

	// 回転の基準となるゲームオブジェクトのForward方向（-Z）を取得し、XZ平面に投影
	// モデルの初期の向いている方向に依存するので細かい調整がしたければストラテジーパターンで作る
	Math::Vector3 boundForward_ = -boundMatrix_.Backward(); // -Z = Forward
	boundForward_.y = 0.0f;
	if (boundForward_.LengthSquared() <= CommonConstant::EPSILON) { return; }
	boundForward_.Normalize();

	// バインド対象のYaw角を計算
	const float boundYaw_ = Calculation::VectorToYaw(boundForward_);

	// 入力方向をバインド基準で回転
	Math::Matrix  yawMatrix_      = Math::Matrix::CreateRotationY (boundYaw_);
	Math::Vector3 worldDirection_ = Math::Vector3::TransformNormal(inputDirection_, yawMatrix_);

	// XZ平面に制限し再正規化
	worldDirection_.y = 0.0f;
	if (worldDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }
	worldDirection_.Normalize();

	// ワールド方向からYaw角を取得
	const float targetYaw_ = Calculation::VectorToYaw(worldDirection_);

	// Yawからクオータニオンを生成
	Math::Quaternion targetQuaternion_ = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::Up, targetYaw_);

	selfTransformComponentCache_->SetRotation(targetQuaternion_);
}

void RotationInputBasedOnTargetInstantComponent::ImGuiSpawnDataInspector()
{
	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->ImGuiSpawnDataInspector();
	}
}
void RotationInputBasedOnTargetInstantComponent::ImGuiPrefabDataInspector()
{
	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->ImGuiPrefabDataInspector();
	}
}

void RotationInputBasedOnTargetInstantComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->DeserializeSpawnData(Json);
	}
}
void RotationInputBasedOnTargetInstantComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->DeserializePrefabData(Json);
	}
}

nlohmann::json RotationInputBasedOnTargetInstantComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	if (m_boundTransformComponent)
	{
		json_.update(m_boundTransformComponent->SerializeSpawnData());
	}

	return json_;
}
nlohmann::json RotationInputBasedOnTargetInstantComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_boundTransformComponent)
	{
		json_.update(m_boundTransformComponent->SerializePrefabData());
	}

	return json_;
}