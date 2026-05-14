#include "PhysicsDebugWireComponent.h"

#include "../../../Utility/Json/JsonUtility.h"

void PhysicsDebugWireComponent::DrawDebugWire()
{
	if (!m_debugWire) { return; }

	m_debugWire->Draw();
}

void PhysicsDebugWireComponent::Init()
{
	if (!m_debugWire)
	{
		m_debugWire = std::make_shared<KdDebugWireFrame>();
	}

	m_color = kWhiteColor;

	// 無効化
	Disable();
}

void PhysicsDebugWireComponent::AddDebugWire(const KdCollider::RayInfo& RayInfo)
{
	// デバックワイヤーがしっかりとインスタンス化されていてデバック情報を飛ばす場所がしっかり決まっていたら実行
	if (!m_debugWire                        ) { return; }
	if (RayInfo.m_dir == Math::Vector3::Zero) { return; }

	m_debugWire->AddDebugLine(RayInfo.m_pos , RayInfo.m_dir , RayInfo.m_range , m_color);
}
void PhysicsDebugWireComponent::AddDebugWire(const KdCollider::SphereInfo& SphereInfo)
{
	if (!m_debugWire) { return; }

	m_debugWire->AddDebugSphere(SphereInfo.m_sphere.Center, SphereInfo.m_sphere.Radius, m_color);
}

void PhysicsDebugWireComponent::ImGuiPrefabDataInspector()
{
	ImGui::ColorEdit4("WireColor" , &m_color.x);
}

void PhysicsDebugWireComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	if (Json.contains("Color"))
	{
		m_color = JsonUtility::JsonToColor(Json["Color"]);
	}
}
nlohmann::json PhysicsDebugWireComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["Color"] = JsonUtility::ColorToJson(m_color);

	return json_;
}