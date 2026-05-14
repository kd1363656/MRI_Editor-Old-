#pragma once
#include "../BTNodeDecoratorBase.h"

class BTNodeDecoratorInverterBase : public BTNodeDecoratorBase
{

public:

	BTNodeDecoratorInverterBase ()          = default;
	~BTNodeDecoratorInverterBase() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeDecoratorInverterBase>(); }

	void Tick() override;

	virtual bool IsConditionMatched() = 0;

};