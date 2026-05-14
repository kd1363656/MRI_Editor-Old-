#pragma once
#include "../BTNodeCompositeBase.h"

class BTNodeCompositeSequence : public BTNodeCompositeBase
{

public:

	BTNodeCompositeSequence ()          = default;
	~BTNodeCompositeSequence() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeCompositeSequence>(); }

	void Tick() override;
};