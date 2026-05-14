#include "ComponentFactory.h"

#include "../../Component/Transform/TransformComponent.h"

#include "../../Component/Render/Animatoin/Model/RenderAnimationModelComponent.h"
#include "../../Component/Render/Static/Model/RenderStaticModelComponent.h"

#include "../../Component/Input/Player/InputPlayerComponent.h"

#include "../../Component/BehaviorTree/BehaviorTreeComponent.h"

#include "../../Component/Camera/CameraComponent.h"

#include "../../Component/SpringArm/SpringArmComponent.h"

#include "../../Component/Move/Input/BasedOnTarget/MoveInputBasedOnTargetComponent.h"
#include "../../Component/Move/Input/Action/MoveInputActionComponent.h"

#include "../../Component/Rotation/Input/BasedOnTarget/Instant/RotationInputBasedOnTargetInstantComponent.h"
#include "../../Component/Rotation/Input/BasedOnTarget/Smooth/RotationInputBasedOnTargetSmoothComponent.h"
#include "../../Component/Rotation/Input/Mouse/RotationInputMouseComponent.h"

#include "../../Component/Combat/CombatComponent.h"

#include "../../Component/Physics/Collider/PhysicsColliderComponent.h"
#include "../../Component/Physics/Collision/Ray/PhysicsCollisionRayComponent.h"
#include "../../Component/Physics/Collision/Sphere/PhysicsCollisionSphereComponent.h"

#include "../../Component/Physics/DebugWire/PhysicsDebugWireComponent.h"

#include "../../Component/Physics/Gravity/PhysicsGravityComponent.h"

#include "../../GameObject/GameObject.h"

#include "../../Scene/SceneManager.h"

void ComponentFactory::Init()
{
#ifdef _DEBUG
	KdDebugGUI::Instance().AddLog("============ Start register gameObject factory ============\n\n");
#endif

	RegisterFactoryMethod<TransformComponent>();

	RegisterFactoryMethod<RenderAnimationModelComponent>();
	RegisterFactoryMethod<RenderStaticModelComponent>   ();

	RegisterFactoryMethod<InputPlayerComponent>();

	RegisterFactoryMethod<BehaviorTreeComponent>();

	RegisterFactoryMethod<CameraComponent>();

	RegisterFactoryMethod<SpringArmComponent>();

	RegisterFactoryMethod<MoveInputBasedOnTargetComponent>();
	RegisterFactoryMethod<MoveInputActionComponent>       ();

	RegisterFactoryMethod<RotationInputBasedOnTargetInstantComponent>();
	RegisterFactoryMethod<RotationInputBasedOnTargetSmoothComponent> ();
	RegisterFactoryMethod<RotationInputMouseComponent>				 ();

	RegisterFactoryMethod<CombatComponent>();

	RegisterFactoryMethod<PhysicsColliderComponent>       ();
	RegisterFactoryMethod<PhysicsCollisionRayComponent>   ();
	RegisterFactoryMethod<PhysicsCollisionSphereComponent>();
	
	RegisterFactoryMethod<PhysicsDebugWireComponent>();

	RegisterFactoryMethod<PhysicsGravityComponent>();

#ifdef _DEBUG
	KdDebugGUI::Instance().AddLog("\n============ End register gameObject factory ==============\n\n\n\n");
#endif
}