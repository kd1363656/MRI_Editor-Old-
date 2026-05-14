#pragma once

class TransformComponent;

class TransformStrategyCreateLocalSTRParentSRT : public IStrategy<TransformComponent>
{

public:

	TransformStrategyCreateLocalSTRParentSRT ()          = default;
	~TransformStrategyCreateLocalSTRParentSRT() override = default;

	void Execute(std::weak_ptr<TransformComponent> Owner) override;

};