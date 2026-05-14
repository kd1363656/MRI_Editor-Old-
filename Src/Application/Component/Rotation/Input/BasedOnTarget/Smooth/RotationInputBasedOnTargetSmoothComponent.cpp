#include "RotationInputBasedOnTargetSmoothComponent.h"
#include "../../../../Transform/TransformComponent.h"

#include "../../../../../CommonCore/Rotation/CommonCoreRotation.h"
#include "../../../../../CommonCore/Rotation/Input/Smooth/CommonCoreRotationInputSmooth.h"

#include "../../../../../Observer/Component/ComponentObserver.h"
#include "../../../../../Tag/ComponentEvent/ComponentEventTag.h"

#include "../../../../../../System/Calculation/Calculation.h"

#include "../../../../../Modifier/Interporator/InterpolatorModifierBase.h"

void RotationInputBasedOnTargetSmoothComponent::Init()
{
	if (!m_commonCoreRotationInputSmooth)
	{
		m_commonCoreRotationInputSmooth = std::make_shared<CommonCoreRotationInputSmooth>();
	}

	m_commonCoreRotationInputSmooth->Init();

	m_boundTransformComponent = std::make_shared<CommonCoreGUIDBinder<TransformComponent>>();
}
void RotationInputBasedOnTargetSmoothComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCoreRotationInputSmooth)
	{
		m_commonCoreRotationInputSmooth->PostLoadInit(owner_);
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

void RotationInputBasedOnTargetSmoothComponent::Update()
{
	if (!m_commonCoreRotationInputSmooth) { return; }
	if (!m_boundTransformComponent)				   { return; }

	// もしコンポーネント用のオブザーバーがあって
	// 移動が許可されていなければ移動しない
	
	if (auto componentObserverCache_ = m_componentObserverCache.lock())
	{
		const bool isMatching_ = componentObserverCache_->IsEventMatching(StaticID<ComponentEventTagBase>::GetTypeID<RotationInputSmoothBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep , CommonEnum::BoolFlag::False);
		if (isMatching_) { return; }
	}

	auto commonCoreRotation_ = m_commonCoreRotationInputSmooth->GetCommonCoreRotationCache().lock();
	if (!commonCoreRotation_) { return; }

	auto interpolatorModifier_ = m_commonCoreRotationInputSmooth->GetInterpolatorModifierCache().classData;
	if (!interpolatorModifier_) { return; }

	Math::Vector3 inputDirection_ = commonCoreRotation_->GetRotationDirection();
	if (inputDirection_.LengthSquared() <= CommonConstant::EPSILON) 
	{
		// 補完進捗度を初期化
		interpolatorModifier_->ResetInterpolate();
		return; 
	}

	auto selfTransformComponentCache_ = commonCoreRotation_->GetSelfTransformComponentCache().lock();
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
	Math::Vector3 boundForward_ = boundMatrix_.Forward();
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

	// 現在の回転
	Math::Quaternion currentQuaternion_ = selfTransformComponentCache_->GetRotation();

	// Slerp補正(クオータニオンの内積が"0.0f"以下なら最短経路じゃなくなってしまうため符号を反転する)
	if (currentQuaternion_.Dot(targetQuaternion_) < 0.0f)
	{
		targetQuaternion_ = -targetQuaternion_;
	}

	// 補完
	float deltaTime_ = Application::Instance().GetScaledDeltaTime();
	Math::Quaternion resultQuat_ = Math::Quaternion::Slerp
	(
		currentQuaternion_	                                  ,
		targetQuaternion_	                                  ,
		interpolatorModifier_->GetCurrentValue() * deltaTime_
	);

	selfTransformComponentCache_->SetRotation(resultQuat_);

	interpolatorModifier_->Update();
}

void RotationInputBasedOnTargetSmoothComponent::ImGuiSpawnDataInspector()
{
	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->ImGuiSpawnDataInspector();
	}
}
void RotationInputBasedOnTargetSmoothComponent::ImGuiPrefabDataInspector()
{
	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->ImGuiPrefabDataInspector();
	}

	if (m_commonCoreRotationInputSmooth)
	{
		m_commonCoreRotationInputSmooth->ImGuiPrefabDataInspector();
	}
}

void RotationInputBasedOnTargetSmoothComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->DeserializeSpawnData(Json);
	}
}
void RotationInputBasedOnTargetSmoothComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->DeserializePrefabData(Json);
	}

	if (m_commonCoreRotationInputSmooth)
	{
		m_commonCoreRotationInputSmooth->DeserializePrefabData(Json);
	}
}

nlohmann::json RotationInputBasedOnTargetSmoothComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	if (m_boundTransformComponent)
	{
		json_.update(m_boundTransformComponent->SerializeSpawnData());
	}

	return json_;
}
nlohmann::json RotationInputBasedOnTargetSmoothComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_boundTransformComponent)
	{
		json_.update(m_boundTransformComponent->SerializePrefabData());
	}

	if (m_commonCoreRotationInputSmooth)
	{
		json_.update(m_commonCoreRotationInputSmooth->SerializePrefabData());
	}

	return json_;
}