#pragma once
#include "../ImGuiWindowBase.h"

class MenuBarImGuiWindow : public ImGuiWindowBase
{

public:

	MenuBarImGuiWindow ()          = default;
	~MenuBarImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<MenuBarImGuiWindow>(); }

	void Update                         () override;
	void UpdateSaveAllDataWindow        () const;
	void UpdateReLoadAllDataWindow      ();
	void UpdateDrawDebugWireSelectWindow();

};