#pragma once
#include "../../Factory/Strategy/StrategyFactory.h"

namespace StrategyUtility
{
	template <class ConcreateStrategy , class ComponentType>
		requires std::derived_from<ConcreateStrategy, IStrategy<ComponentType>>
	inline void ChangeStrategyExecuteOnce(const std::shared_ptr<ComponentType>& Self , std::shared_ptr<StrategyInterface<ComponentType>>& CurrentStrategy)
	{
		CurrentStrategy = std::make_shared<ConcreateStrategy>();
		CurrentStrategy->Execute(Self);
	}

	template <class ConcreateStrategy, class ComponentType>
		requires std::derived_from<ConcreateStrategy, IStrategy<ComponentType>>
	inline void ChangeStrategy(std::shared_ptr<IStrategy<ComponentType>>& CurrentStrategy)
	{
		CurrentStrategy = std::make_shared<ConcreateStrategy>();
	}
}