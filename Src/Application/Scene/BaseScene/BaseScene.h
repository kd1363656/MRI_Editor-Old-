#pragma once

class GameObject;

class ResourceController;

class GameObjectFileSaveAndLoad;
class PrefabFileSaveAndLoad;

class GUIDGameObjectManager;
class GUIDBehaviorTreeManager;

class BaseScene
{

public:

	BaseScene         () = default;
	virtual ~BaseScene() = default;

	virtual void Init();

	void EarlyUpdate();
	void Update     ();
	void LateUpdate () const;

	void PreDraw   ();
	void Draw      ();
	void DrawSprite();
	void DrawDebug ();
	void DrawModel (CommonEnum::StandardShaderTypeFlags Flag);

	// オブジェクトリストを取得
	const std::list<std::shared_ptr<GameObject>>& GetGameObjectList  () const { return m_gameObjectList;   }
	const std::list<std::shared_ptr<GameObject>>& GetUIGameObjectList() const { return m_uiGameObjectList; }

	// オブジェクトリストに追加
	void AddGameObject(const std::shared_ptr<GameObject> AddGameObject);
	
	std::shared_ptr<GameObject> CreateGameObject() const;

	std::weak_ptr<GUIDGameObjectManager>   GetGUIDGameObjectManager  () { return m_guidGameObjectManager;   }
	std::weak_ptr<GUIDBehaviorTreeManager> GetGUIDBehaviorTreeManager() { return m_guidBehaviorTreeManager; }

	std::weak_ptr<ResourceController> GetResourceController() const { return m_resourceController; }

	std::weak_ptr<GameObjectFileSaveAndLoad> GetGameObjectFileSaveAndLoad() { return m_gameObjectFileSaveAndLoad; }
	std::weak_ptr<PrefabFileSaveAndLoad>     GetPrefabFileSaveAndLoad    () { return  m_prefabFileSaveAndLoad;    }

protected:

	virtual std::string_view GetPrefabSaveAndLoadListFilePath      () const = 0;
	virtual std::string_view GetGameObjectSaveAndLoadFilePath      () const = 0;
	virtual std::string_view GetGameObjectGUIDSaveAndLoadFilePath  () const = 0;
	virtual std::string_view GetBehaviorTreeGUIDSaveAndLoadFilePath() const = 0;

	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<GameObject>> m_gameObjectList;
	std::list<std::shared_ptr<GameObject>> m_uiGameObjectList;
	
	std::shared_ptr<GUIDGameObjectManager>   m_guidGameObjectManager   = nullptr;
	std::shared_ptr<GUIDBehaviorTreeManager> m_guidBehaviorTreeManager = nullptr;

	std::shared_ptr<ResourceController> m_resourceController = nullptr;

	std::shared_ptr<GameObjectFileSaveAndLoad> m_gameObjectFileSaveAndLoad = nullptr;
	std::shared_ptr<PrefabFileSaveAndLoad>     m_prefabFileSaveAndLoad     = nullptr;
};
