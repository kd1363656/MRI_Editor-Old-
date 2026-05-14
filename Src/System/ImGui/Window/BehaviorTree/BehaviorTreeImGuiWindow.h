#pragma once
#include "../ImGuiWindowBase.h"

class BehaviorTreeImGuiWindow : public ImGuiWindowBase
{

public:

	struct BTLink
	{
		uint64_t fromGUID = CommonConstant::UNREGISTERD_GUID;
		uint64_t toGUID   = CommonConstant::UNREGISTERD_GUID;
	};

	BehaviorTreeImGuiWindow ()          = default;
	~BehaviorTreeImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<BehaviorTreeImGuiWindow>(); }

	void Update() override;

private:

};