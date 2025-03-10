#pragma once
#include "native_hooks.hpp"
#include "util/globals.h"

namespace shop_controller
{
	inline void SET_WARNING_MESSAGE_WITH_HEADER(rage::scrNativeCallContext* src)
	{
		if (auto entry_line = src->get_arg<const char*>(1); !strcmp(entry_line, "CTALERT_F_2"))
		{
			LOG(warn, "You have received a transaction rate limit");
			notify(Warning, 5s, "You have received a transaction rate limit");

			*scr_globals::transaction_overlimit.as<PBOOL>() = FALSE;

			return;
		}

		HUD::SET_WARNING_MESSAGE_WITH_HEADER(src->get_arg<const char*>(0), src->get_arg<const char*>(1), src->get_arg<int>(2), src->get_arg<const char*>(3), src->get_arg<BOOL>(4), src->get_arg<Any>(5), src->get_arg<Any*>(6), src->get_arg<Any*>(7), src->get_arg<BOOL>(8), src->get_arg<Any>(9));
	}
}
