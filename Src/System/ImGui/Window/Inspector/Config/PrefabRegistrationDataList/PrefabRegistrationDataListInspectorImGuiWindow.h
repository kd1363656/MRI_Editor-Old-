#pragma once
#include "../../../ImGuiWindowBase.h"

#include "../Src/Application/Utility/Common/CommonStruct.h"
#include "../Src/Application/Utility/Common/CommonConstant.h"
#include "../Src/Application/Utility/ImGui/ImGuiUtility.h"

class PrefabRegistrationDataListInspectorImGuiWindow : public ImGuiWindowBase
{
public:

	PrefabRegistrationDataListInspectorImGuiWindow ()          = default;
	~PrefabRegistrationDataListInspectorImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<PrefabRegistrationDataListInspectorImGuiWindow>(); }

	void Update() override;

	void UpdatePrefabRegistrationDataSelector                ();
	void UpdateAddPrefabRegistrationDataButton               ();
	void UpdatePrefabRegistrationDataName                    ();
	void UpdatePrefabRegistrationDataFilePath                ();
	void UpdatePrefabRegistrationDataListInspectorImGuiWindow();

private:

	CommonStruct::PrefabRegistrationData m_prefabRegistrationData;

	char m_inputPrefabName[ImGuiUtility::MAX_TEXT_BUFFER] = { 0 };
};