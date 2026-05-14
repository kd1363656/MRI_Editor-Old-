#pragma once
#include "../../ImGuiWindowBase.h"

#include "../Src/Application/Utility/Common/CommonStruct.h"

class PrefabDataInspectorImGuiWindow : public ImGuiWindowBase
{

public:

	PrefabDataInspectorImGuiWindow ()          = default;
	~PrefabDataInspectorImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<PrefabDataInspectorImGuiWindow>(); }

	void Update() override;

	// プレハブデータを所定の位置保存
	void SavePrefabData();

	void UpdateAddPrefabData     () const;
	void UpdateComponentInspector(std::shared_ptr<GameObject> GameObject) const;

};