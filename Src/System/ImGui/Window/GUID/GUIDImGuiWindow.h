#pragma once
#include "../ImGuiWindowBase.h"

class GUIDImGuiWindow : public ImGuiWindowBase
{

public:

	GUIDImGuiWindow ()          = default;
	~GUIDImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<GUIDImGuiWindow>(); }

	void Update() override;
	
};