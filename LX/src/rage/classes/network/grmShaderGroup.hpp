#pragma once
#include "base/pgDictionary.hpp"
#include "gui/grc_textures.h"

namespace rage
{
	struct grmShaderGroup : datBase
	{
		/* 0x08 */ pgDictionary<grcTexture>* texture_dict;
		char padding[0x10];
		void* var_array; // rage::atArray<rage::grcEffectVarEntry>
	};
	static_assert(sizeof(grmShaderGroup) == 0x28);
}