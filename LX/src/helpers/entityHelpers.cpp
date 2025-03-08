#include "entityHelpers.h"
#include "vehicleHelpers.h"
#include "util/patches.hpp"
#include "util/util.h"

namespace entity
{
	void cage_ped(const Ped ped)
	{
		constexpr auto hash = "prop_gold_cont_01"_joaat;
		const auto location = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
		world_model::spawn(hash, location, true);
	}

	void clean_ped(const Ped ped)
	{
		Ped player_ped = PLAYER::PLAYER_PED_ID();
		PED::CLEAR_PED_BLOOD_DAMAGE(player_ped);
		PED::CLEAR_PED_WETNESS(player_ped);
		PED::CLEAR_PED_ENV_DIRT(player_ped);
		PED::RESET_PED_VISIBLE_DAMAGE(player_ped);
		// https://forum.cfx.re/t/information-needed-to-clear-visible-player-damage-scars-etc/283216
		// https://docs.fivem.net/natives/?_0x397C38AA7B4A5F83
		for (int i = 0; i < 5; ++i)
		{
			PED::CLEAR_PED_DAMAGE_DECAL_BY_ZONE(ped, i, "ALL");
		}
	}

	void delete_selected_entity(Entity& ent, bool force)
	{
		if (!ENTITY::DOES_ENTITY_EXIST(ent))
		{
			ent = NULL;
			return;
		}

		if (force && !util::natives::forcefully_take_control(ent))
		{
			return notify(Error, "Failed to take control of entity before deleting");
		}

		if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
		{
			for (auto obj : pools::get_all_props_array())
			{
				if (!ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(ent, obj))
					continue;

				ENTITY::DELETE_ENTITY(&obj);
			}
			for (auto veh : pools::get_all_vehicles_array())
			{
				if (ent == veh || !ENTITY::IS_ENTITY_ATTACHED_TO_ENTITY(ent, veh))
					continue;

				ENTITY::DELETE_ENTITY(&veh);
			}
		}

		ENTITY::DETACH_ENTITY(ent, 1, 1);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, 7000.f, 7000.f, 15.f, FALSE, FALSE, FALSE);

		if (!ENTITY::IS_ENTITY_A_MISSION_ENTITY(ent))
		{
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ent, TRUE, TRUE);
		}

		if (ENTITY::IS_ENTITY_A_PED(ent))
		{
			PED::DELETE_PED(&ent);
		}
		else if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
		{
			VEHICLE::DELETE_VEHICLE(&ent);
		}
		else if (ENTITY::IS_ENTITY_AN_OBJECT(ent))
		{
			OBJECT::DELETE_OBJECT(&ent);
		}
		else
		{
			ENTITY::DELETE_ENTITY(&ent);
		}
	}

	std::vector<Entity> get_entities(bool vehicles, bool peds, bool props, bool pickups, bool include_self_veh, bool include_self_ped)
	{
		std::vector<Entity> target_entities;

		if (vehicles)
		{
			for (const auto vehicle : pools::get_all_vehicles_array())
			{
				if (!include_self_veh && vehicle == self::vehicle)
					continue;

				target_entities.push_back(vehicle);
			}
		}

		if (peds)
		{
			for (const auto ped : pools::get_all_peds_array())
			{
				if (!include_self_ped && ped == self::ped)
					continue;

				target_entities.push_back(ped);
			}
		}

		if (props)
		{
			auto props_arr = pools::get_all_props_array();
			target_entities.insert(target_entities.end(), props_arr.begin(), props_arr.end());
		}

		if (pickups)
		{
			auto pickups_arr = pools::get_all_pickups_array();
			target_entities.insert(target_entities.end(), pickups_arr.begin(), pickups_arr.end());
		}
		return target_entities;
	}
}
