#pragma once
#include "../../ImGuiWindowBase.h"

class PrefabSceneFilePathListInspectorImGuiWindow : public ImGuiWindowBase
{

public:

	PrefabSceneFilePathListInspectorImGuiWindow()          = default;
	~PrefabSceneFilePathListInspectorImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<PrefabSceneFilePathListInspectorImGuiWindow>(); }

	void Update() override;

	void UpdateAddScenePrefabFilePath          ();
	void UpdateScenePrefabFilePathListInspector();

private:

	std::string m_selectedSceneLoadPrefabName = "";

};