#pragma once

class CommonCoreRotation : public CommonCoreBase
{

public:

	CommonCoreRotation ()          = default;
	~CommonCoreRotation() override = default;

	void Init		 ()									 override;
	void PostLoadInit(std::shared_ptr<GameObject> Owner) override;

	void ResetRotationDirection() { m_rotationDirection = Math::Vector3::Zero; }

	std::weak_ptr<TransformComponent> GetSelfTransformComponentCache() const { return m_selfTransformComponentCache; }

	const Math::Vector3& GetRotationDirection    () const { return m_rotationDirection; }
	Math::Vector3&		 GetWorkRotationDirection()		  { return m_rotationDirection; }

private:

	std::weak_ptr<TransformComponent> m_selfTransformComponentCache;

	Math::Vector3 m_rotationDirection = Math::Vector3::Zero;

};