#pragma once
#include "../../GenericFactoryBase.h"
#include "../../../Component/BehaviorTree/BlackBoard/Data/IBTBlackBoardData.h"

class BTBlackBoardDataFactory : public GenericFactoryBase<BTBlackBoardDataFactory, IBTBlackBoardData>
{

public:

	void Init() override;
	
private:

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<BTBlackBoardDataFactory>;

	BTBlackBoardDataFactory ()          = default;
	~BTBlackBoardDataFactory() override = default;

};