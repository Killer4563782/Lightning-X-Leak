#pragma once
#include "native_hooks.hpp"
#include "memory/pointers.h"
#include "services/api/api_service.hpp"

namespace network
{
	inline void NETWORK_SESSION_HOST(rage::scrNativeCallContext* src)
	{
		if (g_api_service->join_queued)
		{
			g_pointers->g_join_session_by_info(get_network(), &g_api_service->queued_info, 1, 1 | 2, nullptr, 0);
			g_api_service->join_queued = false;
			src->set_return_value<BOOL>(TRUE);
		}
		else
		{
			src->set_return_value<BOOL>(NETWORK::NETWORK_SESSION_HOST(src->get_arg<int>(0), src->get_arg<int>(1), src->get_arg<BOOL>(2)));
		}
	}
}
