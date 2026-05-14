#pragma once

class SceneChanger
{

public:

	SceneChanger () = default;
	~SceneChanger() = default;

	void Init();
	
	void Draw();

	void Update();

	void DeserializePrefabData(const nlohmann::json& Json);

	nlohmann::json SerializePrefabData();

	void ImGuiPrefabDataInspector();
	
private:

	void FadeIn();
	
	std::weak_ptr<KdTexture> m_tex;

	std::string m_texFilePath = "";

	Math::Color m_color = Math::Color();

	Math::Vector2 m_scale = Math::Vector2::Zero;

	Math::Vector2 m_pos = {};

	CommonEnum::FadeState m_nowState = CommonEnum::FadeState::None;

	float m_fadeInSpeed  = 0.0f;
	float m_fadingSpeed  = 0.0f;
	float m_fadeOutSpeed = 0.0f;

};