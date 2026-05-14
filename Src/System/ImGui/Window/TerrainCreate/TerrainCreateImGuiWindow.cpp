#include "TerrainCreateImGuiWindow.h"

#include "../../../../Application/Factory/Comopnent/ComponentFactory.h"
#include "../../../../Application/Component/Camera/CameraComponent.h"
#include "../../../../Application/Component/Transform/TransformComponent.h"
#include "../../../../Application/Component/Render/Model/RenderModelComponentBase.h"
#include "../../../../Application/CommonCore/Render/Model/CommonCoreRenderModel.h"

#include "../../../../Application/GameObject/GameObject.h"

#include "../../../../Application/Scene/SceneManager.h"
#include "../../../../Application/Scene/BaseScene/BaseScene.h"

#include "../../../../Application/Utility/ImGui/ImGuiUtility.h"
#include "../../../../Application/Utility/Common/CommonEnum.h"
#include "../../../../Application/Utility/BitShift/BitShiftUtility.h"

#include "../../ImGuiManager.h"
#include "../../Config/PrefabRegistrationDataList/PrefabRegistrationDataListImGuiConfig.h"

void TerrainCreateImGuiWindow::Update()
{
	if(ImGui::Begin("TerrainCreate"))
	{
		UpdateAddGameObjectList                    ();
		UpdateAddGameObjectNum                     ();
		UpdateAddGameObjectCreateStartPosition     ();
		UpdateAddGameObjectGloballyUniqueIdentifier();
		UpdateAddGameObjectButton                  ();
	}
	ImGui::End();
}

void TerrainCreateImGuiWindow::UpdateAddGameObjectList()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto prefabRegistrationDataListImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();
	if (!prefabRegistrationDataListImGuiConfig_) { return; }

	ImGui::PushID(&prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList());

	if (ImGui::BeginCombo("##Add TerrainType", m_addGameObjectPrefabName_.c_str()))
	{
		for (const auto& [key_, value_] : prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList())
		{
			if (!BitShiftUtility::IsStandFlag(CommonEnum::GameObjectCategoryType::Terrain , value_.categoryType)) { continue; }

			bool isSelected_ = m_addGameObjectPrefabName_ == value_.prefabName;

			// ゲームオブジェクトのアタッチしたいプレハブの名前を与える
			if (ImGui::RadioButton(value_.prefabName.c_str(), isSelected_))
			{
				m_addGameObjectPrefabName_ = value_.prefabName;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PopID();

}
void TerrainCreateImGuiWindow::UpdateAddGameObjectNum()
{
	ImGui::SameLine();

	ImGui::DragInt  ("CreateNumber"        , &m_createNumber        , 1   );
	ImGui::DragFloat("CreateTerrainRadius" , &m_createTerrainRadius , 0.1f);
}
void TerrainCreateImGuiWindow::UpdateAddGameObjectCreateStartPosition()
{
	ImGui::DragFloat3("CreateStartPostion", &m_createStartPosition.x, 0.1f);

	ImGuiUtility::DirectionComboSelector("CreateDirection", m_createDirection);
}
void TerrainCreateImGuiWindow::UpdateAddGameObjectGloballyUniqueIdentifier()
{
	ImGuiUtility::BindToGameObjectByGUID("BindGameObject" , m_boundGameObjectGUID);
}
void TerrainCreateImGuiWindow::UpdateAddGameObjectButton()
{
	auto scene_ = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene_) { return; }

	auto prefabRegistrationDataListImGuiConfig_ = ImGuiManager::GetInstance().GetImGuiConfig<PrefabRegistrationDataListImGuiConfig>().lock();
	if (!prefabRegistrationDataListImGuiConfig_) { return; }

	if (ImGui::Button("Add Terrain"))
	{
		// 生成数が決まっていないなら止める
		if (m_createNumber == 0) { return; }

		for (int i = 0; i < m_createNumber; ++i)
		{
			auto gameObject_ = scene_->CreateGameObject();
			gameObject_->Init();

			if (!gameObject_) { return; }

			// もし登録リストに追加するゲームオブジェクトの名前が登録されていたらプレハブデータを追加する
			for (const auto& [key_, value_] : prefabRegistrationDataListImGuiConfig_->GetPrefabRegistrationDataList())
			{
				// 追加すれゲームオブジェクトの名前が一致しなければ"continue"
				if (m_addGameObjectPrefabName_ != value_.prefabName) { continue; }

				// カテゴリータイプとプレハブから"Json"ファイルを読み込む際のパラメータを渡す
				gameObject_->SetPrefabName(value_.prefabName);
				gameObject_->SetCategoryType(value_.categoryType);

				ImGuiUtility::LoadPrefabData(m_addGameObjectPrefabName_, gameObject_);
			}

			 const Math::Vector3 position_ = m_createStartPosition + (m_createDirection * m_createTerrainRadius) * i;

			 // ローカル行列で位置を設定
			 if (auto transformComponent_ = gameObject_->GetComponent<TransformComponent>().lock())
			 {
				 transformComponent_->SetPos(position_);
			 }

			 /// 描画コンポーネントにカメラの情報を与える(カリング処理のため)
			 for(const auto& list_ : scene_->GetGameObjectList())
			 {
				 if (m_boundGameObjectGUID != list_->GetGUID()) { continue; }

				 // カメラがなければ追加をやめる
				 auto subjectCamera_ = list_->GetComponent<CameraComponent>().lock();
				 if (!subjectCamera_) { return; }

				 // 対象となるゲームオブジェクトからカメラコンポーネントと紐づける
				 // "GUID"を結び付ける
				 for (const auto& componentList_ : gameObject_->GetBaseComponentList<RenderModelComponentBase>())
				 {
					 auto component_ = componentList_.lock();
					 if (!component_) { continue; }

					 auto renderModel_ = component_->GetCommonCoreRenderModelCache().lock();
					 if (!renderModel_) { continue; }

					 auto boundCamera_ = renderModel_->GetBoundCameraComponent().lock();
					 if (!boundCamera_) { continue; }

					 auto camera_ = boundCamera_->GetBindComponentCache().lock();
					 if (!camera_) { continue; }
					
					 // すぐに反映
					 boundCamera_->SetStartTargetGUID(m_boundGameObjectGUID);
					 boundCamera_->PostLoadInit(list_);
				 }
			 }

			 scene_->AddGameObject(gameObject_);
		}
	}
}