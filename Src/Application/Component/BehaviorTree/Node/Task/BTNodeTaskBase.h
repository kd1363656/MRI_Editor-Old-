#pragma once
#include "../BTNodeBase.h"

class BTNodeTaskBase : public BTNodeBase
{

public:

	BTNodeTaskBase ()          = default;
	~BTNodeTaskBase() override = default;

	uint32_t GetTypeID() const override { return StaticID<BTNodeBase>::GetTypeID<BTNodeTaskBase>(); }

	void ImGuiNodeInspector() override;

};