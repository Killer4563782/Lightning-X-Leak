#pragma once
#include "commands/manager.hpp"
#include "commands/settings.h"
#include "util/globals.h"

namespace main_transition
{
	inline void SC_TRANSITION_NEWS_SHOW(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle)
			src->set_return_value<BOOL>(FALSE);
		else
			src->set_return_value<BOOL>(SOCIALCLUB::SC_TRANSITION_NEWS_SHOW(src->get_arg<Any>(0)));
	}

	inline void SC_TRANSITION_NEWS_SHOW_TIMED(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle)
			src->set_return_value<BOOL>(FALSE);
		else
			src->set_return_value<BOOL>(SOCIALCLUB::SC_TRANSITION_NEWS_SHOW_TIMED(src->get_arg<Any>(0), src->get_arg<Any>(0)));
	}

	inline void CLEAR_PED_TASKS_IMMEDIATELY(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle && *scr_globals::transition_state.as<eTransitionState*>() <= eTransitionState::FreemodeFinalSetupPlayer)
			return;

		TASK::CLEAR_PED_TASKS_IMMEDIATELY(src->get_arg<Entity>(0));
	}

	/**
	 * @brief This hook lets you stop player-switch in "Pre-HUD Checks"
	 */
	inline void IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle)
			src->set_return_value<BOOL>(TRUE);
		else
			src->set_return_value<BOOL>(STREAMING::IS_SWITCH_TO_MULTI_FIRSTPART_FINISHED());
	}

	/**
	 * @brief Prevets map from unloading.
	 */
	inline void SET_FOCUS_ENTITY(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle)
			STREAMING::SET_FOCUS_ENTITY(self::ped);
		else
			STREAMING::SET_FOCUS_ENTITY(src->get_arg<Entity>(0));
	}

	/**
	 * @brief Let's you controll your ped when going sp to mp.
	 */
	inline void ACTIVATE_FRONTEND_MENU(rage::scrNativeCallContext* src)
	{
		const auto menu_hash = src->get_arg<Hash>(0);
		const auto toggle_pause = src->get_arg<BOOL>(1);
		const auto component = src->get_arg<int>(2);

		if ("seamless_joining"_BC->m_toggle && menu_hash != "FE_MENU_VERSION_EMPTY_NO_BACKGROUND"_joaat)
			HUD::ACTIVATE_FRONTEND_MENU(menu_hash, toggle_pause, component);

		if (!"seamless_joining"_BC->m_toggle)
			HUD::ACTIVATE_FRONTEND_MENU(menu_hash, toggle_pause, component);
	}

	/**
	 * @brief Let's you controll your ped when going sp to mp.
	 */
	inline void RESTART_FRONTEND_MENU(rage::scrNativeCallContext* src)
	{
		const auto menu_hash = src->get_arg<Hash>(0);
		const auto p1 = src->get_arg<int>(1);

		if ("seamless_joining"_BC->m_toggle)
		{
			if (menu_hash != "FE_MENU_VERSION_EMPTY_NO_BACKGROUND"_joaat)
			{
				HUD::RESTART_FRONTEND_MENU(menu_hash, p1);
			}
		}
		else
		{
			HUD::RESTART_FRONTEND_MENU(menu_hash, p1);
		}
	}

	/**
	 * @brief Prevents the game from freezing your screen.
	 */
	inline void TOGGLE_PAUSED_RENDERPHASES(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle)
			GRAPHICS::RESET_PAUSED_RENDERPHASES();
		else
			GRAPHICS::TOGGLE_PAUSED_RENDERPHASES(src->get_arg<int>(0));
	}

	/**
	 * @brief Makes you visible.
	 */
	inline void SET_ENTITY_VISIBLE(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle && src->get_arg<Entity>(0) == self::ped)
			ENTITY::SET_ENTITY_VISIBLE(self::ped, TRUE, FALSE);
		else
			ENTITY::SET_ENTITY_VISIBLE(src->get_arg<Entity>(0), src->get_arg<BOOL>(1), src->get_arg<BOOL>(2));
	}

	/**
	 * @brief Prevents the game from teleporting you.
	 */
	inline void SET_PED_COORDS_KEEP_VEHICLE(rage::scrNativeCallContext* src)
	{
		if (!"seamless_joining"_BC->m_toggle
			|| *scr_globals::transition_state.as<eTransitionState*>() ==
			eTransitionState::ConfirmFreemodeSessionJoining
			// This check is for character selection if I remember correctly.
			|| src->get_arg<Ped>(0) != self::ped)
			PED::SET_PED_COORDS_KEEP_VEHICLE(src->get_arg<Ped>(0), src->get_arg<float>(1), src->get_arg<float>(2), src->get_arg<float>(3));
	}

	/**
	 * @brief Prevents you from falling.
	 */
	inline void SET_ENTITY_COLLISION(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle && src->get_arg<Entity>(0) == self::ped)
			ENTITY::SET_ENTITY_COLLISION(self::ped, TRUE, FALSE);
		else
			ENTITY::SET_ENTITY_COLLISION(src->get_arg<Entity>(0), src->get_arg<BOOL>(1), src->get_arg<BOOL>(2));
	}

	/**
	 * @brief Allows controll in session switch.
	 */
	inline void SET_PLAYER_CONTROL(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle)
		{
			PLAYER::SET_PLAYER_CONTROL(self::player, TRUE, 0);
			*scr_globals::script_global(2621446).as<int*>() |= 1 << 16; // prevents scripts freezing us / resetting momentum during create player
		}
		else 
			PLAYER::SET_PLAYER_CONTROL(src->get_arg<Player>(0), src->get_arg<BOOL>(1), src->get_arg<int>(2));
	}

	/**
	 * @brief Allows controll in session switch.
	 */
	inline void FREEZE_ENTITY_POSITION(rage::scrNativeCallContext* src)
	{
		if ("seamless_joining"_BC->m_toggle && src->get_arg<Entity>(0) == self::ped)
			ENTITY::FREEZE_ENTITY_POSITION(self::ped, FALSE);
		else 
			ENTITY::FREEZE_ENTITY_POSITION(src->get_arg<Entity>(0), src->get_arg<BOOL>(1));
	}

	/**
	 * @brief Prevents player from teleporting after switch.
	 */
	inline void NETWORK_RESURRECT_LOCAL_PLAYER(rage::scrNativeCallContext* src)
	{
		if (!"seamless_joining"_BC->m_toggle)
			NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(src->get_arg<float>(0), src->get_arg<float>(1),
			                                        src->get_arg<float>(2), src->get_arg<float>(3),
			                                        src->get_arg<BOOL>(4), src->get_arg<BOOL>(5), src->get_arg<BOOL>(6),
			                                        src->get_arg<int>(7), src->get_arg<int>(8));
	}

	//
	// PLAYER_SWITCH END
	//

	/**
	 * @brief Prevent weird reloading when using custom dlcs.
	 * @note You should not be using custom dlcs like this in the first place.
	 */
	inline void GET_EVER_HAD_BAD_PACK_ORDER(rage::scrNativeCallContext* src)
	{
		src->set_return_value<BOOL>(FALSE);
	}

	inline void NETWORK_CAN_BAIL(rage::scrNativeCallContext* src)
	{
		if (commands::settings::session::prevent_network_bail)
			src->set_return_value<BOOL>(FALSE);
		else
			src->set_return_value<BOOL>(NETWORK::NETWORK_CAN_BAIL());
	}

	inline void NETWORK_BAIL(rage::scrNativeCallContext* src)
	{
		if (commands::settings::session::prevent_network_bail)
		{
			notify(Success, 7s, "Prevented network bail: {}.{}.{}", src->get_arg<int>(0), src->get_arg<int>(1), src->get_arg<int>(2));
			return;
		}

		NETWORK::NETWORK_BAIL(src->get_arg<int>(0), src->get_arg<int>(1), src->get_arg<int>(2));
	}
}
