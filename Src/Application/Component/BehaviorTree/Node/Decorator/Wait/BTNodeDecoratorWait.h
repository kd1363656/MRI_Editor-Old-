#pragma once
#include "../BTNodeDecoratorBase.h"

class BTNodeDecoratorWait : public BTNodeDecoratorBase
{

public:

	BTNodeDecoratorWait ()          = default;
	~BTNodeDecoratorWait() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeDecoratorWait>(); }

	void Init() override;
	void Tick() override;
	
	void ImGuiPrefabDataInspector() override;

private:

	float m_waitTime = CommonConstant::FLOAT_ONE;
	float m_waitCnt  = 0.0f;

};