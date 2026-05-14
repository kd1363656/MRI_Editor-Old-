#include "BTNodeFactory.h"

#include "../../../Component/BehaviorTree/BlackBoard/BTBlackBoard.h"

#include "../../../Component/BehaviorTree/Node/Composite/BTNodeCompositeBase.h"
#include "../../../Component/BehaviorTree/Node/Composite/Sequence/BTNodeCompositeSequence.h"
#include "../../../Component/BehaviorTree/Node/Composite/Selector/BTNodeCompositeSelector.h"

#include "../../../Component/BehaviorTree/Node/Branch/WithinDistance/BTNodeWithinDistanceBranch.h"

#include "../../../Component/BehaviorTree/Node/Decorator/Wait/BTNodeDecoratorWait.h"

#include "../../../Component/BehaviorTree/Node/Task/Move/Random/LockOn/BTNodeTaskMoveRandomLockOn.h"
#include "../../../Component/BehaviorTree/Node/Task/AlwaysPass/Move/Run/LockOn/BTNodeTaskAlwaysPassMoveRunLockOn.h"
#include "../../../Component/BehaviorTree/Node/Task/AlwaysPass/CalcVectorToTarget/BTNodeTaskAlwaysPassCalcVectorToTarget.h"

void BTNodeFactory::Init()
{
	// コンポジットノード
	RegisterFactoryMethod<BTNodeCompositeSelector>();
	RegisterFactoryMethod<BTNodeCompositeSequence>();

	// ブランチノード
	RegisterFactoryMethod<BTNodeWithinDistanceBranch>();

	// デコレーターノード
	RegisterFactoryMethod<BTNodeDecoratorWait>();
	
	// タスクノード
	RegisterFactoryMethod<BTNodeTaskMoveRandomLockOn>();

	// タスクノード(絶対に成功判定を返す)
	RegisterFactoryMethod<BTNodeTaskAlwaysPassMoveRunLockOn>     ();
	RegisterFactoryMethod<BTNodeTaskAlwaysPassCalcVectorToTarget>();

	InitTypeToName();
}

void BTNodeFactory::GetTypeToNameVector(uint32_t ID, std::vector<std::string>& Vector) const
{
	auto itr_ = m_typeToNameList.find(ID);
	if (itr_ == m_typeToNameList.end()) { return; }

	Vector = itr_->second;
}

void BTNodeFactory::InitTypeToName()
{
	// コンポジットノード
	RegisterTypeToName<BTNodeCompositeBase , BTNodeCompositeSelector>();
	RegisterTypeToName<BTNodeCompositeBase , BTNodeCompositeSequence>();
}