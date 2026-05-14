#pragma once
#include "../../GenericFactoryBase.h"
#include "../../../Observer/Component/Notify/NotifyToComponentBase.h"

class GameObject;

class NotifyToComponentFactory : public GenericFactoryBase<NotifyToComponentFactory, NotifyToComponentBase>
{

public:

	void Init() override;

private:

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<NotifyToComponentFactory>;

	NotifyToComponentFactory ()          = default;
	~NotifyToComponentFactory() override = default;

};