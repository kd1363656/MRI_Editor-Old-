#pragma once
#include "../ImGuiConfigBase.h"

#include "../../../../Application/Utility/Common/CommonStruct.h"

class SceneViewImGuiConfig : public ImGuiConfigBase
{

public:

	SceneViewImGuiConfig () = default;
	~SceneViewImGuiConfig() override { SaveFile(); }

	uint32_t GetTypeID() const override { return StaticID<ImGuiConfigBase>::GetTypeID<SceneViewImGuiConfig>(); }

	void Init() override;

	void LoadFile() override;

	void SaveFile() override;

	KdRenderTargetChanger& GetRenderTargetChanger() { return m_renderTargetChanger; }
	KdRenderTargetPack&    GetRenderTargetPack   () { return m_renderTargetPack;    }

	const CommonStruct::WindowSize& GetSceneViewWindowSize() const { return m_sceneViewWidowSize; }

private:

	const std::string CONFIG_FILE_PATH = "Asset/Data/Config/ImGui/SceneView/SceneViewConfig.json";

	KdRenderTargetChanger m_renderTargetChanger;
	KdRenderTargetPack    m_renderTargetPack;

	CommonStruct::WindowSize m_sceneViewWidowSize = {};
};