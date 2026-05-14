#include "MoveInputBasedOnTargetComponent.h"
#include "../../../Transform/TransformComponent.h"

#include "../../../../CommonCore/Move/CommonCoreMove.h"
#include "../../../../CommonCore/Move/Input/CommonCoreMoveInput.h"
#include "../../../../Observer/Component/ComponentObserver.h"

#include "../../../../Tag/ComponentEvent/ComponentEventTag.h"

#include "../../../../Modifier/Interporator/InterpolatorModifierBase.h"

#include "../../../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

#include "../../../../main.h"

void MoveInputBasedOnTargetComponent::Init()
{
	if (!m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput = std::make_shared<CommonCoreMoveInput>();
	}

	if (!m_boundTransformComponent)
	{
		m_boundTransformComponent = std::make_shared<CommonCoreGUIDBinder<TransformComponent>>();
	}

	m_commonCoreMoveInput->Init    ();
	m_boundTransformComponent->Init();
}
void MoveInputBasedOnTargetComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput->PostLoadInit(owner_);
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

void MoveInputBasedOnTargetComponent::Update()
{
	if (!m_commonCoreMoveInput)		       { return; }
	if (!m_interpolatorModifier.classData) { return; }

	auto commonCoreMoveCache_ = m_commonCoreMoveInput->GetCommonCoreMoveCache().lock();
	if (!commonCoreMoveCache_) { return; }

	auto selfTransformComponentCache_ = commonCoreMoveCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	// もしコンポーネント用のオブザーバーがあって
	// 移動が許可されていなければ移動しない
	if (auto componentObserverCache_ = m_componentObserverCache.lock())
	{
		const bool isMatching_ = componentObserverCache_->IsEventMatching(StaticID<ComponentEventTagBase>::GetTypeID<MoveInputBasedOnTargetActiveTag>(), CommonEnum::EventLane::Keep, CommonEnum::BoolFlag::False);
		if (isMatching_) 
		{
			// 加算していた移動量を初期値に変換
			m_interpolatorModifier.classData->ResetInterpolate();
			return; 
		}
	}

	// 移動量を取得して一時変数で正規化
	Math::Vector3 inputMoveDirection_ = m_commonCoreMoveInput->GetInputMoveDirection();
	// もし入力方向が全く入力されていなければ更新しない
	if (inputMoveDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }

	// もし移動する際に対象となるゲームオブジェクトの"TransformComponent"が代入されていなければ
	// 既存の"XYZ"軸を参照して動く、そうでなければターゲットの回転を基準に動く
	if (auto boundTransformComponent_ = m_boundTransformComponent->GetBindComponentCache().lock())
	{
		Math::Matrix rotationMat_ = boundTransformComponent_->CreateRotationMatrix();
		inputMoveDirection_.Normalize();

		// 行列を使って移動ベクトルを回転
		Math::Vector3 rotateMoveDirection_ = Math::Vector3::TransformNormal(inputMoveDirection_ , rotationMat_);
		// "Y"座標は移動しない
		rotateMoveDirection_.y = 0.0f;

		rotateMoveDirection_.Normalize();

		const float   deltaTime_  = Application::Instance().GetScaledDeltaTime();
		Math::Vector3 resultMove_ = rotateMoveDirection_ * m_interpolatorModifier.classData->GetCurrentValue() * deltaTime_;
		
		// 自身の"Transform"コンポーネントがあれば移動量を足しこんで座標を確定
		const Math::Vector3 resultPos_ = selfTransformComponentCache_->GetPos() + resultMove_;
		selfTransformComponentCache_->SetPos(resultPos_);
	}
	else
	{
		// 行列がなければ既存の"XYZ"軸を基準に移動
		FixPos(selfTransformComponentCache_);
	}

	// 移動量を更新
	m_interpolatorModifier.classData->Update();
}

void MoveInputBasedOnTargetComponent::ImGuiSpawnDataInspector()
{
	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->ImGuiPrefabDataInspector();
	}
}
void MoveInputBasedOnTargetComponent::ImGuiPrefabDataInspector()
{
	auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance();

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->ImGuiPrefabDataInspector();
	}

	// 変更があれば名前を記録してインスタンス化
	ImGuiUtility::InterpolatorModifierSelector(m_interpolatorModifier.name.c_str() , m_interpolatorModifier);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->ImGuiInspector();
	}
}

void MoveInputBasedOnTargetComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput->DeserializeSpawnData(Json);
	}

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->DeserializeSpawnData(Json);
	}
}
void MoveInputBasedOnTargetComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	const auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance();

	m_interpolatorModifier.name      = Json.value("InterpolatorModifierName" , CommonConstant::STRING_UNKNOWN);
	m_interpolatorModifier.classData = interpolatorModifierFactory_.Create(m_interpolatorModifier.name);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->Init();
		m_interpolatorModifier.classData->DeserializeData(Json);
	}

	if (m_boundTransformComponent)
	{
		m_boundTransformComponent->DeserializePrefabData(Json);
	}
}

nlohmann::json MoveInputBasedOnTargetComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	if (m_boundTransformComponent)
	{
		json_.update(m_boundTransformComponent->SerializeSpawnData());
	}

	return json_;
}
nlohmann::json MoveInputBasedOnTargetComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	if (m_interpolatorModifier.classData)
	{
		auto interpolatorJson_ = m_interpolatorModifier.classData->SerializeData();

		if (!interpolatorJson_.is_null())
		{
			json_.update(interpolatorJson_);
		}
	}

	json_["InterpolatorModifierName"] = m_interpolatorModifier.name;

	if (m_boundTransformComponent)
	{
		json_.update(m_boundTransformComponent->SerializePrefabData());
	}

	return json_;
}

void MoveInputBasedOnTargetComponent::FixPos(std::shared_ptr<TransformComponent> SelfTransformComponent)
{
	if (!SelfTransformComponent) { return; }

	if (!m_commonCoreMoveInput  ) { return; }
	if (!m_interpolatorModifier.classData) { return; }

	// 移動量を取得して一時変数で正規化
	Math::Vector3 inputMoveDirection_ = m_commonCoreMoveInput->GetInputMoveDirection();

	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

	Math::Vector3 resultMove_ = inputMoveDirection_ * m_interpolatorModifier.classData->GetCurrentValue() * deltaTime_;

	// 移動結果を座標に反映
	const Math::Vector3 resultPosition_ = SelfTransformComponent->GetPos() + resultMove_;
	SelfTransformComponent->SetPos(resultPosition_);
}