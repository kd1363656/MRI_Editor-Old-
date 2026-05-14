#include "BaseScene.h"

#include "../../GameObject/GameObject.h"

#include "../../Component/Camera/CameraComponent.h"
#include "../../Component/Render/Model/RenderModelComponentBase.h"
#include "../../Component/Physics/DebugWire/PhysicsDebugWireComponent.h"

#include "../../Resource/ResourceController.h"

#include "../../Utility/BitShift/BitShiftUtility.h"

#include "../../FileSaveAndLoad/GameObject/GameObjectFileSaveAndLoad.h"
#include "../../FileSaveAndLoad/Prefab/PrefabFileSaveAndLoad.h"

#include "../../GUID/BehaviorTree/GUIDBehaviorTreeManager.h"

#include "../../../System/ImGui/ImGuiManager.h"
#include "../../../System/ImGui/Config/SceneView/SceneViewImGuiConfig.h"

// ※子ゲームオブジェクトは"GetComponent"で実行する処理以外はゲームオブジェクト内で行う

void BaseScene::EarlyUpdate()
{
	// 削除申請があったゲームオブジェクトの"GUID"を消去して"Log"で伝える
	if (m_guidGameObjectManager)
	{
		for (const auto& gameObject_ : m_gameObjectList)
		{
			const uint64_t id_ = gameObject_->GetGUID();
			if (!gameObject_->GetIsDeleteRequested()) { continue; }
			
			m_guidGameObjectManager->UnRegisterGUID(id_);
			KdDebugGUI::Instance().AddLog("Erase requested GUID : %d\n" , gameObject_->GetGUID());
		}
	}

	// 削除依頼のあるゲームオブジェクトを削除
	std::erase_if(m_gameObjectList, [](const auto& Pair) -> bool
	{
		return Pair->GetIsDeleteRequested();
	});

	for (const auto& gameObject_ : m_gameObjectList)
	{
		if (gameObject_->HasParent()) { continue; }
		gameObject_->EarlyUpdate();
	}
}
void BaseScene::Update()
{
	for (const auto& gameObject_ : m_gameObjectList)
	{
		if (gameObject_->HasParent()) { continue; }
		gameObject_->Update();
	}

	// エフェクシアの更新
	KdEffekseerManager::GetInstance().Update();
}
void BaseScene::LateUpdate() const
{
	// 当たり判定などを行う
	for (const auto& gameObject_ : m_gameObjectList)
	{
		if (gameObject_->HasParent()) { continue; }
		gameObject_->LateUpdate();
	}

	// 当たり判定の後に行う
	for (const auto& gameObject_ : m_gameObjectList)
	{
		if (gameObject_->HasParent()) { continue; }
		gameObject_->OnPostCollision();
	}

	// 当たり判定の後の正確な座標を取得してからおかなう処理
	for (const auto& gameObject_ : m_gameObjectList)
	{
		if (gameObject_->HasParent()) { continue; }
		gameObject_->OnPostCollisionResolved();
	}

	// 行列を確定する
	for (const auto& gameObject_ : m_gameObjectList)
	{
		if (gameObject_->HasParent()) { continue; }
		gameObject_->FixMatrix();
	}
}

// カメラを描画するために必要
void BaseScene::PreDraw()
{
	for (const auto& gameObject_ : m_gameObjectList)
	{
		// コンポーネントの取得は子ゲームオブジェクトだったとしても実行
		if (auto cameraComponent_ = gameObject_->GetComponent<CameraComponent>().lock())
		{
			cameraComponent_->PreDraw();
		}

		if (gameObject_->HasParent()) { continue; }

		//	// エフェクトの描画
		//	auto camera_ = cameraComponent_->GetCamera().lock();
		//	if (!camera_) { continue; }
		//	
		//	// エフェクシアをカメラをセットして描画
		//	auto& effekseerManager_ = KdEffekseerManager::GetInstance();
		//	effekseerManager_.SetCamera(camera_);
		//}
	}
}

void BaseScene::Draw()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
		DrawModel(CommonEnum::StandardShaderTypeFlags::GenerateDepthMapFromLight);
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(背景など)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		DrawModel(CommonEnum::StandardShaderTypeFlags::UnLit);	
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{
		DrawModel(CommonEnum::StandardShaderTypeFlags::Lit);
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(エフェクトなど)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		DrawModel(CommonEnum::StandardShaderTypeFlags::Effect);
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		DrawModel(CommonEnum::StandardShaderTypeFlags::Bright);
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();

	KdEffekseerManager::GetInstance().Draw();
}

void BaseScene::DrawSprite()
{
	DrawModel(CommonEnum::StandardShaderTypeFlags::Sprite);
}

void BaseScene::DrawDebug()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// デバッグ情報の描画はこの間で行う
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (const auto& obj : m_gameObjectList)
		{
			if (auto debugWire_ = obj->GetComponent<PhysicsDebugWireComponent>().lock())
			{
				if (!debugWire_->IsEnable()) { continue; }
				debugWire_->DrawDebugWire();
			}
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();
}

void BaseScene::DrawModel(CommonEnum::StandardShaderTypeFlags Flag)
{
	for (const auto& obj_ : m_gameObjectList)
	{
		for (auto list_ : obj_->GetBaseComponentList<RenderModelComponentBase>())
		{
			auto render_ = list_.lock();

			// 視錐台の中に入っていないかヌルポインタなら実行しない
			if (!render_) { continue; }
			render_->Draw(Flag);
		}
	}
}

void BaseScene::AddGameObject(const std::shared_ptr<GameObject> AddGameObject)
{
	if (!AddGameObject) { return; }

	// それぞれ適したオブジェクトリストに格納
	if (BitShiftUtility::IsStandFlag(CommonEnum::GameObjectCategoryType::UI, AddGameObject->GetCategoryType()))
	{
		m_uiGameObjectList.emplace_back(AddGameObject);
	}
	else
	{
		m_gameObjectList.emplace_back(AddGameObject);
	}
}

std::shared_ptr<GameObject> BaseScene::CreateGameObject() const
{
	return std::make_shared<GameObject>();
}

void BaseScene::Init()
{
	// ゲームオブジェクトの"GloballyUniqueIdentifier"
	if(!m_guidGameObjectManager)
	{
		m_guidGameObjectManager = std::make_shared<GUIDGameObjectManager>();
	}

	if (!m_guidBehaviorTreeManager)
	{
		m_guidBehaviorTreeManager = std::make_shared<GUIDBehaviorTreeManager>();
	}

	// ファイルパスをセットしてロード
	m_guidGameObjectManager->SetSaveAndLoadFilePath(GetGameObjectGUIDSaveAndLoadFilePath().data());
	m_guidGameObjectManager->LoadRegistry();

	m_guidBehaviorTreeManager->SetSaveAndLoadFilePath(GetBehaviorTreeGUIDSaveAndLoadFilePath().data());
	m_guidBehaviorTreeManager->LoadRegistry();

	// リソース管理クラスの生成・初期化
	if (!m_resourceController)
	{
		m_resourceController = std::make_shared<ResourceController>();
	}

	m_resourceController->Init();

	// ゲームオブジェクトのプレハブ情報、出現情報を読み込む
	if (!m_prefabFileSaveAndLoad)
	{
		m_prefabFileSaveAndLoad = std::make_shared<PrefabFileSaveAndLoad>();
	}

	m_prefabFileSaveAndLoad->Init();
	m_prefabFileSaveAndLoad->SetSaveAndLoadFilePath(GetPrefabSaveAndLoadListFilePath().data());
	m_prefabFileSaveAndLoad->LoadFile();

	if(!m_gameObjectFileSaveAndLoad)
	{
		m_gameObjectFileSaveAndLoad = std::make_shared<GameObjectFileSaveAndLoad>();
	}

	m_gameObjectFileSaveAndLoad->Init();
	m_gameObjectFileSaveAndLoad->SetSaveAndLoadFilePath(GetGameObjectSaveAndLoadFilePath().data());
	m_gameObjectFileSaveAndLoad->LoadFile();

	// すべての必要なコンポーネントを読み込み終わったらコンポーネント同士の連携を行なったり
	// 必要なリソースのファイルパスをもとに初期化を行う
	for (const auto& gameObject_ : m_gameObjectList)
	{
		gameObject_->PostLoadInit();
	}

	// すべてロードし終わってからフライウェイトで管理していたオブジェクトを削除
	// そうすることで前回のシーンで使用したリソースのインスタンスを渡すことができるのでロード時間を短縮できる
	if(m_resourceController)
	{
		m_resourceController->ClearCachedAssets();
	}
}