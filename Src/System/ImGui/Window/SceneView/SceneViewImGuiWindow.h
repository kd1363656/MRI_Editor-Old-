#pragma once
#include "../ImGuiWindowBase.h"

class SceneViewImGuiConfig;

class SceneViewImGuiWindow : public ImGuiWindowBase
{

public:

	SceneViewImGuiWindow ()          = default;
	~SceneViewImGuiWindow() override = default;

	uint32_t GetTypeID() const override { return StaticID<ImGuiWindowBase>::GetTypeID<SceneViewImGuiWindow>(); }

	void Update               () override;
	void UpdateSceneViewWindow(std::shared_ptr<SceneViewImGuiConfig> SceneViewImGuiConfig);

	bool GetIsCursorInScreen() const { return m_isCursorInScreen; }

private:

	bool m_isCursorInScreen = false;
};