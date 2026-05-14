#pragma once

class TransformComponent;

class GameObject;

class CommonCoreMove : public CommonCoreBase
{

public:

	CommonCoreMove ()          = default;
	~CommonCoreMove() override = default;

	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	std::weak_ptr<TransformComponent> GetSelfTransformComponentCache() const { return m_selfTransformComponent; }

private:

	std::weak_ptr<TransformComponent> m_selfTransformComponent;

};