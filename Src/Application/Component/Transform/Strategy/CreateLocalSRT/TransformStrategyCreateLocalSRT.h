#pragma once

class TransformComponent;

class TransformStrategyCreateLocalSRT : public IStrategy<TransformComponent>
{

public:

	TransformStrategyCreateLocalSRT ()          = default;
	~TransformStrategyCreateLocalSRT() override = default;

	void Execute(std::weak_ptr<TransformComponent> Owner) override;

};