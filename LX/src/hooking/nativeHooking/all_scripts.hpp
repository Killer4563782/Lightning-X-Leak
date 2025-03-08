#pragma once
#include "commands/manager.hpp"
#include "util/script_global.hpp"
#include "rage/enums.h"
#include "services/reactions/reactions.h"

namespace all_scripts
{
	inline void NETWORK_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT(rage::scrNativeCallContext* src)
	{
		if (src->get_arg<int>(2) != -1 && src->get_arg<uint32_t>(2) >= 0x100)
		{
			if (crash.process(nullptr, "Crash N5")) // out of bounds instance id
				return;
		}

		NETWORK::NETWORK_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT(src->get_arg<int>(0), src->get_arg<BOOL>(1), src->get_arg<int>(2));
	}

	inline void NETWORK_TRY_TO_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT(rage::scrNativeCallContext* src)
	{
		if (src->get_arg<int>(2) != -1 && src->get_arg<uint32_t>(2) >= 0x100)
		{
			if (crash.process(nullptr, "Crash N2")) // out of bounds instance id
			{
				src->set_return_value<BOOL>(FALSE);
				return;
			}
		}

		src->set_return_value<BOOL>(NETWORK::NETWORK_TRY_TO_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT(src->get_arg<int>(0), src->get_arg<BOOL>(1), src->get_arg<int>(2)));
	}

	inline void SET_CURRENT_PED_WEAPON(rage::scrNativeCallContext* src)
	{
		const auto ped = src->get_arg<Ped>(0);
		const auto hash = src->get_arg<rage::joaat_t>(1);

		if ("weapons_in_interior"_BC->m_toggle && ped == self::ped && hash == "WEAPON_UNARMED"_joaat)
			return;

		WEAPON::SET_CURRENT_PED_WEAPON(ped, hash, src->get_arg<int>(2));
	}

	inline void DISABLE_CONTROL_ACTION(rage::scrNativeCallContext* src)
	{
		const auto action = src->get_arg<ControllerInputs>(1);

		if ("free_movement_in_interior"_BC->m_toggle)
		{
			switch (action)
			{
			case INPUT_JUMP:
			case INPUT_SPRINT:
			case INPUT_DUCK:
			case INPUT_COVER:
			case INPUT_TALK:
			case INPUT_VEH_HEADLIGHT:
			case INPUT_VEH_NEXT_RADIO:
			case INPUT_VEH_PREV_RADIO:
			case INPUT_VEH_NEXT_RADIO_TRACK:
			case INPUT_VEH_PREV_RADIO_TRACK:
			case INPUT_VEH_RADIO_WHEEL:
			case INPUT_VEH_ROOF:
			case INPUT_VEH_JUMP:
			case INPUT_VEH_HYDRAULICS_CONTROL_TOGGLE:
			case INPUT_VEH_CAR_JUMP:
			case INPUT_VEH_ROCKET_BOOST:
			case INPUT_VEH_FLY_BOOST:
			case INPUT_VEH_PARACHUTE:
			case INPUT_VEH_BIKE_WINGS:
			case INPUT_VEH_TRANSFORM:
				return;
			}
		}

		if ("weapons_in_interior"_BC->m_toggle) // Filtering from the inside of Kosatka
		{
			switch (action)
			{
			case INPUT_ATTACK:
			case INPUT_AIM:
			case INPUT_SELECT_WEAPON:
			case INPUT_DETONATE:
			case INPUT_WEAPON_SPECIAL:
			case INPUT_WEAPON_SPECIAL_TWO:
			case INPUT_VEH_AIM:
			case INPUT_VEH_ATTACK:
			case INPUT_VEH_ATTACK2:
			case INPUT_VEH_PASSENGER_AIM:
			case INPUT_VEH_PASSENGER_ATTACK:
			case INPUT_VEH_SELECT_NEXT_WEAPON:
			case INPUT_VEH_SELECT_PREV_WEAPON:
			case INPUT_VEH_FLY_ATTACK:
			case INPUT_MELEE_ATTACK_LIGHT:
			case INPUT_MELEE_ATTACK_HEAVY:
			case INPUT_MELEE_ATTACK_ALTERNATE:
			case INPUT_MELEE_BLOCK:
			case INPUT_SELECT_WEAPON_UNARMED:
			case INPUT_SELECT_WEAPON_MELEE:
			case INPUT_SELECT_WEAPON_HANDGUN:
			case INPUT_SELECT_WEAPON_SHOTGUN:
			case INPUT_SELECT_WEAPON_SMG:
			case INPUT_SELECT_WEAPON_AUTO_RIFLE:
			case INPUT_SELECT_WEAPON_SNIPER:
			case INPUT_SELECT_WEAPON_HEAVY:
			case INPUT_SELECT_WEAPON_SPECIAL:
			case INPUT_ATTACK2:
			case INPUT_MELEE_ATTACK1:
			case INPUT_MELEE_ATTACK2:
			case INPUT_VEH_GUN_LEFT:
			case INPUT_VEH_GUN_RIGHT:
			case INPUT_VEH_GUN_UP:
			case INPUT_VEH_GUN_DOWN:
			case INPUT_VEH_MELEE_HOLD:
			case INPUT_VEH_MELEE_LEFT:
			case INPUT_VEH_MELEE_RIGHT:
				return;
			}
		}

		PAD::DISABLE_CONTROL_ACTION(src->get_arg<int>(0), action, src->get_arg<int>(2));
	}

	inline void HUD_FORCE_WEAPON_WHEEL(rage::scrNativeCallContext* src)
	{
		if ("weapons_in_interior"_BC->m_toggle && src->get_arg<BOOL>(0) == false)
			return;

		HUD::HUD_FORCE_WEAPON_WHEEL(src->get_arg<BOOL>(0));
	}

	inline void SET_ENTITY_HEALTH(rage::scrNativeCallContext* src)
	{
		Entity entity = src->get_arg<Entity>(0);
		int health = src->get_arg<int>(1);
		int p2 = src->get_arg<int>(2);
		int p3 = src->get_arg<int>(3);

		if ("godmode"_LC->m_toggle && entity == self::ped)
			health = ENTITY::GET_ENTITY_MAX_HEALTH(entity);

		ENTITY::SET_ENTITY_HEALTH(entity, health, p2, p3);
	}

	inline void APPLY_DAMAGE_TO_PED(rage::scrNativeCallContext* src)
	{
		Ped ped = src->get_arg<Ped>(0);
		int damage = src->get_arg<int>(1);
		BOOL damage_armor_first = src->get_arg<BOOL>(2);
		Any p3 = src->get_arg<Any>(3);
		int p4 = src->get_arg<int>(4);

		if ("godmode"_LC->m_toggle && ped == self::ped)
			return;

		PED::APPLY_DAMAGE_TO_PED(ped, damage, damage_armor_first, p3, p4);
	}

	inline void RETURN_TRUE(rage::scrNativeCallContext* src)
	{
		src->set_return_value<BOOL>(TRUE);
	}

	inline void RETURN_FALSE(rage::scrNativeCallContext* src)
	{
		src->set_return_value<BOOL>(FALSE);
	}

	inline void DO_NOTHING(rage::scrNativeCallContext* src)
	{

	}
}