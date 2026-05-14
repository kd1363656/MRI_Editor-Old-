#include "TransformStrategyCreateLocalSTRParentSRT.h"

#include "../../TransformComponent.h"

void TransformStrategyCreateLocalSTRParentSRT::Execute(std::weak_ptr<TransformComponent> Owner)
{
	auto owner_ = Owner.lock();
	if (!owner_) { return; }

	auto parent_ = owner_->GetParentTransformComponent().lock();
	if (!parent_) { return; }

	const Math::Matrix resultMat_ = owner_->CreateScaleMatrix       () * 
									owner_->CreateTranslationMatrix () * 
									owner_->CreateRotationMatrix    () *
								    parent_->CreateTranslationMatrix();

	owner_->SetMatrix(resultMat_);
}