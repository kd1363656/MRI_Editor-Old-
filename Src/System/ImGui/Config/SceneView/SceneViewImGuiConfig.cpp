#include "SceneViewImGuiConfig.h"
#include "../../../FileSystem/FileSystem.h"

#include "../../../../Application/Utility/Json/JsonUtility.h"

void SceneViewImGuiConfig::Init()
{
	LoadFile();

	m_renderTargetPack.CreateRenderTarget(m_sceneViewWidowSize.width , m_sceneViewWidowSize.height , true);
}

void SceneViewImGuiConfig::LoadFile()
{
	auto json_ = FileSystem::LoadJsonFile(CONFIG_FILE_PATH);
	
	if (json_.is_null()) { return; }
	
	if (json_.contains("SceneViewWindowSize")) { m_sceneViewWidowSize = JsonUtility::JsonToWindowSize(json_["SceneViewWindowSize"]); }
}

void SceneViewImGuiConfig::SaveFile()
{
	auto json_ = nlohmann::json();

	json_["SceneViewWindowSize"] = JsonUtility::WindowSizeToJson(m_sceneViewWidowSize);

	FileSystem::SaveJsonFile(json_ , CONFIG_FILE_PATH);
}