#pragma once
#include "../BTNodeTaskBase.h"

class BTNodeTaskAlwaysPassBase : public BTNodeTaskBase
{

public:

	BTNodeTaskAlwaysPassBase ()          = default;
	~BTNodeTaskAlwaysPassBase() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeTaskAlwaysPassBase>(); }

	void Tick() override
	{
		SetNodeState(BTNodeBase::NodeState::Success);
	}
};