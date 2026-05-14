#include "BTNodeCompositeBase.h"

#include "../../../../Utility/ImGui/ImGuiUtility.h"

#include "../../../../../System/Input/RawInputManager.h"

#include "../../../../GUID/BehaviorTree/GUIDBehaviorTreeManager.h"

#include "../Root/BTNodeRoot.h"

void BTNodeCompositeBase::PostLoadInit()
{
	BTNodeBase::PostLoadInit();

	for (const auto& child_ : m_childNodeList)
	{
		if (!child_) { continue; }

		child_->PostLoadInit();
	}
}

void BTNodeCompositeBase::OnDestroy()
{
	auto rootNode_ = GetNodeRootCache().lock();
	if (!rootNode_) { return; }

	BTNodeBase::OnDestroy();

	for (const auto& child_ : m_childNodeList)
	{
		if (!child_) { continue; }

		rootNode_->AddCandidateNodeList(child_);
	}
}

void BTNodeCompositeBase::ImGuiNodeInspector()
{
	auto rootNode_ = GetNodeRootCache().lock();
	if (!rootNode_) { return; }

	const auto& inputManager_ = RawInputManager::GetInstance();

	const uint64_t rootGUID_ = GetGUID();

	ImNodes::PushColorStyle(ImNodesCol_TitleBar , ImGuiUtility::COMPOSITE_NODE_COLOR);
	BTNodeBase::ImGuiNodeInspector();
	ImNodes::PopColorStyle();

	// 子ノードのリンクの描画
	for (const auto& childNode_ : m_childNodeList)
	{
		if (!childNode_) { continue; }

		ImGuiUtility::DrawLink(rootGUID_ , childNode_->GetGUID());
		childNode_->ImGuiNodeInspector();
	}

	// ノードが選択されていて"Delete"キーが押されていたら該当ノードを削除
	if (!m_childNodeList.empty())
	{
		auto itr_ = m_childNodeList.begin();
		while (itr_ != m_childNodeList.end())
		{
			auto childNode_ = *itr_;
		
			if (!childNode_) 
			{
				itr_ = m_childNodeList.erase(itr_);
				continue;
			}
			
			const int selfID_ = static_cast<int>(childNode_->GetGUID());
			
			// もし該当ノードが選択されていなければ実行しない
			if (ImNodes::IsNodeSelected(selfID_))
			{
				ImGui::PushID(selfID_);

				rootNode_->SetSelectedNode(childNode_);

				// もし右クリックされたらコンテキストメニューを表示
				if (inputManager_.IsMouseClickedOnce(VK_RBUTTON))
				{
					ImGui::OpenPopup("NodeContextMenu");
				}

				if (ImGui::BeginPopup("NodeContextMenu"))
				{
					// "ID"の登録を解除してから削除
					if (ImGui::MenuItem("Delete"))
					{
						childNode_->OnDestroy();
						itr_ = m_childNodeList.erase(itr_);
						ImGui::EndPopup();
						ImGui::PopID();
						continue;
					}

					// 候補ノードに自信を格納
					if (ImGui::MenuItem("CancelNodeLink"))
					{
						rootNode_->AddCandidateNodeList(*itr_);
						itr_ = m_childNodeList.erase(itr_);
						ImGui::EndPopup();
						ImGui::PopID();
						continue;
					}

					ImGui::EndPopup();
				}

				ImGui::PopID();
			}
			
			itr_++;
		}
	}
}
void BTNodeCompositeBase::ImGuiAfterEndNodeEditorInspector()
{
	BTNodeBase::ImGuiAfterEndNodeEditorInspector();

	// もしリンクが生成されたノードのポインタを取得出来たら格納、
	// そして上から実行されるようにソート
	auto linked_ = ImGuiUtility::ImGuiReceiveLinkedNode(GetGUID() , GetNodeRootCache());
	if (linked_)
	{
		AddChildNode(linked_);
	}

	// 実行順序が上から実行されるようにソート
	// スクリーン座標をもとにした座標なので"Y"座標が小さければ小さいほど上にある
	std::sort(m_childNodeList.begin() , m_childNodeList.end() , [](const auto& Self , const auto& Pair)
	{
		return (Self->GetNodePos().y < Pair->GetNodePos().y);
	});

	// 子ノードもリンク接続を実行
	for (const auto& childNode_ : m_childNodeList)
	{
		childNode_->ImGuiAfterEndNodeEditorInspector();
	}
}

void BTNodeCompositeBase::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializeSpawnData(Json);

	// "PrefabData"で生成されたノードが順番にロードできる構造体を作製
	// "nlohmann::json::array"で保存する関係上ロードするデータの順序が必ず守られる
	std::vector<CommonStruct::LoadChildNodeSpawnData> loadArray_;
	for (const auto& childNode_ : m_childNodeList)
	{
		if (!childNode_) { continue; }

		CommonStruct::LoadChildNodeSpawnData data_ = {};
		data_.nodeCache = childNode_;
		data_.isLoaded  = false;

		loadArray_.emplace_back(data_);
	}

	// 配列かどうかを確認して一回ロードしたら即座に"break"するといった処理
	if (JsonUtility::CheckIsContainAndArray(Json, "ChildNodeList"))
	{
		for (const auto& jsonArray_ : Json["ChildNodeList"])
		{
			if (!jsonArray_.contains("ChildNode")) { continue; }
			
			for (auto& loadData_ : loadArray_)
			{
				if (loadData_.isLoaded) { continue; }

				auto load_ = loadData_.nodeCache.lock();
				if (!load_) { continue; }

				load_->DeserializeSpawnData(jsonArray_["ChildNode"]);
				loadData_.isLoaded = true;
				break;
			}
		}
	}
}
void BTNodeCompositeBase::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	BTNodeBase::DeserializePrefabData(Json);

	if (JsonUtility::CheckIsContainAndArray(Json , "ChildNodeList"))
	{
		for (const auto& jsonArray_ : Json["ChildNodeList"])
		{
			// ファクトリーから生成したノードが存在したらデシリアライズ
			const std::string nodeName_ = jsonArray_.value("ChildNodeName", CommonConstant::STRING_UNKNOWN);
			auto created_ = BTNodeFactory::GetInstance().Create(nodeName_);
			if (!created_) { continue; }
			
			created_->Init();
			created_->SetBlackBoardCache(GetBlackBoardCache());
			created_->SetNodeRootCache  (GetNodeRootCache  ());

			if (jsonArray_.contains("ChildNode"))
			{
				created_->DeserializePrefabData(jsonArray_["ChildNode"]);
			}

			// リストに追加
			AddChildNode(created_);
		}
	}
}

nlohmann::json BTNodeCompositeBase::SerializeSpawnData()
{
	auto rootJson_ = nlohmann::json();

	// 基底クラスの"Json"
	{
		auto baseJson_ = BTNodeBase::SerializeSpawnData();

		if (!baseJson_.is_null())
		{
			rootJson_.update(baseJson_);
		}
	}

	// 子ノードを保存
	{
		auto arrayJson_ = nlohmann::json::array();

		for (const auto& child_ : m_childNodeList)
		{
			if (!child_) { continue; }

			auto json_ = nlohmann::json();
			json_["ChildNode"] = child_->SerializeSpawnData();

			arrayJson_.emplace_back(json_);
		}

		rootJson_["ChildNodeList"] = arrayJson_;
	}


	return rootJson_;
}
nlohmann::json BTNodeCompositeBase::SerializePrefabData()
{
	auto rootJson_ = nlohmann::json();

	// 基底クラスの"Json"
	{
		auto baseJson_ = BTNodeBase::SerializePrefabData();

		if (!baseJson_.is_null())
		{
			rootJson_.update(baseJson_);
		}
	}

	// 子ノードを保存
	{
		auto arrayJson_ = nlohmann::json::array();

		for (const auto& child_ : m_childNodeList)
		{
			if (!child_) { continue; }

			auto json_ = nlohmann::json();

			json_["ChildNodeName"] = child_->GetNodeName();
			json_["ChildNode"    ] = child_->SerializePrefabData();

			arrayJson_.emplace_back(json_);
		}

		rootJson_["ChildNodeList"] = arrayJson_;
	}

	return rootJson_;
}

void BTNodeCompositeBase::AddChildNode(std::shared_ptr<BTNodeBase> Child)
{
	if (!Child) { return; }
	m_childNodeList.emplace_back(Child);
}