#pragma once
#include "../GenericFactoryBase.h"

class ComponentFactory : public GenericFactoryBase<ComponentFactory , ComponentBase>
{

public:

	void Init() override;

private:

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<ComponentFactory>;

	ComponentFactory ()          = default;
	~ComponentFactory() override = default;

};