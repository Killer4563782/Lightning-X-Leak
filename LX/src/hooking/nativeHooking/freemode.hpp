#pragma once
#include "commands/patches/script_functions.h"
#include "util/scripts.h"

namespace freemode
{
	inline void GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(rage::scrNativeCallContext* src)
	{
		const auto hash = src->get_arg<rage::joaat_t>(0);

		if (hash == "director_mode"_joaat || hash == "main"_joaat)
		{
			src->set_return_value(0);
			return;
		}

		src->set_return_value(SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(hash));
	}

	inline void IS_PLAYER_PLAYING(rage::scrNativeCallContext* src)
	{
		// block undead OTR
		if ("reveal_players"_BC->m_toggle && src->get_arg<Player>(0) != self::player && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
			src->set_return_value<BOOL>(TRUE);
		else
			src->set_return_value<BOOL>(PLAYER::IS_PLAYER_PLAYING(src->get_arg<Player>(0)));
	};

	inline void SET_ENTITY_VISIBLE(rage::scrNativeCallContext* src)
	{
		auto entity = src->get_arg<Entity>(0);
		auto toggle = src->get_arg<bool>(1);
		auto outfit = src->get_arg<bool>(2);

		if ("invisible"_LVC->m_toggle && entity == self::ped && toggle)
			return;

		ENTITY::SET_ENTITY_VISIBLE(entity, toggle, outfit);
	}

	inline void NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA(rage::scrNativeCallContext* src)
	{
		if (NETWORK::NETWORK_IS_ACTIVITY_SESSION() || NETWORK::NETWORK_IS_IN_TRANSITION() ||
			NETWORK::NETWORK_IS_TRANSITION_TO_GAME() || NETWORK::NETWORK_IS_HOST_OF_THIS_SCRIPT())
		{
			src->set_return_value<BOOL>(NETWORK::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA());
		}
		else
		{
			if (SCRIPT::GET_HASH_OF_THIS_SCRIPT_NAME() == "freemode"_joaat && "fast_join"_BC->m_toggle)
			{
				scr_functions::set_freemode_session_active({});
				src->set_return_value<BOOL>(TRUE);
			}
			else
			{
				src->set_return_value<BOOL>(NETWORK::NETWORK_HAS_RECEIVED_HOST_BROADCAST_DATA());
			}
		}
	}
}
