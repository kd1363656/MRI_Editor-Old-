#include "CommonCoreMove.h"
#include "../../Component/Transform/TransformComponent.h"

#include "../../GameObject/GameObject.h"

#include "../../Modifier/Interporator/InterpolatorModifierBase.h"

#include "../../main.h"

void CommonCoreMove::PostLoadInit(std::shared_ptr<GameObject> Owner)
{
	if (!Owner) { return; }

	m_selfTransformComponent = Owner->GetComponent<TransformComponent>();
}