#include "CommonCoreRotation.h"

#include "../../Component/Transform/TransformComponent.h"

void CommonCoreRotation::Init()
{
	m_rotationDirection = Math::Vector3::Zero;
}

void CommonCoreRotation::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	if (!Owner) { return; }

	m_selfTransformComponentCache = Owner->GetComponent<TransformComponent>();
}