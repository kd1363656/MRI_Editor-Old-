#pragma once

#include "../../Utility/Component/ComponentUtility.h"
#include "../../Utility/ImGui/ImGuiUtility.h"

// 安全性が下がってしまうがヘッダーをインクルードしなくてもいいようにしたければ"required"を使えない
template <class ComponentType>
class CommonCoreGUIDBinder : public CommonCoreBase
{

public:

	CommonCoreGUIDBinder ()          = default;
	~CommonCoreGUIDBinder() override = default;

	void Init() override
	{
		m_nowTargetGUID   = CommonConstant::UNREGISTERD_GUID;
		m_startTargetGUID = CommonConstant::UNREGISTERD_GUID;
	}
	// 別に"Owner"が必要ではないのでもし引数がなければ"nullptr"
	void PostLoadInit(std::shared_ptr<GameObject> Owner = nullptr) override
	{
		// 初回だけ絶対に参照するデータに違いが出ないようにする
		// "Json"ファイルでシーンを保存したとき用の対策
		m_nowTargetGUID = m_startTargetGUID;

		// ゲームオブジェクトリスト全てを捜査して合致するゲームオブジェクトのコンポーネントを取得
		ComponentUtility::BoundGUIDComponent(m_bindComponent , m_nowTargetGUID);
	}

	void ImGuiSpawnDataInspector () override
	{
		ImGuiUtility::BindToGameObjectByGUID("BindTargetComponentGUIDList"      , m_nowTargetGUID   , m_bindComponent);
		ImGuiUtility::BindToGameObjectByGUID("BindStartTargetComponentGUIDList" , m_startTargetGUID);
	}
	// シーンによってゲームオブジェクトに対して付与されている"GUID"は違うが
	// そのシーンで特定のゲームオブジェクトを複製する際に使いたいからプレハブ化している
	void ImGuiPrefabDataInspector() override
	{
		ImGuiUtility::BindToGameObjectByGUID("BindTargetComponentGUIDList"      , m_nowTargetGUID , m_bindComponent);
		ImGuiUtility::BindToGameObjectByGUID("BindStartTargetComponentGUIDList" , m_startTargetGUID);
	}

	void DeserializeSpawnData(const nlohmann::json& Json) override
	{
		if (Json.is_null()) { return; }

		m_nowTargetGUID   = Json.value("TargetGUID"      , CommonConstant::UNREGISTERD_GUID);
		m_startTargetGUID = Json.value("StartTargetGUID" , CommonConstant::UNREGISTERD_GUID);
	}
	void DeserializePrefabData(const nlohmann::json& Json) override
	{
		if (Json.is_null()) { return; }

		m_nowTargetGUID   = Json.value("TargetGUID"      , CommonConstant::UNREGISTERD_GUID);
		m_startTargetGUID = Json.value("StartTargetGUID" , CommonConstant::UNREGISTERD_GUID);
	}

	nlohmann::json SerializeSpawnData () override
	{
		auto json_ = nlohmann::json();

		json_["TargetGUID"     ] = m_nowTargetGUID;
		json_["StartTargetGUID"] = m_startTargetGUID;

		return json_;
	}
	nlohmann::json SerializePrefabData() override
	{
		auto json_ = nlohmann::json();

		json_["TargetGUID"     ] = m_nowTargetGUID;
		json_["StartTargetGUID"] = m_startTargetGUID;

		return json_;
	}
	
	void ChangeBindComponent(const uint64_t GUID)
	{
		m_nowTargetGUID = GUID;
	}

	std::weak_ptr<ComponentType>  GetBindComponentCache   () const { return m_bindComponent; }
	std::weak_ptr<ComponentType>& GetRefBindComponentCache()       { return m_bindComponent; }

	void SetStartTargetGUID(uint64_t Set) { m_startTargetGUID = Set; }

private:

	std::weak_ptr<ComponentType> m_bindComponent;

	uint64_t m_nowTargetGUID   = CommonConstant::UNREGISTERD_GUID;
	uint64_t m_startTargetGUID = CommonConstant::UNREGISTERD_GUID;
};