#pragma once

class ComponentObserver;

// TODO "Json"の読み込みが汚いので修正必須
class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

	struct MultiComponentDump
	{
		std::shared_ptr<ComponentBase> component       = nullptr;
		bool						   isLoadSpawnData = false;
	};

	struct ComponentDumpData
	{
		std::unordered_map<std::string , std::shared_ptr<ComponentBase>            , CommonStruct::StringHash, std::equal_to<>> uniqueComponent;
		std::unordered_map<std::string , std::list<GameObject::MultiComponentDump> , CommonStruct::StringHash, std::equal_to<>> multi;
	};

	GameObject () = default;
	~GameObject() = default;

	void Init        ();
	void PostLoadInit() const;

	void EarlyUpdate();
	void Update     () const;
	void LateUpdate () const;

	void OnPostCollision		();
	void OnPostCollisionResolved();

	void FixMatrix();

	// "Prefab"を読み込んでから"SpawnData"を読み込む設計(プレハブのオーバーライド機能)
	void Deserialize(const std::string_view PrefabName , const nlohmann::json& SpawnJsonData , const nlohmann::json& PrefabJsonData);

	void DeserializePrefabData(const nlohmann::json& PrefabJsonData , GameObject::ComponentDumpData& Dump);

	void DeserializeSpawnData(const nlohmann::json& SpawnJsonData , GameObject::ComponentDumpData& Dump);

	void DeserializeGUID(const nlohmann::json& SpawnJsonData);

	nlohmann::json SerializeSpawnData ();
	nlohmann::json SerializePrefabData();
	
	void DeleteRequestedComponent();
	void DeleteExpiredChildren   ();
	
	// 存在しないコンポーネントのみを追加
	// "ImGui"で作製したときにコポーネントを取り付ける
	void AttachComponent(const std::string& WantAttachComponent);
	void AddComponent(std::shared_ptr<ComponentBase> WantAdd);

	void AddChild(const std::shared_ptr<GameObject> GameObject);

	void RemoveChild(const std::shared_ptr<GameObject>& TargetChild);

	void ResetParent();

	template <class ComponentType>
		requires std::derived_from<ComponentType , ComponentBase>
	std::weak_ptr<ComponentType> GetComponent() const
	{
		const uint32_t id_ = StaticID<ComponentBase>::GetTypeID<ComponentType>();
	
		auto itr_ = m_uniqueComponentMap.find(id_);

		if (itr_ != m_uniqueComponentMap.end())
		{
			if (auto component_ = itr_->second.lock())
			{
				return std::static_pointer_cast<ComponentType>(component_);
			}
		}

		return std::weak_ptr<ComponentType>();
	}

	template <class ComponentType>
		requires std::derived_from<ComponentType, ComponentBase>
	std::vector<std::weak_ptr<ComponentType>> GetMultiComponents() const
	{
		const uint32_t id_ = StaticID<ComponentBase>::GetTypeID<ComponentType>();
		std::vector<std::shared_ptr<ComponentType>> list_ = {};

		auto itr_ = m_multiComponentMap.find(id_);

		if (itr_ != m_multiComponentMap.end())
		{
			list_.emplace_back(itr_->second);	
		}

		return list_;
	}

	// インターフェース継承コンポーネントを取得
	// こうしないと"OCP"を守れる設計じゃないと感じたから
	template <class ComponentType>
		requires std::derived_from<ComponentType , ComponentBase>
	std::vector<std::weak_ptr<ComponentType>> GetBaseComponentList() const
	{
		std::vector<std::weak_ptr<ComponentType>> baseComponentList_;
		const uint32_t id_ = StaticID<ComponentBase>::GetTypeID<ComponentType>();
	
		auto itr_ = m_baseComponentMap.find(id_);

		if(itr_ != m_baseComponentMap.end())
		{
			for(const auto& list_ : itr_->second)
			{
				auto component_ = std::static_pointer_cast<ComponentType>(list_.lock());
				if (!component_) { continue; }

				baseComponentList_.emplace_back(component_);
			}
		}

		return baseComponentList_;
	}

	bool HasParent() const;

	void SwapComponentOrder(size_t SwapIndexA , size_t SwapIndexB);

	const auto& GetComponentList    () const { return m_componentList; }
	
	const auto& GetUniqueComponentMap() const { return m_uniqueComponentMap; }
	const auto& GetMultiComponentMap () const { return m_multiComponentMap;  }

	auto& GetWorkComponentMultiMap() const { return m_multiComponentMap; }

	std::weak_ptr<ComponentObserver> GetComponentObserver() const { return m_componentObserver; }

	const std::weak_ptr<GameObject> GetParent() const { return m_parent; }
	std::weak_ptr<GameObject>       GetParent()       { return m_parent; }

	const std::list<std::weak_ptr<GameObject>>& GetChildList() const { return m_childList; }
	std::list<std::weak_ptr<GameObject>>&       GetChildList()       { return m_childList; }

	std::string_view GetPrefabName() const { return m_prefabName; }

	uint64_t GetCategoryType() const { return m_categoryType; }
	uint64_t GetGUID        () const { return m_guid;		  }

	bool GetIsDeleteRequested() const { return m_deleteRequested; }

	void SetParent(const std::shared_ptr<GameObject>& Set) { m_parent = Set; }
	
	void SetPrefabName(const std::string& Set) { m_prefabName = Set; }

	void SetCategoryType(uint64_t Set) { m_categoryType = Set; }
	// 取扱注意、少しでも"ID"がずれればバグになるから
	void SetGUID(uint64_t Set) { m_guid = Set; }
	
	void SetIsDeleteRequested(bool Set);

private:

	std::shared_ptr<ComponentObserver> m_componentObserver = nullptr;

	std::unordered_map<uint32_t , std::weak_ptr<ComponentBase>>              m_uniqueComponentMap;
	std::unordered_map<uint32_t , std::vector<std::weak_ptr<ComponentBase>>> m_multiComponentMap;
	std::unordered_map<uint32_t , std::vector<std::weak_ptr<ComponentBase>>> m_baseComponentMap;
	
	// コンポーネントの削除頻度がエディターで削除するかどうかを決めるぐらいのため"std::vector"を採用
	// もし削除するような場面がでてきて頻度が頻繁なら"std::list"を推奨
	std::vector<std::shared_ptr<ComponentBase>> m_componentList;

	std::weak_ptr<GameObject>            m_parent;
	std::list<std::weak_ptr<GameObject>> m_childList;

	// "Prefab"で取得した"Json"ファイルをゲームオブジェクトに読み込む際のキーとして使う
	// ビットシフトで実現したかったが詳細なプレハブ情報を扱うならビットシフトの"32"個の上限を大きく超過する
	// だから柔軟性を考慮して"std::string"を使用
	std::string m_prefabName = CommonConstant::STRING_UNKNOWN;

	uint64_t m_categoryType = 0u;
	uint64_t m_guid         = CommonConstant::UNREGISTERD_GUID;

	bool m_deleteRequested = false;
};