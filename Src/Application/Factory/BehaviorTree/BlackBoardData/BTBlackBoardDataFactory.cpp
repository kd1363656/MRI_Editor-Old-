#include "BTBlackBoardDataFactory.h"

#include "../../../Component/BehaviorTree/BlackBoard/Data/CalcDistanceToTarget/BTBlackBoardCalcDistanceToTargetData.h"
#include "../../../Component/BehaviorTree/BlackBoard/Data/CalcVectorToTarget/BTBlackBoardCalcVectorToTargetData.h"
#include "../../../Component/BehaviorTree/BlackBoard/Data/TargetTransformComponentCache/BTBlackBoardTargetTransformComponentCacheData.h"
#include "../../../Component/BehaviorTree/BlackBoard/Data/FinishedRandomMove/BTBlackBoardFinishedRandomMoveData.h"

void BTBlackBoardDataFactory::Init()
{
	// コンポジットノード
	RegisterFactoryMethod<BTBlackBoardCalcVectorToTargetData>			();
	RegisterFactoryMethod<BTBlackBoardCalcDistanceToTargetData>			();
	RegisterFactoryMethod<BTBlackBoardTargetTransformComponentCacheData>();
	RegisterFactoryMethod<BTBlackBoardFinishedRandomMoveData>		    ();
}