#pragma once

class GameObject;
class ComponentBase;

// ゲームオブジェクトに持たせることを想定したコンポーネントの基底クラス
// "ComponentOwnedBase"にはゲームオブジェクトのポインタを持たせずこの"ComponentOwnedBase"にのみ持たせることで
// "ComponentCommonBehaviorBase"に必要なコンポーネントの情報を与える唯一のコンポーネントになるので保守性が高くなる
class ComponentBase
{
public:

	ComponentBase         () = default;
	virtual ~ComponentBase() = default;

	// 複数アタッチしていいコンポーネントかどうか基本的には禁止
	virtual bool IsAllowMultiple() const { return false; }

	// "GetTypeIdentifier"では
	virtual uint32_t GetTypeID() const = 0;

	template <class ComponentType>
		requires std::derived_from<ComponentType , ComponentBase>
	void RegisterBaseTypeIDs()
	{
		m_baseTypeIDs.insert(StaticID<ComponentBase>::GetTypeID<ComponentType>());
	}
	
	// "PostLoadInit"は"Json"で読み込んだデータを読み込んだあとにする
	virtual void Init        () { /* 必要に応じてオーバーライドしてください */ }
	virtual void PostLoadInit() { /* 必要に応じてオーバーライドしてください */ }
	
	virtual void EarlyUpdate() { /* 必要に応じてオーバーライドしてください */ }
	virtual void Update     () { /* 必要に応じてオーバーライドしてください */ }
	virtual void LateUpdate () { /* 必要に応じてオーバーライドしてください */ }

	virtual void OnCollision			(const std::list<KdCollider::CollisionResult>& Result) { /* 必要に応じてオーバーライドしてください */ }
	virtual void OnPostCollision		()													   { /* 必要に応じてオーバーライドしてください */ }
	virtual void OnPostCollisionResolved()													   { /* 必要に応じてオーバーライドしてください */ }
	
	virtual void ImGuiSpawnDataInspector () { /* 必要に応じてオーバーライドしてください */ }
	virtual void ImGuiPrefabDataInspector() { /* 必要に応じてオーバーライドしてください */ }

	virtual void DeserializeSpawnData (const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }
	virtual void DeserializePrefabData(const nlohmann::json& Json) { /* 必要に応じてオーバーライドしてください */ }

	virtual nlohmann::json SerializeSpawnData () { return nlohmann::json(); }
	virtual nlohmann::json SerializePrefabData() { return nlohmann::json(); }
	
	bool IsEnable()
	{
		return m_isEnable;
	}

	void Enable()
	{
		if (m_isEnable) { return; }
		m_isEnable = true;
	}

	void Disable()
	{
		if (!m_isEnable) { return; }
		m_isEnable = false;
	}

	std::string_view GetTypeName() const { return m_typeName; }

	const std::unordered_set<uint32_t>& GetBaseTypeIDs() const { return m_baseTypeIDs; }

	bool GetIsDeleteRequested()const { return m_isDeleteRequested; }

	void SetOwner(std::weak_ptr<GameObject> Set) { m_owner = Set; }

	void SetTypeName(const std::string& TypeName) { m_typeName = TypeName; }

	void SetIsDeleteRequested(bool Set) { m_isDeleteRequested = Set; }

protected:
	
	std::shared_ptr<GameObject> GetOwner()const { return m_owner.lock(); }
	
private:
	
	std::weak_ptr<GameObject> m_owner;
	
	std::string m_typeName = "";

	// コンポーネント一つ一つは基本ユニークだが"MovementComponent"などユニークだが動作としての役割(座標を移動量に応じて加算する)が
	// 被るコンポーネントがあった場合当たり判定でマップタイルと当たったら移動量をゼロにするような処理があった場合に中間基底クラスにダウンキャストして
	// OCPを守るために"std::unordered_set"を使っている、さらに"Init"で必ず初期化するためもしダウンキャストができていなかったら"GameObject"の"AddComponent"呼び出し前の処理で
	// "Init"で初期化できていないかコンポーネントの"Init"で初期化できていないかでデバック範囲が絞れて保守性が高いと考えた
	std::unordered_set<uint32_t> m_baseTypeIDs;
	
	bool m_isEnable          = true;
	bool m_isDeleteRequested = false;
};