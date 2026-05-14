#include "InterpolatorFactory.h"

#include "../../../Modifier/Interporator/Hermite/HermiteInterpolatorModifier.h"

void InterpolatorModifierFactory::Init()
{
	RegisterFactoryMethod<HermiteInterpolatorModifier>();
}