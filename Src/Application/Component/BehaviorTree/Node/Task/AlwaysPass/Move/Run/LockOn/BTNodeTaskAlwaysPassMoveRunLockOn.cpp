#include "BTNodeTaskAlwaysPassMoveRunLockOn.h"

#include "../../../../../../../Transform/TransformComponent.h"

#include "../../../../../../../../CommonCore/Move/Input/CommonCoreMoveInput.h"
#include "../../../../../../../../CommonCore/Move/CommonCoreMove.h"

#include "../../../../../../../../../System/Calculation/Calculation.h"

#include "../../../../../../BlackBoard/BTBlackBoard.h"
#include "../../../../../../BlackBoard/Data/CalcVectorToTarget/BTBlackBoardCalcVectorToTargetData.h"
#include "../../../../../../BlackBoard/Data/CalcDistanceToTarget/BTBlackBoardCalcDistanceToTargetData.h"

#include "../../../../../../../../Modifier/Interporator/InterpolatorModifierBase.h"
#include "../../../../../../../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

void BTNodeTaskAlwaysPassMoveRunLockOn::Init()
{
	BTNodeTaskBase::Init();

	if (!m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput = std::make_shared<CommonCoreMoveInput>();
	}

	m_commonCoreMoveInput->Init();
}
void BTNodeTaskAlwaysPassMoveRunLockOn::PostLoadInit()
{
	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	auto owner_ = blackBoardCache_->GetOwnerCache().lock();
	if (!owner_) { return; }

	BTNodeTaskBase::PostLoadInit();

	if (m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput->PostLoadInit(owner_);
	}
}

void BTNodeTaskAlwaysPassMoveRunLockOn::Tick()
{
	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	if (!m_commonCoreMoveInput) { return; }

	auto commonCoreMoveCache_ = m_commonCoreMoveInput->GetCommonCoreMoveCache().lock();
	if (!commonCoreMoveCache_) { return; }

	// ※注意 "AlwaysPass"ノードにある毎フレーム距離と方向ベクトルを算出するノードが先に実行されること前提の設計
	auto toTargetVectorData_ = blackBoardCache_->GetSharedInfo<BTBlackBoardCalcVectorToTargetData , Math::Vector3>().lock();
	if (!toTargetVectorData_) { return; }

	auto selfTransformComponentCache_ = commonCoreMoveCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	if (!m_interpolatorModifier.classData) { return; }

	// 入力方向を正面に固定(ターゲットに向かって直進し続ける)
	m_commonCoreMoveInput->SetInputMoveDirection(Math::Vector3::Forward);

	// 移動方向の入力がなければ処理を実行しない
	if (m_commonCoreMoveInput->GetInputMoveDirection().LengthSquared() <= CommonConstant::EPSILON) { return; }

	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

	// ターゲットの方向を常に向くように回転率から求める
	{
		Math::Vector3 toTargetDirection_ = toTargetVectorData_->Get();

		// 対象となるゲームオブジェクトの"Y"軸は参照しない
		toTargetDirection_.y = 0.0f;
		toTargetDirection_.Normalize();

		if (toTargetDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }

		// 対象ゲームオブジェクトに向き合うようにするためにベクトルを反転
		const float targetYaw_ = Calculation::VectorToYaw(-toTargetDirection_);

		Math::Quaternion targetQuaternion_  = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::Up , targetYaw_);
		Math::Quaternion currentQuaternion_ = selfTransformComponentCache_->GetRotation();

		// 内積が反転していれば回転対象となるクオータニオンの符号を反転
		if (currentQuaternion_.Dot(targetQuaternion_) < 0.0f)
		{
			targetQuaternion_ = -targetQuaternion_;
		}

		Math::Quaternion resultQuaternion_ = Math::Quaternion::Slerp
		(
			currentQuaternion_												,
			targetQuaternion_											    ,
			m_interpolatorModifier.classData->GetCurrentValue() * deltaTime_
		);

		selfTransformComponentCache_->SetRotation(resultQuaternion_);
	}

	// 自身の回転率をもとに移動方向へ移動
	{
		Math::Vector3 targetDirection_ = Math::Vector3::TransformNormal(m_commonCoreMoveInput->GetInputMoveDirection() , selfTransformComponentCache_->CreateRotationMatrix());
		targetDirection_.y = 0.0f;
		targetDirection_.Normalize();

		if (targetDirection_.LengthSquared() <= CommonConstant::EPSILON) { return; }

		Math::Vector3 resultPos_ = selfTransformComponentCache_->GetPos() + (m_interpolatorModifier.classData->GetCurrentValue() * targetDirection_ * deltaTime_);

		selfTransformComponentCache_->SetPos(resultPos_);
	}

	m_interpolatorModifier.classData->Update();

	BTNodeTaskAlwaysPassBase::Tick();
}

void BTNodeTaskAlwaysPassMoveRunLockOn::ImGuiPrefabDataInspector()
{
	ImGuiUtility::InterpolatorModifierSelector("InterpolatorSelector" , m_interpolatorModifier);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->ImGuiInspector();
	}
}

void BTNodeTaskAlwaysPassMoveRunLockOn::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializePrefabData(Json);

	const auto& interpolatorModifierFactory_ = InterpolatorModifierFactory::GetInstance();

	m_interpolatorModifier.name      = Json.value("InterpolatorModifierName" , CommonConstant::STRING_UNKNOWN);
	m_interpolatorModifier.classData = interpolatorModifierFactory_.Create(m_interpolatorModifier.name);

	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->Init();
		m_interpolatorModifier.classData->DeserializeData(Json);
	}
}

nlohmann::json BTNodeTaskAlwaysPassMoveRunLockOn::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	{
		auto baseJson_ = BTNodeBase::SerializePrefabData();

		if (!baseJson_.is_null())
		{
			json_.update(baseJson_);
		}
	}

	if (m_interpolatorModifier.classData)
	{
		auto interpolatorJson_ = m_interpolatorModifier.classData->SerializeData();

		if (!interpolatorJson_.is_null())
		{
			json_.update(interpolatorJson_);
		}
	}

	json_["InterpolatorModifierName"] = m_interpolatorModifier.name;

	return json_;
}