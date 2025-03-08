#include "vehicleHelpers.h"

#include "commands/manager.hpp"
#include "entityHelpers.h"
#include "inputs.h"
#include "fiber/fiber_pool.hpp"
#include "fiber/script.hpp"
#include "rage/gta/vehicle_values.hpp"
#include "util/util.h"
#include <util/globals.h>

namespace vehicle
{
	void set_mp_bitset(const Vehicle veh)
	{
		DECORATOR::DECOR_SET_INT(veh, property_name_vehicle_spawn_in_online, 0);
		const auto network_id = NETWORK::VEH_TO_NET(veh);
		self::spawned_vehicles.insert(network_id);
		if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
			NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(network_id, TRUE);
		VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
		ENTITY::SET_ENTITY_SHOULD_FREEZE_WAITING_ON_COLLISION(veh, TRUE);
	}

	bool into_vehicle(const Vehicle veh)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh))
		{
			notify(Error, "Entity is not a vehicle!");
			return false;
		}

		int seat_index = -1;
		if (!VEHICLE::IS_VEHICLE_SEAT_FREE(veh, seat_index, true))
		{
			if (!VEHICLE::IS_VEHICLE_SEAT_FREE(veh, -2, true))
			{
				notify(Error, "Vehicle is full!");
				return false;
			}
			seat_index = -2;
		}

		"spectate"_LC->enable();
		const auto location = ENTITY::GET_ENTITY_COORDS(veh, TRUE);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(self::ped, location.x, location.y, location.z + 2.f, FALSE, FALSE, FALSE);
		script::get_current()->yield();
		PED::SET_PED_INTO_VEHICLE(self::ped, veh, seat_index);
		"spectate"_LC->disable();

		return true;
	}

	void max_vehicle(Vehicle veh, bool request_control)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh))
		{
			return;
		}

		if (request_control && !util::natives::request_control(veh))
		{
			return notify(Error, "Failed to take control of vehicle!");
		}

		VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TURBO, TRUE);
		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_TYRE_SMOKE, TRUE);
		VEHICLE::TOGGLE_VEHICLE_MOD(veh, MOD_XENON_LIGHTS, TRUE);
		VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, 1);
		VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, false);

		for (int slot = MOD_SPOILER; slot <= MOD_PLATES; slot++)
		{
			if (slot == MOD_LIVERY)
			{
				continue;
			}

			if (const int count = VEHICLE::GET_NUM_VEHICLE_MODS(veh, slot); count > 0)
			{
				int selected_mod = -1;

				for (int mod = count - 1; mod >= -1; mod--)
				{
					if (VEHICLE::IS_VEHICLE_MOD_GEN9_EXCLUSIVE(veh, slot, mod))
					{
						continue;
					}

					selected_mod = mod;
					break;
				}

				if (selected_mod != -1)
				{
					VEHICLE::SET_VEHICLE_MOD(veh, slot, selected_mod, TRUE);
				}
			}
		}
	}

	void max_vehicle_performance(Vehicle veh)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(veh))
		{
			return;
		}

		if (!util::natives::request_control(veh))
		{
			return notify(Error, "Failed to take control of vehicle!");
		}

		eVehicleModTypes performance_mods[] = {MOD_ENGINE, MOD_BRAKES, MOD_TRANSMISSION, MOD_SUSPENSION, MOD_ARMOR, MOD_NITROUS, MOD_TURBO};
		VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

		for (const auto mod_slot : performance_mods)
		{
			if (mod_slot != MOD_NITROUS && mod_slot != MOD_TURBO)
				VEHICLE::SET_VEHICLE_MOD(veh, mod_slot, VEHICLE::GET_NUM_VEHICLE_MODS(veh, mod_slot) - 1, TRUE);
			else
				VEHICLE::TOGGLE_VEHICLE_MOD(veh, mod_slot, TRUE);
		}
		
	}

	void downgrade(Vehicle vehicle)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			return;
		}

		if (!util::natives::request_control(vehicle))
		{
			return notify(Error, "Failed to take control of vehicle!");
		}

		VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
		for (int i = 0; i < 50; i++)
		{
			VEHICLE::REMOVE_VEHICLE_MOD(vehicle, i);
		}
	}

	bool delete_vehicle(Vehicle vehicle)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			return false;
		}

		if (!util::natives::request_control(vehicle))
		{
			notify(Error, "Failed to take control of vehicle!");
			return false;
		}

		if (self::vehicle == vehicle)
		{
			TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
		}

		entity::delete_selected_entity(vehicle, true);
		return true;
	}

	bool repair_vehicle(Vehicle vehicle)
	{
		if (!ENTITY::IS_ENTITY_A_VEHICLE(vehicle))
		{
			return false;
		}

		if (!util::natives::request_control(vehicle))
		{
			notify(Error, "Failed to take control of vehicle!");
			return false;
		}

		VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(vehicle);
		VEHICLE::SET_VEHICLE_FIXED(vehicle);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.f);

		return true;
	}

	Vehicle spawn_vehicle(const Hash hash, const Vector3& location, const float heading, const bool is_networked, const bool script_veh)
	{
	    if (!STREAMING::IS_MODEL_A_VEHICLE(hash) || !util::natives::request_model(hash))
	    {
	        notify(Error, "Failed to find vehicle model!");
	        return NULL;
	    }

	    float previous_vehicle_speed = 0.0f;
	    if (seamless_re_enter_vehicle && PED::IS_PED_IN_ANY_VEHICLE(self::ped, TRUE))
	    {
	        auto previous_vehicle = PED::GET_VEHICLE_PED_IS_IN(self::ped, FALSE);
	        previous_vehicle_speed = ENTITY::GET_ENTITY_SPEED(previous_vehicle);
	        TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
	        entity::delete_selected_entity(previous_vehicle);
	    }

		Vehicle new_spawned_vehicle{};
		execute_as_script(find_script_thread(*g_pointers->g_is_session_active ? "freemode"_joaat : "main_persistent"_joaat), [&]
		{
			new_spawned_vehicle = VEHICLE::CREATE_VEHICLE(hash, location.x, location.y, location.z, heading, is_networked, script_veh, FALSE);
		});

	    STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

	    if (is_networked && services::is_online())
	    {
	        set_mp_bitset(new_spawned_vehicle);
	    }

	    VEHICLE::SET_VEHICLE_ENGINE_ON(new_spawned_vehicle, TRUE, TRUE, TRUE);
	    ENTITY::SET_ENTITY_HEADING(new_spawned_vehicle, heading);

	    if (ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(new_spawned_vehicle) <= 1.5f)
	    {
	        VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(new_spawned_vehicle, 5.0f);
	    }

	    if (set_ped_in_vehicle_when_spawned)
	    {
	        PED::SET_PED_INTO_VEHICLE(self::ped, new_spawned_vehicle, -1);
	    }

	    if (seamless_re_enter_vehicle && PED::IS_PED_IN_ANY_VEHICLE(self::ped, 0))
	    {
	        VEHICLE::SET_VEHICLE_FORWARD_SPEED_XY(new_spawned_vehicle, previous_vehicle_speed);
	    }

	    if (VEHICLE::IS_THIS_MODEL_A_HELI(hash))
	    {
	        VEHICLE::SET_HELI_BLADES_FULL_SPEED(new_spawned_vehicle);
	    }

	    if (VEHICLE::IS_THIS_MODEL_A_PLANE(hash))
	    {
	        VEHICLE::SET_VEHICLE_KEEP_ENGINE_ON_WHEN_ABANDONED(new_spawned_vehicle, TRUE);
	    }

	    if (set_vehicle_maxed_when_spawned)
	    {
	        max_vehicle(new_spawned_vehicle, false);
	    }

	    return new_spawned_vehicle;
	}

	void spawn_vehicle_by_name()
	{
		get_input("Vehicle Model Name", {}, text_input::InputFlags_Dont_Add_History, [](const std::string& input)
		{
			g_fiber_pool->queue_job([input]
			{
				spawn_vehicle(rage::joaat(input), self::position, ENTITY::GET_ENTITY_HEADING(self::ped), true, false);
			});
		});
	}

	Vehicle spawn_vehicle_for_teleporting(const Hash hash, const Vector3& location, const float heading, bool is_networked, const bool script_veh)
	{
		if (!util::natives::request_model(hash))
		{
			return NULL;
		}
		const auto veh = VEHICLE::CREATE_VEHICLE(hash, location.x, location.y, location.z, heading, is_networked, script_veh, false);

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

		if (is_networked && services::is_online())
		{
			set_mp_bitset(veh);
		}

		return veh;
	}

	void drive_to_waypoint()
	{
		g_fiber_pool->queue_job([&]
		{
			util::blip way_point{ BlipSpriteWaypoint, util::eBlipHandleType::first };
			const auto vehicle = self::local_ped->m_vehicle;
			const auto converted = g_pointers->g_pointer_to_handle(vehicle);
			if (!vehicle)
			{
				notify(Error, "You are not in a vehicle!");
				return;
			}
			if (!way_point.exists())
			{
				notify(Error, "No waypoint set!");
				return;
			}
			if (!util::natives::request_control(converted))
			{
				notify(Error, "Failed to take control of vehicle!");
				return;
			}
			TASK::TASK_VEHICLE_DRIVE_TO_COORD(self::ped, converted, way_point.coords().x, way_point.coords().y,
				way_point.coords().z,
				VEHICLE::GET_VEHICLE_ESTIMATED_MAX_SPEED(converted),
				DrivingStyleAvoidTrafficExtremely, ENTITY::GET_ENTITY_MODEL(converted),
				4, 10.f, 50.f);
		});
	}

	void cancel_driving()
	{
		g_fiber_pool->queue_job([&]
		{
			TASK::CLEAR_PED_TASKS(self::ped);
		});
	}

	Vehicle get_personal_vehicle()
	{
		return *scr_globals::freemode_global.at(301).as<Vehicle*>();
	}

	CVehicle* get_personal_cvehicle()
	{
		Vehicle personal_vehicle = get_personal_vehicle();
		if (personal_vehicle != -1 && ENTITY::DOES_ENTITY_EXIST(personal_vehicle))
		{
			return reinterpret_cast<CVehicle*>(g_pointers->g_handle_to_pointer(personal_vehicle));
		}

		return nullptr;
	}
}
