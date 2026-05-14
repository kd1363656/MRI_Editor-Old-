#include "RenderShaderParamFactory.h"

#include "../../Component/Render/ShaderParam/AlphaDither/RenderShaderParamAlphaDither.h"

void RenderShaderParamFactory::Init()
{
	RegisterFactoryMethod<RenderShaderParamAlphaDither>();
}