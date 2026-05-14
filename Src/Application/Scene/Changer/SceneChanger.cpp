#include "SceneChanger.h"

#include "../../main.h"

#include "../../Utility/Json/JsonUtility.h"
#include "../../Utility/ImGui/ImGuiUtility.h"

#include "../../../System/Calculation/Calculation.h"

void SceneChanger::Init()
{
	m_tex.reset();

	m_color   = kWhiteColor;
	m_color.w = 0.0f;

	m_scale = Math::Vector2::One;
	m_pos   = Math::Vector2::Zero;

	m_nowState = CommonEnum::FadeState::None;

	m_fadeInSpeed  = 0.0f;
	m_fadingSpeed  = 0.0f;
	m_fadeOutSpeed = 0.0f;
}

void SceneChanger::Draw()
{
	auto tex_ = m_tex.lock();
	if (!tex_) { return; }

	const auto width_  = static_cast<float>(tex_->GetWidth ());
	const auto height_ = static_cast<float>(tex_->GetHeight());

	const Math::Vector2 scale_ =
	{
		width_  * m_scale.x ,
		height_ * m_scale.y
	};

	Math::Rectangle rect_ = { 0u , 0u , static_cast<long>(width_) , static_cast<long>(height_)};

	KdShaderManager::Instance().m_spriteShader.DrawTex
	(
		tex_					   , 
		static_cast<int>(m_pos.x)  ,
		static_cast<int>(m_pos.y)  , 
		static_cast<int>(scale_.x) , 
		static_cast<int>(scale_.y) , 
		&rect_					   ,
		&m_color
	);
}

void SceneChanger::Update()
{
	switch(static_cast<int>(m_nowState))
	{
		case static_cast<int>(CommonEnum::FadeState::FadeIn):
			FadeIn();
		break;

		case static_cast<int>(CommonEnum::FadeState::Fading):

		break;

		case static_cast<int>(CommonEnum::FadeState::FadeOut):

		break;

		default:
		break;
	}
}

void SceneChanger::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }
	
	m_texFilePath = Json.value("TexFilePath" , CommonConstant::STRING_UNKNOWN);

	if (Json.contains("Color"))
	{
		m_color = JsonUtility::JsonToColor(Json["Color"]);
	}

	if (Json.contains("Pos"))
	{
		m_pos = JsonUtility::JsonToVec2(Json["Pos"]);
	}

	m_fadeInSpeed  = Json.value("FadInSpeed"  , 0.0f);
	m_fadingSpeed  = Json.value("FadingSpeed" , 0.0f);
	m_fadeOutSpeed = Json.value("FadOutSpeed" , 0.0f);
	
}

nlohmann::json SceneChanger::SerializePrefabData()
{
	auto json_ = nlohmann::json();
	 
	json_["TexFilePath"] = m_texFilePath;

	// 初めは透明度"0.0f"
	m_color.w = 0.0f;
	json_["Color"] = JsonUtility::ColorToJson(m_color);

	json_["Scale"] = JsonUtility::Vec2ToJson(m_scale);
	json_["Pos"  ] = JsonUtility::Vec2ToJson(m_pos);

	json_["FadeInSpeed"] = m_fadeInSpeed;
	json_["FadingSpeed"] = m_fadingSpeed;
	json_["FadOutSpeed"] = m_fadeOutSpeed;

	return json_;
}

void SceneChanger::ImGuiPrefabDataInspector()
{
	if (ImGuiUtility::SelectFilePath("AssetFilePath" , m_texFilePath))
	{
		m_tex = KdAssets::Instance().m_textures.GetData(m_texFilePath);
	}

	ImGui::ColorEdit4("Color" , &m_color.x);

	ImGui::DragFloat2("Scale" , &m_scale.x);
	ImGui::DragFloat2("Pos"   , &m_pos.x  );

	ImGui::DragFloat("FadeInSpeed" , &m_fadeInSpeed );
	ImGui::DragFloat("FadingSpeed" , &m_fadingSpeed );
	ImGui::DragFloat("FadOutSpeed" , &m_fadeOutSpeed);

	ImGuiUtility::EnumRadioButtonSelector("State" , m_nowState , ImGuiUtility::FADE_STATE_LIST);
}

void SceneChanger::FadeIn()
{
	const float deltaTime_ = Application::Instance().GetScaledDeltaTime();

	m_color.w = std::lerp(m_color.w , CommonConstant::FLOAT_ONE , deltaTime_ * m_fadeInSpeed);
}