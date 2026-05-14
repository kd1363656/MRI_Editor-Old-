#include "BTNodeTaskMoveRandomLockOn.h"

#include "../../../../../../Transform/TransformComponent.h"

#include "../../../../../../../CommonCore/Move/Input/CommonCoreMoveInput.h"
#include "../../../../../../../CommonCore/Move/CommonCoreMove.h"

#include "../../../../../../../../System/Calculation/Calculation.h"

#include "../../../../../BlackBoard/BTBlackBoard.h"
#include "../../../../../BlackBoard/Data/CalcVectorToTarget/BTBlackBoardCalcVectorToTargetData.h"
#include "../../../../../BlackBoard/Data/CalcDistanceToTarget/BTBlackBoardCalcDistanceToTargetData.h"
#include "../../../../../BlackBoard/Data/FinishedRandomMove/BTBlackBoardFinishedRandomMoveData.h"

#include "../../../../../../../Factory/Modifier/Interpolator/InterpolatorFactory.h"

void BTNodeTaskMoveRandomLockOn::Init()
{
	BTNodeTaskBase::Init();

	if (!m_commonCoreMoveInput)
	{
		m_commonCoreMoveInput = std::make_shared<CommonCoreMoveInput>();
	}

	m_commonCoreMoveInput->Init();

	InitRandDirectionWeightList(BTNodeTaskMoveRandomLockOn::MoveDirection::Right   );
	InitRandDirectionWeightList(BTNodeTaskMoveRandomLockOn::MoveDirection::Left    );
	InitRandDirectionWeightList(BTNodeTaskMoveRandomLockOn::MoveDirection::Forward );
	InitRandDirectionWeightList(BTNodeTaskMoveRandomLockOn::MoveDirection::Backward);

	m_prevMoveDirection = BTNodeTaskMoveRandomLockOn::MoveDirection::MaxSize;

	m_directionChangeElapsedTimer = 0.0f;
	m_directionFixTime			  = 0.0f;

	m_directionChangeCnt    = 0;
	m_maxDirectionChangeCnt = 0;

	m_isDirectionChangeTimerInitialized = false;
}
void BTNodeTaskMoveRandomLockOn::PostLoadInit()
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

void BTNodeTaskMoveRandomLockOn::Tick()
{
	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	// 移動方向変換回数が設定されている回数を超えたことを通知 
	auto sharedInfo_ = blackBoardCache_->GetSharedInfo<BTBlackBoardFinishedRandomMoveData, bool>().lock();
	if (!sharedInfo_) { return; }

	// もし基底回数動き回っていたなら外部ノードで初期化されるまでノード状態を"Fail"で返す
	const bool isFinishedMove_ = sharedInfo_->Get();
	if (isFinishedMove_) 
	{
		SetNodeState(BTNodeBase::NodeState::Fail);
		return;
	}

	if (!m_commonCoreMoveInput) { return; }

	FixMoveRandomDirection();

	// 移動方向の入力がなければ処理を実行しない
	if (m_commonCoreMoveInput->GetInputMoveDirection().LengthSquared() <= CommonConstant::EPSILON) { return; }

	Rotation();
	Move    ();

	SetNodeState(BTNodeBase::NodeState::Success);
}

void BTNodeTaskMoveRandomLockOn::ImGuiPrefabDataInspector()
{
	ImGuiUtility::InterpolatorModifierSelector("InterpolatorSelector" , m_interpolatorModifier);

	// 選択された名前のクラスをインスタンス化
	if (m_interpolatorModifier.classData)
	{
		m_interpolatorModifier.classData->ImGuiInspector();
	}

	ImGui::Text     ("DirectionChangeElapsedTimer : %.2f" , m_directionChangeElapsedTimer      );
	ImGui::DragFloat("DirectionFixTime"                   , &m_directionFixTime          , 0.1f);

	ImGui::Text   ("DirectionChangeCnt : %d" , m_directionChangeCnt	       );
	ImGui::DragInt("MaxDirectionChangeCnt"   , &m_maxDirectionChangeCnt , 1);
	
	ImGui::Checkbox("IsDirectionChangeTimerInitialized" , &m_isDirectionChangeTimerInitialized);
}

void BTNodeTaskMoveRandomLockOn::DeserializePrefabData(const nlohmann::json& Json)
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

	m_directionFixTime	    = Json.value("DirectionFixTime"      , 0.0f);
	m_maxDirectionChangeCnt = Json.value("MaxDirectionChangeCnt" , 0   );
}
nlohmann::json BTNodeTaskMoveRandomLockOn::SerializePrefabData()
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

	json_["DirectionFixTime"     ] = m_directionFixTime;
	json_["MaxDirectionChangeCnt"] = m_maxDirectionChangeCnt;

	return json_;
}

void BTNodeTaskMoveRandomLockOn::InitRandDirectionWeightList(BTNodeTaskMoveRandomLockOn::MoveDirection Index)
{
	// インデックスが最大容量なら"return"
	if (Index == BTNodeTaskMoveRandomLockOn::MoveDirection::MaxSize) { return; }

	const int index_ = static_cast<int>(Index);
	if (m_randDirectionWeightList.size() <= index_) { return; }

	m_randDirectionWeightList[index_].weight = NORMAL_WEIGHT;
	m_randDirectionWeightList[index_].type   = static_cast<int>(Index);
}
void BTNodeTaskMoveRandomLockOn::ChangeWeightByPrevMoveDirection()
{
	// 前回選択された移動方向への抽選度合いが高くなるように設定
	if (m_prevMoveDirection == BTNodeTaskMoveRandomLockOn::MoveDirection::MaxSize) { return; }

	const int index_ = static_cast<int>(m_prevMoveDirection);
	if (m_randDirectionWeightList.size() <= index_) { return; }

	m_randDirectionWeightList[index_].weight = HIGH_WEIGHT;
}
void BTNodeTaskMoveRandomLockOn::ResetWeight()
{
	for (auto& list_ : m_randDirectionWeightList)
	{
		list_.weight = NORMAL_WEIGHT;
	}
}

void BTNodeTaskMoveRandomLockOn::FixMoveDirection(BTNodeTaskMoveRandomLockOn::MoveDirection Direction, Math::Vector3& MoveDirection)
{
	// 最大容量を超えていないかを確認
	const int index_ = static_cast<int>(Direction);
	if (index_ >= static_cast<int>(BTNodeTaskMoveRandomLockOn::MoveDirection::MaxSize)) { return; }

	if (Direction == BTNodeTaskMoveRandomLockOn::MoveDirection::Right)
	{
		MoveDirection = Math::Vector3::Right;
	}
	else if (Direction == BTNodeTaskMoveRandomLockOn::MoveDirection::Left)
	{
		MoveDirection = Math::Vector3::Left;
	}
	else if (Direction == BTNodeTaskMoveRandomLockOn::MoveDirection::Forward)
	{
		MoveDirection = Math::Vector3::Forward;
	}
	else if (Direction == BTNodeTaskMoveRandomLockOn::MoveDirection::Backward)
	{
		MoveDirection = Math::Vector3::Backward;
	}
}

void BTNodeTaskMoveRandomLockOn::FixMoveRandomDirection()
{
	const auto& random_ = CalculationRandom::GetInstance();
	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

	if (!m_commonCoreMoveInput) { return; }

	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	// もし移動方向変換回数が設定されている回数を超えていなければ実行
	if (m_directionChangeCnt < m_maxDirectionChangeCnt)
	{
		// このノードに"Tick"が走った際にすぐに移動できないようにしないための処理
		if (!m_isDirectionChangeTimerInitialized)
		{
			m_directionChangeElapsedTimer = m_directionFixTime;
			ResetWeight();

			m_isDirectionChangeTimerInitialized = true;
		}

		// 移動方向が確定される時間まで入力方向を代入しない
		if (m_directionChangeElapsedTimer <= m_directionFixTime)
		{
			m_directionChangeElapsedTimer += deltaTime_;
		}
		// 移動方向の確定
		else
		{
			m_directionChangeCnt++;
			m_directionChangeElapsedTimer = 0.0f;

			// 前回の移動方向の重みが高くなるようにする
			ChangeWeightByPrevMoveDirection();

			BTNodeTaskMoveRandomLockOn::MoveDirection moveDirection_ = static_cast<BTNodeTaskMoveRandomLockOn::MoveDirection>(random_.GetRandomWeight<int>(m_randDirectionWeightList));
			FixMoveDirection(moveDirection_ , m_commonCoreMoveInput->GetWorkInputMoveDirection());

			// 現在の移動方向を過去の移動方向として記録
			m_prevMoveDirection = moveDirection_;

			// 重みを消去
			ResetWeight();

			// 今の"moveDirection"を前回の"moveDirection"として保存
			m_prevMoveDirection = moveDirection_;
		}
	}
	else
	{
		// 初期化
		m_isDirectionChangeTimerInitialized = false;
		
		// 移動方向変換回数が設定されている回数を超えたことを通知 
		blackBoardCache_->SetSharedInfo<BTBlackBoardFinishedRandomMoveData , bool>(true);
	}
}
void BTNodeTaskMoveRandomLockOn::Rotation()
{
	if (!m_commonCoreMoveInput)			   { return; }
	if (!m_interpolatorModifier.classData) { return; }

	auto commonCoreMoveCache_ = m_commonCoreMoveInput->GetCommonCoreMoveCache().lock();
	if (!commonCoreMoveCache_) { return; }

	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	// ※注意 "AlwaysPass"ノードにある毎フレーム距離と方向ベクトルを算出するノードが先に実行されること前提の設計
	auto toTargetVectorData_ = blackBoardCache_->GetSharedInfo<BTBlackBoardCalcVectorToTargetData , Math::Vector3>().lock();
	if (!toTargetVectorData_) { return; }

	auto selfTransformComponentCache_ = commonCoreMoveCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

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
			targetQuaternion_												,
			m_interpolatorModifier.classData->GetCurrentValue() * deltaTime_
		);

		selfTransformComponentCache_->SetRotation(resultQuaternion_);
	}
}
void BTNodeTaskMoveRandomLockOn::Move()
{
	if (!m_commonCoreMoveInput) { return; }

	if (!m_interpolatorModifier.classData) { return; }

	if (m_commonCoreMoveInput->GetInputMoveDirection().LengthSquared() <= CommonConstant::EPSILON) { return; }

	auto commonCoreMoveCache_ = m_commonCoreMoveInput->GetCommonCoreMoveCache().lock();
	if (!commonCoreMoveCache_) { return; }

	auto selfTransformComponentCache_ = commonCoreMoveCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

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
}