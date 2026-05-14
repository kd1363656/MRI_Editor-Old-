#include "GameObject.h"

#include "../Observer/Component/ComponentObserver.h"

#include "../Component/Transform/TransformComponent.h"
#include "../Component/SpringArm/SpringArmComponent.h"
#include "../Component/Render/Animatoin/Model/RenderAnimationModelComponent.h"

#include "../Factory/Comopnent/ComponentFactory.h"

#include "../Scene/SceneManager.h"
#include "../Scene/BaseScene/BaseScene.h"

#include "../Resource/ResourceController.h"
#include "../Resource/Prefab/PrefabController.h"
#include "../Resource/Prefab/Prefab.h"

#include "../Utility/Json/JsonUtility.h"

void GameObject::Init()
{
	// コンポーネントの通知を関するオブザーバーを生成
	if (!m_componentObserver)
	{
		m_componentObserver = std::make_shared<ComponentObserver>();
	}

	for (const auto& component_ : m_componentList)
	{
		if (!component_) { continue; }

		component_->Init();
		component_->SetOwner(shared_from_this());
	}

	// "GUIDManager"を未登録で初期化
	m_guid = CommonConstant::UNREGISTERD_GUID;

	m_deleteRequested = false;

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }
		
		child_->Init();
	}
}
void GameObject::PostLoadInit() const
{
	for (const auto& component_ : m_componentList)
	{
		component_->PostLoadInit();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->PostLoadInit();
	}
}

void GameObject::EarlyUpdate()
{
	// 必要のないコンポーネントを削除
	DeleteRequestedComponent();
	DeleteExpiredChildren   ();

	// 通知の初期化("Moment"通知のみ)
	if (m_componentObserver)
	{
		m_componentObserver->BeginFrame();
	}

	for (const auto& component_ : m_componentList)
	{
		if (!component_)			 { continue; }
		if (!component_->IsEnable()) { continue; }

		component_->EarlyUpdate();
	}

	// アニメーションでオブザーバーへの通知を送信
	if (auto animationComponent_ = GetComponent<RenderAnimationModelComponent>().lock())
	{
		// アニメーションによる通知を発行
		animationComponent_->DispatchAnimationNotify();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->EarlyUpdate();
	}
}
void GameObject::Update() const
{
	for (const auto& component_ : m_componentList)
	{
		if (!component_)		     { continue; }
		if (!component_->IsEnable()) { continue; }

		component_->Update();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->Update();
	}
}
void GameObject::LateUpdate() const
{
	for (const auto& component_ : m_componentList)
	{
		if (!component_) { continue; }
		if (!component_->IsEnable()) { continue; }

		component_->LateUpdate();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->LateUpdate();
	}
}

void GameObject::OnPostCollision()
{
	for (const auto& component_ : m_componentList)
	{
		if (!component_) { continue; }
		component_->OnPostCollision();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->OnPostCollision();
	}
}

void GameObject::OnPostCollisionResolved()
{
	for (const auto& component_ : m_componentList)
	{
		if (!component_) { continue; }
		component_->OnPostCollisionResolved();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->OnPostCollisionResolved();
	}
}

void GameObject::FixMatrix()
{
	// 行列の確定
	if (auto transformComponent_ = GetComponent<TransformComponent>().lock())
	{
		transformComponent_->FixMatrix();
	}

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->FixMatrix();
	}
}

void GameObject::Deserialize(const std::string_view PrefabName, const nlohmann::json& SpawnJsonData, const nlohmann::json& PrefabJsonData)
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto resourceController_ = scene_->GetResourceController().lock();
	if (!resourceController_) { return; }

	auto prefabController_ = resourceController_->GetPrefabController().lock();
	if (!prefabController_) { return; }

	if (PrefabJsonData.is_null() || SpawnJsonData.is_null()) { return; }
	
	// 自身の読み込み
	{
		// ローカル変数でキャッシュリストを作ることで"SpawnDataJson" , "PrefabDataJson" , どちらも存在している場合に
		// 柔軟に生成、データの読み込みを行うことができる
		GameObject::ComponentDumpData dump_ = {};

		m_prefabName   = PrefabName;
		m_categoryType = PrefabJsonData.value("CategoryType", 0u);

		DeserializePrefabData(PrefabJsonData , dump_);
		DeserializeSpawnData (SpawnJsonData  , dump_);

		// "SpawnData"、"PrefabData"のデシリアライズ化が完了したらコンポーネントをこのゲームオブジェクトにアタッチ
		for (const auto& [key_ , value_] : dump_.uniqueComponent)
		{
			AddComponent(value_);
		}

		for (const auto& [key_ , value_] : dump_.multi)
		{
			for (const auto& list_ : value_)
			{
				AddComponent(list_.component);
			}
		}
	}

	// 子オブジェクト(再帰的なコードなので注意して読んでください)
	{
		if (JsonUtility::CheckIsContainAndArray(SpawnJsonData , "Children"))
		{
			for (const auto& childArray_ : SpawnJsonData["Children"])
			{
				const std::string prefabName_ = childArray_.value("PrefabName", CommonConstant::STRING_UNKNOWN);

				// プレハブデータが登録されていれば子オブジェクトとなるゲームオブジェクトを作製しプレハブデータ、スポーンデータを読み込む
				if (auto prefabDataJson_ = prefabController_->GetPrefabData(prefabName_).lock())
				{
					const nlohmann::json prefabData_ = prefabDataJson_->GetJsonData();
					auto childGameObject_            = scene_->CreateGameObject    ();
					
					childGameObject_->Deserialize(prefabName_ , childArray_ , prefabData_);

					// 親子関係を設定(どちらも"weak_ptr")
					childGameObject_->SetParent(shared_from_this());
					m_childList.emplace_back(childGameObject_);

					// "shared_ptr"本体を持つのはシーンのリスト
					scene_->AddGameObject(childGameObject_);
				}
			}
		}
	}
}
void GameObject::DeserializePrefabData(const nlohmann::json& PrefabJsonData, GameObject::ComponentDumpData& Dump)
{
	if (PrefabJsonData.is_null()) { return; }

	const auto& componentFactory_ = ComponentFactory::GetInstance();

	// プレハブデータの読み込み
	if (!JsonUtility::CheckIsContainAndArray(PrefabJsonData, "Components")) { return; }

	// コンポーネントの読み込みと生成
	for (const auto& componentArray_ : PrefabJsonData["Components"])
	{
		// コンポーネントの名前を取得
		const std::string typeName_ = componentArray_.value("TypeName", CommonConstant::STRING_UNKNOWN);

		// 存在したらコンポーネントを生成
		auto component_ = componentFactory_.Create(typeName_);
		if (!component_) { continue; }

		component_->Init();
		component_->SetOwner(shared_from_this());
		// コンポーネント内で"typeid"をつかってクラス名を初期化してもよかったが
		// 処理効率が落ちる、二度手間なのでしなかった
		component_->SetTypeName(typeName_);

		if (componentArray_.contains("PrefabData"))
		{
			component_->DeserializePrefabData(componentArray_["PrefabData"]);
		}

		// 複数存在可能ならマルチコンポーネントリストに格納
		if (component_->IsAllowMultiple())
		{
			GameObject::MultiComponentDump dump_ = {};
			dump_.component       = component_;
			dump_.isLoadSpawnData = false;

			Dump.multi[typeName_].emplace_back(dump_);
		}
		else
		{
			Dump.uniqueComponent.try_emplace(typeName_, component_);
		}
	}

	// コンポーネントオブザーバーの読み込み
	if (m_componentObserver)
	{
		if (PrefabJsonData.contains("ComponentObserver"))
		{
			m_componentObserver->DeserializePrefabData(PrefabJsonData["ComponentObserver"]);
		}
	}
}
void GameObject::DeserializeSpawnData(const nlohmann::json& SpawnJsonData, GameObject::ComponentDumpData& Dump)
{
	const auto& componentFactory_ = ComponentFactory::GetInstance();

	// スポーンデータ読み込み時に"GUID"の割り振りを行う
	DeserializeGUID(SpawnJsonData);

	// スポーンデータの読み込み
	if (!JsonUtility::CheckIsContainAndArray(SpawnJsonData, "Components")) { return; }

	for (const auto& componentData_ : SpawnJsonData["Components"])
	{
		// コンポーネントの名前を取得
		const std::string typeName_    = componentData_.value("TypeName"     , CommonConstant::STRING_UNKNOWN);
		const bool		  isAllowMulti = componentData_.value("IsAllowMulti" , false);

		// ユニークコンポーネントの読み込み
		if (!isAllowMulti)
		{
			auto itr_ = Dump.uniqueComponent.find(typeName_);
			if (itr_ != Dump.uniqueComponent.end())
			{
				if (itr_->second && componentData_.contains("SpawnData"))
				{
					itr_->second->DeserializeSpawnData(componentData_["SpawnData"]);
				}
			}
			else
			{
				// もしプレハブにないコンポーネントだった場合コンポーネントを生成
				auto component_ = componentFactory_.Create(typeName_);

				if (component_)
				{
					component_->Init();
					component_->SetOwner(shared_from_this());
					// コンポーネント内で"typeid"をつかってクラス名を初期化してもよかったが
					// 処理効率が落ちる、二度手間なのでしなかった
					component_->SetTypeName(typeName_);

					if (componentData_.contains("SpawnData"))
					{
						component_->DeserializeSpawnData(componentData_["SpawnData"]);
					}

					Dump.uniqueComponent.try_emplace(typeName_, component_);
				}
			}
		}
		// ユニークコンポーネントが読み込まれなかったらマルチコンポーネントが読み込まれる
		else
		{
			// マルチコンポーネントの読み鋳込み
			{
				auto& multiList_ = Dump.multi[typeName_];
				bool  isLoaded_ = false;
				int	  loadNum_  = 0;

				// もし検索にヒットしたらスポーン情報を読み込む
				for (auto& list_ : multiList_)
				{
					if (componentData_.contains("SpawnData"))
					{
						// "SpawnData"が既に存在していて読み取れたことを伝える
						isLoaded_ = true;
						loadNum_++;

						// すでに読み取っていたら処理を飛ばす
						if (list_.isLoadSpawnData) { continue; }

						// 読み込んだら"for"文を抜ける
						list_.component->DeserializeSpawnData(componentData_["SpawnData"]);
						list_.isLoadSpawnData = true;
						break;
					}
				}

				// もし一回でも"SpawnData"を読み取れていてマルチリストの容量を超えていたら追加
				if (isLoaded_ && loadNum_ >= multiList_.size())
				{
					// もしプレハブにないコンポーネントだった場合コンポーネントを生清
					auto component_ = componentFactory_.Create(typeName_);

					if (component_)
					{
						component_->Init();
						component_->SetOwner(shared_from_this());
						// コンポーネント内で"typeid"をつかってクラス名を初期化してもよかったが
						// 処理効率が落ちる、二度手間なのでしなかった
						component_->SetTypeName(typeName_);

						if (componentData_.contains("SpawnData"))
						{
							component_->DeserializeSpawnData(componentData_["SpawnData"]);
						}

						GameObject::MultiComponentDump dump_ = {};
						dump_.component = component_;
						dump_.isLoadSpawnData = true;

						multiList_.emplace_back(dump_);
					}
				}
			}
		}
	}
}
void GameObject::DeserializeGUID(const nlohmann::json& SpawnJsonData)
{
	if (SpawnJsonData.is_null()) { return; }

	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto guidManager_ = scene_->GetGUIDGameObjectManager().lock();
	if (!guidManager_)
	{
		KdDebugGUI::Instance().AddLog("\nGUIDManager is null please check scene init\n");
		return;
	}
	
	// "SpawnData"に"GUID"があればそれを優先
	if (SpawnJsonData.contains("GUID")           && 
		SpawnJsonData["GUID"].is_number_unsigned())
	{
		const uint64_t jsonGUID_ = SpawnJsonData.value("GUID", CommonConstant::UNREGISTERD_GUID);

		if (jsonGUID_ == CommonConstant::UNREGISTERD_GUID)
		{
			// 無効値("0")が出た場合は"GUID"を発行
			m_guid = guidManager_->GenerateGUID();
		}
		else
		{
			// 正常に登録完了
			m_guid = jsonGUID_;
			guidManager_->RegisterGUID(jsonGUID_);
		}
	}
	else
	{
		// "GUID"未保存だった場合新規発行
		m_guid = guidManager_->GenerateGUID();
		KdDebugGUI::Instance().AddLog("\nGUID missing in spawnData : Assigning a new GUID\n GUID : %ull" , m_guid);
	}
}

nlohmann::json GameObject::SerializeSpawnData()
{
	auto rootJson_ = nlohmann::json();

	// ゲームオブジェクトの名前を保存
	rootJson_["PrefabName"  ] = m_prefabName;
	rootJson_["CategoryType"] = m_categoryType;
	rootJson_["GUID"        ] = m_guid;

	// ユニークコンポーネントとマルチコンポーネントの"json"を保存したコンポーネント
	auto componentArrayJson_ = nlohmann::json::array();

	for (const auto& component_ : m_componentList)
	{
		auto componentJson_ = nlohmann::json();
		auto spawnDataJson_ = nlohmann::json();

		componentJson_["TypeName"    ] = component_->GetTypeName    ();
		componentJson_["IsAllowMulti"] = component_->IsAllowMultiple();

		spawnDataJson_ = component_->SerializeSpawnData();

		// もししっかりシリアライズ出来たら保存
		if (!spawnDataJson_.is_null())
		{
			componentJson_["SpawnData"] = spawnDataJson_;
		}

		componentArrayJson_.emplace_back(componentJson_);

	}

	// 自身を構成するコンポーネントを保存
	rootJson_["Components"] = componentArrayJson_;

	// 子オブジェクトの保存(再帰的なコードなので注意して読んでください)
	{
		auto childArrayJson_ = nlohmann::json::array();
		
		for (const auto& childList_ : m_childList)
		{
			auto child_ = childList_.lock();
			if (!child_) { continue; }

			auto spawnData_ = child_->SerializeSpawnData();
			
			if (!spawnData_.is_null())
			{
				childArrayJson_.emplace_back(child_->SerializeSpawnData());
			}
		}

		rootJson_["Children"] = childArrayJson_;
	}

	return rootJson_;
}
nlohmann::json GameObject::SerializePrefabData()
{
	auto rootJson_ = nlohmann::json();

	// ゲームオブジェクトの名前を保存
	rootJson_["PrefabName"  ] = m_prefabName;
	rootJson_["CategoryType"] = m_categoryType;

	// ユニークコンポーネントとマルチコンポーネントの"json"を保存したコンポーネント
	auto componentArrayJson_ = nlohmann::json::array();

	// "json::array"は保存順序が保証される
	for (const auto& component_ : m_componentList)
	{
		
		auto prefabDataJson_ = nlohmann::json();
		auto componentJson_  = nlohmann::json();

		componentJson_["TypeName"    ] = component_->GetTypeName    ();
		componentJson_["IsAllowMulti"] = component_->IsAllowMultiple();

		prefabDataJson_ = component_->SerializePrefabData();

		if (!prefabDataJson_.is_null())
		{
			componentJson_["PrefabData"] = prefabDataJson_;
		}

		componentArrayJson_.emplace_back(componentJson_);
	}

	rootJson_["Components"] = componentArrayJson_;
	
	// コンポーネントオブザーバーのプレハブ情報を保存
	if (m_componentObserver)
	{
		rootJson_["ComponentObserver"] = m_componentObserver->SerializePrefabData();
	}

	return rootJson_;
}

void GameObject::DeleteRequestedComponent()
{
	// 削除申請のあるコンポーネントを削除
	{
		auto itr_ = m_componentList.begin();

		while (itr_ != m_componentList.end())
		{
			if (*itr_)
			{
				if ((*itr_)->GetIsDeleteRequested())
				{
					itr_ = m_componentList.erase(itr_);
				}
				else
				{
					itr_++;
				}
			}
			else
			{
				itr_++;
			}
		}
	}

	// ウィークポインタでキャッシュしていないポインタを削除
	{
		auto mapItr_ = m_multiComponentMap.begin();

		while(mapItr_ != m_multiComponentMap.end())
		{
			auto itr_ = mapItr_->second.begin();

			while (itr_ != mapItr_->second.end())
			{
				if ((*itr_).expired())
				{
					itr_ = mapItr_->second.erase(itr_);
				}
				else
				{
					itr_++;
				}
			}

			mapItr_++;
		}
	}

	// コンポーネントのインスタンスが消えていたら削除
	{
		auto mapItr_ = m_baseComponentMap.begin();

		while(mapItr_ != m_baseComponentMap.end())
		{
			auto itr_ = mapItr_->second.begin();
			
			while (itr_ != mapItr_->second.end())
			{
				if (itr_->expired())
				{
					itr_ = mapItr_->second.erase(itr_);
				}
				else
				{
					itr_++;
				}
			}

			mapItr_++;
		}
	}
}
void GameObject::DeleteExpiredChildren()
{
	// 無効な"weak_ptr"を削除
	std::erase_if(m_childList, [](const auto& Pair) -> bool
	{
		return Pair.expired();
	});
}

void GameObject::AttachComponent(const std::string& WantAttachComponent)
{
	const auto& componentFactory_ = ComponentFactory::GetInstance();
	
	auto component_ = componentFactory_.Create(WantAttachComponent);
	
	// 登録されていないコンポーネントだったら追加しない
	if (!component_) { return; }

	component_->Init();
	component_->SetOwner(shared_from_this());
	// コンポーネント内で"typeid"をつかってクラス名を初期化してもよかったが
	// 処理効率が落ちる、二度手間なのでしなかった
	component_->SetTypeName(WantAttachComponent);

	AddComponent(component_);
}

void GameObject::AddComponent(std::shared_ptr<ComponentBase> WantAdd)
{
	const uint32_t id_ = WantAdd->GetTypeID();
	
	// 本体を管理するコンポーネントリストに格納
	m_componentList.emplace_back(WantAdd);

	// 同じ中間基底クラスを継承しているコンポーネントの生成を禁止、中間基底クラスはあくまで"OCP"を実現するために作ったものだから
	// 何個もその中間基底クラスを継承したコンポーネントはいらない
	if (!WantAdd->IsAllowMultiple())
	{
		m_uniqueComponentMap.try_emplace(id_, WantAdd);
		KdDebugGUI::Instance().AddLog("Add not allowMultiple component\n");
	}
	else
	{
		m_multiComponentMap[id_].emplace_back(WantAdd);
		KdDebugGUI::Instance().AddLog("Add allowMultiple component\n");
	}

	// コンポーネントの継承クラス以外保持しない、キャストする際のアクセスを減らすキャッシュリスト
	for (const auto& baseTypeID_ : WantAdd->GetBaseTypeIDs())
	{
		if (baseTypeID_ != id_) 
		{
			m_baseComponentMap[baseTypeID_].emplace_back(WantAdd);
		}
	}
}

void GameObject::AddChild(const std::shared_ptr<GameObject> GameObject)
{
	m_childList.emplace_back(GameObject);
}

void GameObject::RemoveChild(const std::shared_ptr<GameObject>& TargetChild)
{
	m_childList.remove_if([&](const std::weak_ptr<GameObject>& WeakChild) 
	{
		if (auto Child_ = WeakChild.lock())
		{
			// ポインタ比較
			return Child_.get() == TargetChild.get();
		}

		return false;
	});
}

void GameObject::ResetParent()
{
	m_parent.reset();
}
bool GameObject::HasParent() const
{
	return !m_parent.expired();
}

void GameObject::SwapComponentOrder(size_t SwapIndexA, size_t SwapIndexB)
{
	if (SwapIndexA < 0llu && SwapIndexA > m_componentList.size()) { return; }
	if (SwapIndexB < 0llu && SwapIndexB > m_componentList.size()) { return; }

	// 指定したら要素の中身を入れ替える
	std::swap(m_componentList[SwapIndexA] , m_componentList[SwapIndexB]);
}

void GameObject::SetIsDeleteRequested(bool Set)
{
	m_deleteRequested = Set;

	for (const auto& childCache_ : m_childList)
	{
		auto child_ = childCache_.lock();
		if (!child_) { continue; }

		child_->SetIsDeleteRequested(Set);
	}
}