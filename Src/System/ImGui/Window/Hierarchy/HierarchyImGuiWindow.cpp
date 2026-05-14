#include "HierarchyImGuiWindow.h"

#include "../../../../Application/Component/Transform/TransformComponent.h"

#include "../../../../Application/Factory/Comopnent/ComponentFactory.h"

#include "../../../../Application/GameObject/GameObject.h"

#include "../../../../Application/Scene/SceneManager.h"
#include "../../../../Application/Scene/BaseScene/BaseScene.h"

#include "../../../../Application/Utility/ImGui/ImGuiUtility.h"

#include "../../ImGuiManager.h"
#include "../../Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"
#include "../BehaviorTree/BehaviorTreeImGuiWindow.h"

void HierarchyImGuiWindow::Update()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	m_hasDroppedBlank = true;

	// どうゆうオブジェクトが生成されたかを
	if (ImGui::Begin("Hierarchy"))
	{
		AddGameObjectButton   ();
		GameObjectShowSelector();
	}
	ImGui::End();
}

void HierarchyImGuiWindow::AddGameObjectButton()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto prefabRegistrationDataListImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();
	if (!prefabRegistrationDataListImGuiConfig_) { return; }

	ImGui::PushID(&prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList());

	if (ImGui::BeginCombo("##Add GameObjectType" , m_addGameObjectPrefabName_.c_str()))
	{
		for (const auto& [key_ , value_] : prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList())
		{
			bool isSelected_ =  m_addGameObjectPrefabName_ == value_.prefabName;

			// ゲームオブジェクトのアタッチしたいプレハブの名前を与える
			if (ImGui::RadioButton(value_.prefabName.c_str(), isSelected_))
			{
				m_addGameObjectPrefabName_ = value_.prefabName;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::Button("Add GameObject"))
	{
		// もししっかりとしたゲームオブジェクトの名前が選ばれていなければゲームオブジェクトを生成しない
		if (m_addGameObjectPrefabName_.empty())
		{
			KdDebugGUI::Instance().AddLog("In hierarchyImGuiWindow : Please select add gameObject name\n");
			return;
		}

		auto gameObject_ = scene_->CreateGameObject();
		gameObject_->Init();

		if (!gameObject_) { return; }

		// もし登録リストに追加するゲームオブジェクトの名前が登録されていたらプレハブデータを追加する
		for (const auto& [key_, value_] : prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList())
		{
			// 追加すれゲームオブジェクトの名前が一致しなければ"continue"
			if (m_addGameObjectPrefabName_ != value_.prefabName) { continue; }

			// "GloballyUniqueIdentifier"の割り当て
			auto guidManager_ = scene_->GetGUIDGameObjectManager().lock();
			if (guidManager_)
			{
				const uint64_t id_ = guidManager_->GenerateGUID();
				gameObject_->SetGUID(id_);

				KdDebugGUI::Instance().AddLog("In hierarchyImGuiWindow : GeneratedID : %llu\n" , id_);
			}

			// カテゴリータイプとプレハブから"Json"ファイルを読み込む際のパラメータを渡す
			gameObject_->SetPrefabName  (value_.prefabName);
			gameObject_->SetCategoryType(value_.categoryType);

			ImGuiUtility::LoadPrefabData(m_addGameObjectPrefabName_ , gameObject_);
		}

		scene_->AddGameObject(gameObject_);

		// 追加したゲームオブジェクトを自動でインスペクター対象にする
		m_inspectorWindowTargetGameObject = gameObject_;
	}
}
void HierarchyImGuiWindow::GameObjectShowSelector()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	for (const auto& gameObject_ : scene_->GetGameObjectList())
	{
		// 親がいるなら親でゲームオブジェクトを操作するため表示しない
		if (gameObject_->HasParent()) { continue; }
		LinkGameObjectFamilyRelationShip(gameObject_);
	}

	CancellationFamilyRelationShip();

	// フレーム毎にドラッグしていたゲームオブジェクトの情報をリセット
	m_currentDraggingGameObject.reset();
}

void HierarchyImGuiWindow::LinkGameObjectFamilyRelationShip(std::weak_ptr<GameObject> Object)
{
	auto gameObject_ = Object.lock();
	if (!gameObject_) { return; }

	ImGui::PushID(gameObject_.get());
	ImGui::Separator();

	// カラムを3列に設定(名前とボタン)
	ImGui::Columns(5 , nullptr , false); // "false"は境界ドラッグ無効

	const std::string prefabName_ = gameObject_->GetPrefabName().data();
	bool              isSelected_ = gameObject_ == m_inspectorWindowTargetGameObject.lock();
	// ノードがしっかり開閉されているかを確認するためのフラグ、矢印でしか開閉を選択できず、ゲームオブジェクトが選択されていたらハイライトするように設定
	bool nodeOpen_ = ImGui::TreeNodeEx(prefabName_.c_str() , ImGuiTreeNodeFlags_OpenOnArrow | (isSelected_ ? ImGuiTreeNodeFlags_Selected : 0));
	
	// インスペクター選択
	if (ImGui::IsItemClicked())
	{
		m_inspectorWindowTargetGameObject = gameObject_;
	}

	ImGui::NextColumn();

	// ドラッグ処理の対象とするゲームオブジェクトをツリーノードに対してセット
	if (ImGui::BeginDragDropSource())
	{
		// 今ドラッグ中のゲームオブジェクトの情報を保持
		m_currentDraggingGameObject = gameObject_;

		ImGui::SetDragDropPayload("GameObjectWeakPointer", &gameObject_, sizeof(std::shared_ptr<GameObject>));
		ImGui::Text("Child %s", gameObject_->GetPrefabName().data());
		ImGui::EndDragDropSource();
	}

	// ドロップするゲームオブジェクトを決める
	if (ImGui::BeginDragDropTarget())
	{
		// 受け入れるドラッグのオブジェクトタイプを渡す
		if (const ImGuiPayload* payLoad_ = ImGui::AcceptDragDropPayload("GameObjectWeakPointer"))
		{
			auto droppedGameObject_ = *(const std::shared_ptr<GameObject>*)(payLoad_->Data);

			// 自分自身や上位の親を子オブジェクトとして設定しないようにする(無限ループ対策)
			if (droppedGameObject_ != gameObject_ && !IsDescendant(gameObject_, droppedGameObject_))
			{
				// すでに親がいたら、前の親から外す
				if (droppedGameObject_->HasParent())
				{
					if (auto oldParent_ = droppedGameObject_->GetParent().lock())
					{
						oldParent_->RemoveChild(droppedGameObject_);
					}
					droppedGameObject_->ResetParent();
				}

				// 一つの子オブジェクトは一つの親に依存する
				
				droppedGameObject_->SetParent(gameObject_);
				gameObject_->AddChild(droppedGameObject_);

				// "Transform"コンポーネントがあれば親を設定する
				if (auto transform3DComponent = droppedGameObject_->GetComponent<TransformComponent>().lock())
				{
					transform3DComponent->PostLoadInit();
				}

				m_hasDroppedBlank = false; // ゲームオブジェクトの上でドロップ
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::NextColumn();

	bool deleteRequested_ = false;
	// 安全に"Column"処理を追えるために"Column"方式
	if (ImGui::SmallButton("Delete"))
	{
		deleteRequested_ = true;
	}

	ImGui::Columns(1); //カラム終了
	ImGui::PopID();

	// ボタンが押されたならゲームオブジェクトを削除
	if (deleteRequested_)
	{
		gameObject_->SetIsDeleteRequested(true);
	}

	// 子ゲームオブジェクトのノードの描画
	if (nodeOpen_)
	{
		for (const auto& weakChild_ : gameObject_->GetChildList())
		{
			if (auto child_ = weakChild_.lock())
			{
				LinkGameObjectFamilyRelationShip(child_);
			}
		}
		ImGui::TreePop();
	}
}
void HierarchyImGuiWindow::CancellationFamilyRelationShip()
{
	auto gameObject_ = m_currentDraggingGameObject.lock();
	if (!m_hasDroppedBlank || !gameObject_) { return; }

	// すでに親を持っていないなら"return"
	if (!gameObject_->HasParent()) { return; }

	if (auto parentGameObject_ = gameObject_->GetParent().lock())
	{
		parentGameObject_->RemoveChild(gameObject_);
	}

	gameObject_->ResetParent();

	if (auto transformComponent_ = gameObject_->GetComponent<TransformComponent>().lock())
	{
		transformComponent_->PostLoadInit();
	}
}

bool HierarchyImGuiWindow::IsDescendant(const std::weak_ptr<GameObject> Parent, const std::weak_ptr<GameObject> PotentialChild)
{
	auto parent_         = Parent.lock        ();
	auto potentialChild_ = PotentialChild.lock();

	if (!parent_ || !potentialChild_) { return true; }

	// 子オブジェクトの中を調べて自分自身の子オブジェクトかどうかを確認
	for (const auto& weakChild_ : parent_->GetChildList())
	{
		if (auto child_ = weakChild_.lock())
		{
			if (child_ == potentialChild_) 
			{ 
				return true; 
			}
			if (IsDescendant(child_, potentialChild_))
			{ 
				return true; 
			}
		}
	}

	return false;
}