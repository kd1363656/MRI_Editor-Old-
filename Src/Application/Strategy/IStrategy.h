#pragma once

template <class OwnerType>
class IStrategy
{

public:

	IStrategy         () = default;
	virtual ~IStrategy() = default;

	virtual void Execute(std::weak_ptr<OwnerType> Owner) = 0;

};