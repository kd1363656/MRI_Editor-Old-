#include "BTNodeBase.h"

#include "../../../Utility/ImGui/ImGuiUtility.h"

#include "../../../GameObject/GameObject.h"

void BTNodeBase::Init()
{
	m_nodeName = CommonConstant::STRING_UNKNOWN;

	m_nodeState = BTNodeBase::NodeState::Running;

	m_guid = CommonConstant::UNREGISTERD_GUID;

	m_nodePos = {};
}
void BTNodeBase::PostLoadInit()
{
	const int selfID_ = static_cast<int>(m_guid);

	// ノード位置を格納
	ImNodes::SetNodeEditorSpacePos(selfID_, m_nodePos);
}

void BTNodeBase::OnDestroy()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto guidManager_ = scene_->GetGUIDBehaviorTreeManager().lock();
	if (!guidManager_) { return; }

	guidManager_->UnRegisterGUID(m_guid);
}

void BTNodeBase::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	DeserializeGUID(Json);
}
void BTNodeBase::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_nodeName = Json.value("NodeName", CommonConstant::STRING_UNKNOWN);
	
	if (Json.contains("NodePos"))
	{
		m_nodePos = JsonUtility::JsonToImVec2(Json["NodePos"]);
	}
}
void BTNodeBase::DeserializeGUID(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto guidManager_ = scene_->GetGUIDBehaviorTreeManager().lock();
	if (!guidManager_)
	{
		KdDebugGUI::Instance().AddLog("\nGUIDManager is null please check scene init\n");
		return;
	}

	// "SpawnData"に"GUID"があればそれを優先
	if (Json.contains("GUID") &&
		Json["GUID"].is_number_unsigned())
	{
		const uint64_t jsonGUID_ = Json.value("GUID", CommonConstant::UNREGISTERD_GUID);

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
		KdDebugGUI::Instance().AddLog("\nGUID missing in spawnData : Assigning a new GUID\n GUID : %ull", m_guid);
	}
}

nlohmann::json BTNodeBase::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	json_["GUID"] = m_guid;

	return json_;
}
nlohmann::json BTNodeBase::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["NodeName"] = m_nodeName;
	json_["NodePos" ] = JsonUtility::ImVec2ToJson(m_nodePos);

	return json_;
}

void BTNodeBase::ImGuiNodeInspector()
{
	const int selfID_ = static_cast<int>(m_guid);

	ImNodes::BeginNode(selfID_);
	{
		ImGuiUtility::DrawNodeTitleBar(m_nodeName);
		ImGui::Text("GUID  : %d" , selfID_);

		// ノード状態の描画
		for (const auto& state_ : ImGuiUtility::NODE_STATE_LIST)
		{
			if (m_nodeState != state_.type) { continue; }
			ImGui::Text("State : %s", state_.label);
		}

		ImGuiUtility::DrawNodeInputAttribute (m_guid);
		ImGuiUtility::DrawNodeOutputAttribute(m_guid);
	}
	ImNodes::EndNode();
}
void BTNodeBase::ImGuiAfterEndNodeEditorInspector()
{
	const int selfID_ = static_cast<int>(m_guid);

	// ノード座標を毎フレーム格納
	m_nodePos = ImNodes::GetNodeEditorSpacePos(selfID_);
}