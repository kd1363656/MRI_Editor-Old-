#pragma once
#include "../GenericFactoryBase.h"
#include "../../Component/Render/ShaderParam/RenderShaderParamBase.h"

class RenderShaderParamFactory : public GenericFactoryBase<RenderShaderParamFactory , RenderShaderParamBase>
{

public:

	void Init() override;

private:

	// =============================
	// "Singleton"
	// =============================
	friend class SingletonBase<RenderShaderParamFactory>;

	RenderShaderParamFactory ()          = default;
	~RenderShaderParamFactory() override = default;

};