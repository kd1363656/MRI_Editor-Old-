#pragma once

class CommonCoreMove;

class TransformComponent;

class GameObject;

class CommonCoreMoveInput : public CommonCoreBase
{

public:

	CommonCoreMoveInput ()          = default;
	~CommonCoreMoveInput() override = default;

	void Init        ()									 override;
	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	void ResetInputMoveDirection() { m_inputMoveDirection = Math::Vector3::Zero; }

	std::weak_ptr<CommonCoreMove> GetCommonCoreMoveCache() const { return m_commonCoreMove; }

	const Math::Vector3& GetInputMoveDirection() const { return m_inputMoveDirection; }

	Math::Vector3& GetWorkInputMoveDirection() { return m_inputMoveDirection; }

	void SetInputMoveDirection(const Math::Vector3& Set) { m_inputMoveDirection = Set; }

private:

	std::shared_ptr<CommonCoreMove> m_commonCoreMove = nullptr;

	Math::Vector3 m_inputMoveDirection = Math::Vector3::Zero;
};