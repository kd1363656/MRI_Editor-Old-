#include "RenderModelComponentBase.h"

#include "../../../CommonCore/Render/Model/CommonCoreRenderModel.h"

#include "../../../GameObject/GameObject.h"

#include "../../../Factory/RenderShaderParam/RenderShaderParamFactory.h"

#include "../../../Component/Render/ShaderParam/RenderShaderParamBase.h"

void RenderModelComponentBase::Init()
{
	RegisterBaseTypeIDs<RenderModelComponentBase>();

	if (!m_commonCoreRenderModel)
	{
		m_commonCoreRenderModel = std::make_shared<CommonCoreRenderModel>();
	}

	m_commonCoreRenderModel->Init();
}

void RenderModelComponentBase::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCoreRenderModel)
	{
		m_commonCoreRenderModel->PostLoadInit(owner_);
	}
}

void RenderModelComponentBase::ImGuiSpawnDataInspector()
{
	if (!m_commonCoreRenderModel) { return; }

	m_commonCoreRenderModel->ImGuiSpawnDataInspector();
}
void RenderModelComponentBase::ImGuiPrefabDataInspector()
{
	if (!m_commonCoreRenderModel) { return; }

	m_commonCoreRenderModel->ImGuiPrefabDataInspector();

	if (ImGui::CollapsingHeader("RenderShaderParamSelector"))
	{
		// もしセレクターから選択されインスタンスが生成されたならリストに追加
		CommonStruct::GenericClassData<RenderShaderParamBase> data_ = {};
		ImGuiUtility::RenderShaderSelector("RenderShaderParamSelector", data_);

		if (data_.classData)
		{
			AddRenderShaderParamList(data_);
		}

		// プレハブ情報を操作
		{
			auto itr_ = m_renderShaderParamList.begin();

			while (itr_ != m_renderShaderParamList.end())
			{
				ImGui::PushID(std::to_address(itr_));

				if (!itr_->classData)
				{
					itr_++;
					ImGui::PopID();
					continue;
				}

				if (ImGui::TreeNode(itr_->name.c_str()))
				{
					ImGui::SameLine();
					if (ImGui::Button("Delete"))
					{
						itr_ = m_renderShaderParamList.erase(itr_);
						ImGui::PopID();
						ImGui::TreePop();
						continue;
					}

					itr_->classData->ImGuiPrefabDataInspector();
					ImGui::TreePop();
				}

				itr_++;

				ImGui::PopID();
			}
		}
	}
}

void RenderModelComponentBase::DeserializeSpawnData(const nlohmann::json& Json)
{
	if (!m_commonCoreRenderModel) { return; }
	if (Json.is_null())			  { return; }

	m_commonCoreRenderModel->DeserializeSpawnData(Json);
}
void RenderModelComponentBase::DeserializePrefabData(const nlohmann::json& Json)
{
	if (!m_commonCoreRenderModel) { return; }
	if (Json.is_null())			  { return; }

	m_commonCoreRenderModel->DeserializePrefabData(Json);

	if (JsonUtility::CheckIsContainAndArray(Json, "RenderShaderParamList"))
	{
		for (const auto& jsonArray_ : Json["RenderShaderParamList"])
		{
			CommonStruct::GenericClassData<RenderShaderParamBase> data_ = {};

			data_.name = jsonArray_.value("ClassName", CommonConstant::STRING_UNKNOWN);
			data_.classData = RenderShaderParamFactory::GetInstance().Create(data_.name);

			if (!data_.classData) { return; }

			data_.classData->Init();
			data_.classData->DeserializePrefabData(jsonArray_);
			AddRenderShaderParamList(data_);
		}
	}
}

nlohmann::json RenderModelComponentBase::SerializeSpawnData()
{
	auto rootJson_ = nlohmann::json();
	if (!m_commonCoreRenderModel) { return rootJson_; }
	
	auto json_ = m_commonCoreRenderModel->SerializeSpawnData();
	if (!json_.is_null())
	{
		rootJson_.update(json_);
	}

	return rootJson_;
}
nlohmann::json RenderModelComponentBase::SerializePrefabData()
{
	auto rootJson_ = nlohmann::json();
	if (!m_commonCoreRenderModel) { return rootJson_; }

	auto commonCoreRenderJson_ = m_commonCoreRenderModel->SerializePrefabData();
	if (!commonCoreRenderJson_.is_null())
	{
		rootJson_.update(commonCoreRenderJson_);
	}

	{
		auto arrayJson_ = nlohmann::json::array();

		// クラス名とクラス情報を保存
		for (const auto& renderShaderParam_ : m_renderShaderParamList)
		{
			if (!renderShaderParam_.classData) { continue; }

			auto json_ = nlohmann::json();

			json_["ClassName"] = renderShaderParam_.name;
			auto createdJson_ = renderShaderParam_.classData->SerializePrefabData();
			if (!createdJson_.is_null())
			{
				json_.update(createdJson_);
			}

			arrayJson_.emplace_back(json_);
		}

		rootJson_["RenderShaderParamList"] = arrayJson_;
	}

	return rootJson_;
}

void RenderModelComponentBase::AddRenderShaderParamList(CommonStruct::GenericClassData<RenderShaderParamBase> WantAdd)
{
	if (!WantAdd.classData) { return; }

	m_renderShaderParamList.emplace_back(WantAdd);
}