#include "NotifyToComponentFactory.h"

#include "../../../Observer/Component/Notify/Keep/KeepNotifyToComponent.h"
#include "../../../Observer/Component/Notify/Moment/MomentNotifyToComponent.h"

void NotifyToComponentFactory::Init()
{
	RegisterFactoryMethod<KeepNotifyToComponent>  ();
	RegisterFactoryMethod<MomentNotifyToComponent>();
}