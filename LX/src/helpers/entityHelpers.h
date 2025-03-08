#pragma once
#include "memory/pointers.h"

namespace base_pointers
{
	class GenericPool;
	class VehiclePool;
}

namespace entity
{
	void cage_ped(Ped ped);
	void clean_ped(Ped ped);
	void delete_selected_entity(Entity& ent, bool force = false);
	std::vector<Entity> get_entities(bool vehicles, bool peds, bool props, bool pickups, bool include_self_veh, bool include_self_ped = false);
}

namespace pools
{
	inline generic_pool* get_all_peds()
	{
		if (auto ptr = g_pointers->g_ped_pool; ptr)
			return *ptr;

		return nullptr;
	}

	inline vehicle_pool* get_all_vehicles()
	{
		if (auto ptr = g_pointers->g_vehicle_pool; ptr && *ptr)
			return **ptr;

		return nullptr;
	}

	inline generic_pool* get_all_props()
	{
		if (auto ptr = g_pointers->g_prop_pool; ptr)
			return *ptr;

		return nullptr;
	}

	inline generic_pool* get_all_pickups()
	{
		if (auto ptr = g_pointers->g_pickup_pool; ptr)
			return *ptr;

		return nullptr;
	}

	inline std::vector<Entity> get_all_peds_array()
	{
		if (auto peds = get_all_peds())
			return peds->to_array();

		return {};
	}

	inline std::vector<Entity> get_all_vehicles_array()
	{
		if (auto vehicles = get_all_vehicles())
			return vehicles->to_array();

		return {};
	}

	inline std::vector<Entity> get_all_props_array()
	{
		if (auto props = get_all_props())
			return props->to_array();

		return {};
	}

	inline std::vector<Entity> get_all_pickups_array()
	{
		if (auto pickups = get_all_pickups())
			return pickups->to_array();

		return {};
	}
};
