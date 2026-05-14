#pragma once
#include "../BTNodeCompositeBase.h"

class BTNodeCompositeSelector : public BTNodeCompositeBase
{

public:

	BTNodeCompositeSelector ()          = default;
	~BTNodeCompositeSelector() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeCompositeSelector>(); }

	void Tick() override;
};