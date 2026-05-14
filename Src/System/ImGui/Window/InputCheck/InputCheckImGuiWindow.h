#pragma once
#include "../ImGuiWindowBase.h"
#include "../../../../Application/Utility/Common/CommonEnum.h"

class InputCheckImGuiWindow : public ImGuiWindowBase
{

public:

	InputCheckImGuiWindow()          = default;
	~InputCheckImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<InputCheckImGuiWindow>(); }

	void Update() override;

	void PressKeyInputCheck  (const char* Label , CommonEnum::UseKeyInputFlags   UseKeyFlag  );
	void ClickMouseInputCheck(const char* Label , CommonEnum::UseMouseInputFlags UseMouseFlag);
};