#pragma once
#include "../../GenericFactoryBase.h"
#include "../../../Modifier/Interporator/InterpolatorModifierBase.h"

class GameObject;

class InterpolatorModifierFactory : public GenericFactoryBase<InterpolatorModifierFactory , InterpolatorModifierBase>
{

public:

	void Init() override;

private:

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<InterpolatorModifierFactory>;

	InterpolatorModifierFactory ()          = default;
	~InterpolatorModifierFactory() override = default;

};