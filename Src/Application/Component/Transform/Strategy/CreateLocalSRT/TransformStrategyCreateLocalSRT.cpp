#include "TransformStrategyCreateLocalSRT.h"

#include "../../TransformComponent.h"

void TransformStrategyCreateLocalSRT::Execute(std::weak_ptr<TransformComponent> Owner)
{
	auto owner_ = Owner.lock();
	if (!owner_) { return; }

	owner_->SetMatrix(owner_->CreateMatrix());
}