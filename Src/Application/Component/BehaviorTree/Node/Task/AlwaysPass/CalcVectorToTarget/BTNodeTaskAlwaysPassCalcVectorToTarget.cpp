#include "BTNodeTaskAlwaysPassCalcVectorToTarget.h"

#include "../../../../../../Component/Transform/TransformComponent.h"

#include "../../../../BlackBoard/Data/TargetTransformComponentCache/BTBlackBoardTargetTransformComponentCacheData.h"
#include "../../../../BlackBoard/Data/CalcVectorToTarget/BTBlackBoardCalcVectorToTargetData.h"
#include "../../../../BlackBoard/Data/CalcDistanceToTarget/BTBlackBoardCalcDistanceToTargetData.h"

void BTNodeTaskAlwaysPassCalcVectorToTarget::PostLoadInit()
{
	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	auto owner_ = blackBoardCache_->GetOwnerCache().lock();
	if (!owner_) { return; }

	BTNodeTaskBase::PostLoadInit();

	m_selfTransformComponentCache = owner_->GetComponent<TransformComponent>();
}

void BTNodeTaskAlwaysPassCalcVectorToTarget::Tick()
{
	auto selfTransformComponentCache_ = m_selfTransformComponentCache.lock();
	if (!selfTransformComponentCache_) { return; }

	auto blackBoardCache_ = GetBlackBoardCache().lock();
	if (!blackBoardCache_) { return; }

	auto sharedInfoCache_ =  blackBoardCache_->GetSharedInfo<BTBlackBoardTargetTransformComponentCacheData, std::shared_ptr<CommonCoreGUIDBinder<TransformComponent>>>().lock();
	if (!sharedInfoCache_) { return; }
	
	auto boundTransformComponentCache_ = sharedInfoCache_->Get()->GetBindComponentCache().lock();
	if (!boundTransformComponentCache_) { return; }

	const Math::Vector3 selfPos_   = selfTransformComponentCache_->GetPos ();
	const Math::Vector3 targetPos_ = boundTransformComponentCache_->GetPos();

	const Math::Vector3 toTarget_ = targetPos_ - selfPos_;

	// 算出した距離を代入
	blackBoardCache_->SetSharedInfo<BTBlackBoardCalcDistanceToTargetData>(toTarget_);

	// 算出した距離から自身から敵の方向へのベクトルを算出
	Math::Vector3 vector_ = toTarget_;
	vector_.Normalize();

	// 算出した方向ベクトルを代入
	blackBoardCache_->SetSharedInfo<BTBlackBoardCalcVectorToTargetData>(vector_);

	BTNodeTaskAlwaysPassBase::Tick();
}