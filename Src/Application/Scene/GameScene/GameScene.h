#pragma once

#include"../BaseScene/BaseScene.h"

class GameScene : public BaseScene
{

public:

	GameScene ()          = default;
	~GameScene() override = default;

	void Init() override;

private:

	const std::string GAME_OBJECT_SAVE_AND_LOAD_FILE_PATH        = "Asset/Data/Scene/GameScene/LoadGameObject";
	const std::string PREFAB_SAVE_AND_LOAD_LIST_FILE_PATH        = "Asset/Data/Scene/GameScene/LoadPrefabList";
	const std::string GAME_OBJECT_GUID_SAVE_AND_LOAD_FILE_PATH   = "Asset/Data/Scene/GameScene/LoadGameObjectGUID";
	const std::string BEHAVIOR_TREE_GUID_SAVE_AND_LOAD_FILE_PATH = "Asset/Data/Scene/GameScene/LoadBehaviorTreeGUID";

	std::string_view GetGameObjectSaveAndLoadFilePath      () const override { return GAME_OBJECT_SAVE_AND_LOAD_FILE_PATH;        }
	std::string_view GetPrefabSaveAndLoadListFilePath      () const override { return PREFAB_SAVE_AND_LOAD_LIST_FILE_PATH;        }
	std::string_view GetGameObjectGUIDSaveAndLoadFilePath  () const override { return GAME_OBJECT_GUID_SAVE_AND_LOAD_FILE_PATH;   }
	std::string_view GetBehaviorTreeGUIDSaveAndLoadFilePath() const override { return BEHAVIOR_TREE_GUID_SAVE_AND_LOAD_FILE_PATH; }

};
