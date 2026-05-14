#include "PlayerInputConfig.h"

#include "../../System/FileSystem/FileSystem.h"
#include "../../System/Input/RawInputManager.h"

void PlayerInputConfig::Init()
{
	LoadConfig();
	m_mouseConfigList[CommonEnum::UseMouseInputFlags::ClickMouseLeft] = VK_LBUTTON;
	SaveConfig();
}

void PlayerInputConfig::LoadConfig()
{
	auto json_ = FileSystem::LoadJsonFile(INPUT_CONFIG_FILE_PATH);

	if (json_.is_null()) { return; }

	// リストに読み取り結果を反映
	DeserializeConfig("KeyConfigList"   , json_ , m_keyConfigList  );
	DeserializeConfig("MouseConfigList" , json_ , m_mouseConfigList);
}
void PlayerInputConfig::SaveConfig()
{
	auto rootJson_ = nlohmann::json();

	rootJson_.update(SerializeConfig("KeyConfigList"  , m_keyConfigList  ));
	rootJson_.update(SerializeConfig("MouseConfigList", m_mouseConfigList));

	FileSystem::SaveJsonFile(rootJson_ , INPUT_CONFIG_FILE_PATH);
}

int PlayerInputConfig::GetKeyConfigData(CommonEnum::UseKeyInputFlags Key)
{
	auto itr_ = m_keyConfigList.find(Key);

	// キー情報が登録されていなければ"NULL"を返す
	if (itr_ == m_keyConfigList.end())
	{
		return NULL;
	}

	return itr_->second;
}
int PlayerInputConfig::GetMouseConfigData(CommonEnum::UseMouseInputFlags Mouse)
{
	auto itr_ = m_mouseConfigList.find(Mouse);

	// キー情報が登録されていなければ"NULL"を返す
	if (itr_ == m_mouseConfigList.end())
	{
		return NULL;
	}

	return itr_->second;
}