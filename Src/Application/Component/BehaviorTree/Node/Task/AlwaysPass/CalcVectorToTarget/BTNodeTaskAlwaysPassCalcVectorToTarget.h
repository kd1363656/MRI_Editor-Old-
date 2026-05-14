#pragma once
#include "../BTNodeTaskAlwaysPassBase.h"

class TransformComponent;

class BTNodeTaskAlwaysPassCalcVectorToTarget : public BTNodeTaskAlwaysPassBase
{

public:

	BTNodeTaskAlwaysPassCalcVectorToTarget ()          = default;
	~BTNodeTaskAlwaysPassCalcVectorToTarget() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeTaskAlwaysPassCalcVectorToTarget>(); }

	void PostLoadInit() override;

	void Tick() override;

private:

	std::weak_ptr<TransformComponent> m_selfTransformComponentCache;

};