#include "RotationInputMouseComponent.h"

#include "../../../Transform/TransformComponent.h"

#include "../../../../CommonCore/Rotation/CommonCoreRotation.h"

#include "../../../../../System/Calculation/Calculation.h"
#include "../../../../../System/Input/RawInputManager.h"

// 線形補完で追うようにする(滑らかで奇麗だから)
void RotationInputMouseComponent::Init()
{
	if (!m_commonCoreRotation)
	{
		m_commonCoreRotation = std::make_shared<CommonCoreRotation>();
	}

	m_commonCoreRotation->Init();

	m_targetRotation = Math::Vector3::Zero;

	m_minRotatableDegreeX = -CommonConstant::QUARTER_DEGREE;
	m_maxRotatableDegreeX =  CommonConstant::QUARTER_DEGREE;

	m_maxRotationSpeed = CommonConstant::FLOAT_ONE;

	m_disableMouseLock = false;
}

void RotationInputMouseComponent::PostLoadInit()
{
	auto owner_ = GetOwner();
	if (!owner_) { return; }

	if (m_commonCoreRotation)
	{
		m_commonCoreRotation->PostLoadInit(owner_);
	}
}

void RotationInputMouseComponent::Update()
{
	MouseLock();

	// マウスロックが外れていたら処理可能
	if (m_disableMouseLock) { return; }

	UpdateRotation();
}

void RotationInputMouseComponent::ImGuiPrefabDataInspector()
{
	ImGui::DragFloat("MaxRotationSpeed", &m_maxRotationSpeed, 0.1f);

	ImGui::DragFloat("MinRotatableDegreeX", &m_minRotatableDegreeX);
	ImGui::DragFloat("MaxRotatableDegreeX", &m_maxRotatableDegreeX);
}
void RotationInputMouseComponent::DeserializePrefabData(const nlohmann::json& Json)
{
	if (Json.is_null()) { return; }

	m_minRotatableDegreeX = Json.value("MinRotatableDegreeX", -CommonConstant::QUARTER_DEGREE);
	m_maxRotatableDegreeX = Json.value("MaxRotatableDegreeX",  CommonConstant::QUARTER_DEGREE);

	m_maxRotationSpeed = Json.value("MaxRotationSpeed", CommonConstant::FLOAT_ONE);
}

nlohmann::json RotationInputMouseComponent::SerializePrefabData()
{
	auto json_ = nlohmann::json();

	json_["MinRotatableDegreeX"] = m_minRotatableDegreeX;
	json_["MaxRotatableDegreeX"] = m_maxRotatableDegreeX;
	
	json_["MaxRotationSpeed"] = m_maxRotationSpeed;

	return json_;
}

void RotationInputMouseComponent::MouseLock()
{
	auto& input_ = RawInputManager::GetInstance();

	// 一回だけキー入力に反応してトグル操作を行う
	if (input_.IsKeyPressedOnce(VK_TAB))
	{
		m_disableMouseLock = m_disableMouseLock ? false : true;
	}
}

void RotationInputMouseComponent::UpdateRotation()
{
	if (!m_commonCoreRotation) { return; }

	auto selfTransformComopnentCache_ = m_commonCoreRotation->GetSelfTransformComponentCache().lock();
	if (!selfTransformComopnentCache_) { return; }

	const auto& application_ = Application::Instance();

	// 画面中心のスクリーン座標
	const CommonStruct::WindowSize windowHalfSize_ = 
	{
		application_.GetWindowSize().width  / 2,
		application_.GetWindowSize().height / 2
	};

	POINT nowMousePosition_ = {};
	GetCursorPos(&nowMousePosition_);

	// マウス移動量
	Math::Vector2 movement_ = 
	{
		static_cast<float>(nowMousePosition_.x - windowHalfSize_.width ) ,
		static_cast<float>(nowMousePosition_.y - windowHalfSize_.height)
	};

	// マウスを画面中心に戻す
	SetCursorPos(windowHalfSize_.width, windowHalfSize_.height);

	const float deltaTime_ = application_.GetScaledDeltaTime();

	// マウスが動いていないなら処理しない
	if (movement_.LengthSquared() >= CommonConstant::EPSILON)
	{
		const float rotationSpeed_ = m_maxRotationSpeed * deltaTime_;

		// 修正：ターゲット回転からオイラー角を取得
		Math::Vector3 rotation_ = m_targetRotation;

		// マウス移動をオイラー角に加算（X: 上下, Y: 左右）
		rotation_.x += movement_.y * rotationSpeed_;
		rotation_.y += movement_.x * rotationSpeed_;

		// X軸の回転にだけ制限をかける（上下）
		rotation_.x = std::clamp(rotation_.x, m_minRotatableDegreeX, m_maxRotatableDegreeX);

		// 修正：回転を蓄積し、Slerpで向かう先に設定
		m_targetRotation = rotation_;
	}

	// 現在の回転と目標の回転を取得
	Math::Quaternion currentQuaternion_ = selfTransformComopnentCache_->GetRotation();
	Math::Quaternion targetQuaternion_  = Calculation::EulerToQuaternion(m_targetRotation);

	// クオータニオン補正（最短経路）
	if (currentQuaternion_.Dot(targetQuaternion_) <= CommonConstant::EPSILON)
	{
		targetQuaternion_ = -targetQuaternion_;
	}

	// 補完処理
	Math::Quaternion resultQuaternion_ = Math::Quaternion::Slerp
	(
		currentQuaternion_			   ,
		targetQuaternion_			   ,
		m_maxRotationSpeed * deltaTime_
	);

	selfTransformComopnentCache_->SetRotation(resultQuaternion_);
}