#pragma once

#include "../../Strategy/IStrategy.h"
#include "../GenericFactoryBase.h"

template<class OwnerType>
class StrategyFactory : public GenericFactoryBase<StrategyFactory<OwnerType> , IStrategy<OwnerType>>
{

private:

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<StrategyFactory<OwnerType>>;

	StrategyFactory ()          = default;
	~StrategyFactory() override = default;

};