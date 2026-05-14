#include "RenderStaticModelComponent.h"
#include "../../../Transform/TransformComponent.h"

#include "../../../../CommonCore/Render/CommonCoreRender.h"
#include "../../../../CommonCore/Render/Model/CommonCoreRenderModel.h"
#include "../../../../CommonCore/AssetFilePath/CommonCoreAssetFilePath.h"

#include "../../../../Utility/BitShift/BitShiftUtility.h"

void RenderStaticModelComponent::Init()
{
	RenderModelComponentBase::Init();

	if (!m_modelData)
	{
		m_modelData = std::make_shared<KdModelData>();
	}
}
void RenderStaticModelComponent::PostLoadInit()
{
	RenderModelComponentBase::PostLoadInit();
}

void RenderStaticModelComponent::Draw(const CommonEnum::StandardShaderTypeFlags Flag)
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();
	if (!commonCoreRenderModelCache_													              ) { return; }
	if (!BitShiftUtility::IsStandFlag(Flag, commonCoreRenderModelCache_->GetStandardShaderTypeFlags())) { return; }

	auto commonCoreRenderCache_ = commonCoreRenderModelCache_->GetCommonCoreRenderCache().lock();
	if (!commonCoreRenderCache_) { return; }

	auto selfTransformComponentCache_ = commonCoreRenderCache_->GetSelfTransformComponentCache().lock();
	if (!selfTransformComponentCache_) { return; }

	if (!IsInFrustum()) { return; }

	if (m_modelData)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel
		(
			*m_modelData                              , 
			selfTransformComponentCache_->GetMatrix() , 
			commonCoreRenderCache_->GetColor       ()
		);
	}
}

void RenderStaticModelComponent::ImGuiSpawnDataInspector()
{
	RenderModelComponentBase::ImGuiSpawnDataInspector();
}
void RenderStaticModelComponent::ImGuiPrefabDataInspector()
{
	RenderModelComponentBase::ImGuiPrefabDataInspector();

	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();

	auto commonCoreRenderCache = commonCoreRenderModelCache_->GetCommonCoreRenderCache().lock();
	if (!commonCoreRenderCache) { return; }

	// パスが変更されたらモデルを上書き
	if (auto commonCoreAssetFilePath = commonCoreRenderCache->GetCommonCoreAssetFilePathCache().lock())
	{
		if (commonCoreAssetFilePath->GetHasPathChanged())
		{
			if (!m_modelData)
			{
				m_modelData = std::make_shared<KdModelData>();
			}

			m_modelData = KdAssets::Instance().m_modeldatas.GetData(commonCoreAssetFilePath->GetAssetFilePath());
		}
	}
}

void RenderStaticModelComponent::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	RenderModelComponentBase::DeserializeSpawnData(Json);
}
void RenderStaticModelComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	RenderModelComponentBase::DeserializePrefabData(Json);

	LoadModel();
}

nlohmann::json RenderStaticModelComponent::SerializeSpawnData()
{
	auto json_ = nlohmann::json();

	auto baseJson_ = RenderModelComponentBase::SerializeSpawnData();
	if (!baseJson_.is_null()) 
	{
		json_.update(baseJson_);
	}

	return json_;
}
nlohmann::json RenderStaticModelComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	auto baseJson_ = RenderModelComponentBase::SerializePrefabData();
	if (!baseJson_.is_null())
	{
		json_.update(baseJson_);
	}

	return json_;
}

bool RenderStaticModelComponent::IsInFrustum() const
{
	return true;
}

Math::Matrix RenderStaticModelComponent::GetBoneWorldMatrix(const CommonEnum::BoneType BoneType)
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();
	if (!commonCoreRenderModelCache_) { return Math::Matrix::Identity; }

	if (!m_modelData) { return Math::Matrix::Identity; }

	const std::string boneName_ = commonCoreRenderModelCache_->GetBoneName(BoneType);

	auto node_ = m_modelData->FindNode(boneName_);
	if (!node_) { return Math::Matrix::Identity; }

	return node_->m_worldTransform;
}

void RenderStaticModelComponent::LoadModel()
{
	auto commonCoreRenderModelCache_ = GetCommonCoreRenderModelCache().lock();
	if (!commonCoreRenderModelCache_) { return; }

	auto commonCoreRenderCache = commonCoreRenderModelCache_->GetCommonCoreRenderCache().lock();
	if (!commonCoreRenderCache) { return; }

	// "json"ファイルが読み込まれた時点でモデルを読み込む(当たり判定などでモデル情報が必要なため)
	if (m_modelData)
	{
		if (auto commonCoreAssetFilePath = commonCoreRenderCache->GetCommonCoreAssetFilePathCache().lock())
		{
			m_modelData = KdAssets::Instance().m_modeldatas.GetData(commonCoreAssetFilePath->GetAssetFilePath());
		}
	}
}