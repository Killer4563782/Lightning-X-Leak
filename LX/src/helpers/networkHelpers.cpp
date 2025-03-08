#include "networkHelpers.h"
#include "commands/manager.hpp"
#include "util/globals.h"
#include <rage/gta/net_game_event.hpp>
#include <util/packet.hpp>
#include "entityHelpers.h"
#include "vehicleHelpers.h"
#include "util/patches.hpp"
#include "util/protections.h"
#include "util/util.h"

namespace network_helpers
{
	Player get_interior_from_player(const Player player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player].CurrentInteriorIndex;
	}

	bool is_in_cutscene(const player_ptr& player)
	{
		return player->is_valid() && player->get_player_info() && player->get_player_info()->m_game_state == eGameState::InMPCutscene;
	}

	bool is_in_interior(const player_ptr& player)
	{
		return player->is_valid() && get_interior_from_player(player->id()) != 0;
	}

	bool is_in_godmode(const player_ptr& player)
	{
		const auto ped = player->get_ped();
		return ped && ped->m_damage_bits & God;
	}

	bool is_invisible(const player_ptr& player)
	{
		if (!player->get_ped())
			return false;

		if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].IsInvisible)
			return true;

		if ((player->get_current_vehicle() && player->get_current_vehicle()->m_driver == player->get_ped()) ||
			PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
			return false;

		return (player->get_ped()->m_flags & static_cast<int>(rage::fwEntity::EntityFlags::IS_VISIBLE)) == 0;
	}

	bool is_hidden_from_player_list(const player_ptr& player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].CayoPericoFlags & 1;
	}

	bool is_using_rc_vehicle(const player_ptr& player)
	{
		if (scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[player->id()].PropertyData.PAD_0365 & 1 << 29)
			return true; // bandito

		if (scr_globals::gpbd_fm_1.as<GPBD_FM*>()->Entries[player->id()].PropertyData.ArcadeData.AppearanceBitset2 & 1
			<< 16)
			return true; // tank

		return false;
	}

	bool is_using_orbital_cannon(const player_ptr& player)
	{
		return scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].OrbitalBitset.IsSet(
			eOrbitalBitset::kOrbitalCannonActive);
	}

	void set_player_model(const rage::joaat_t model)
	{
		if (!util::natives::request_model(model))
		{
			notify(Error, 5s, "Failed to load model!");
			return;
		}
		if (!STREAMING::IS_MODEL_A_PED(model) || protections::is_crash_ped(model))
		{
			notify(Error, "Invalid input model!");
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
			return;
		}
		PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
		script::get_current()->yield();
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
		PED::SET_PED_DEFAULT_COMPONENT_VARIATION(self::ped);
	}
}

namespace mechanic
{
	Vehicle get_personal_vehicle()
	{
		return *scr_globals::freemode_global.at(299).as<Vehicle*>();
	}
}

namespace crashes
{
	void ped_crash(const player_ptr& player)
	{
		static bool running = false;
		g_fiber_pool->queue_job([&]
		{
			if (running)
				return;

			running = true;
			"anti_crash_cam"_LC->enable();
			const auto pos = player->get_ped()->get_position();
			while (!STREAMING::HAS_MODEL_LOADED("slod_human"_joaat))
			{
				STREAMING::REQUEST_MODEL("slod_human"_joaat);
				script::get_current()->yield();
			}
			Ped crash_ped{};
			execute_as_script(services::is_online() ? "freemode"_joaat : "main_persistent"_joaat, [&]
			{
				crash_ped = PED::CREATE_PED(PED_TYPE_CIVMALE, "slod_human"_joaat, pos->x, pos->y, pos->z,
				                            ENTITY::GET_ENTITY_HEADING(self::ped), TRUE, FALSE);
			});
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED("slod_human"_joaat);
			script::get_current()->yield(500ms);
			entity::delete_selected_entity(crash_ped, true);
			"anti_crash_cam"_LC->disable();
			running = false;
		});
	}

	rage::fwArchetype* get_model_info(const rage::joaat_t hash)
	{
		uint32_t index = 0xFFFF;
		return g_pointers->g_get_model_info(hash, &index);
	}

	void crash_1(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			auto pos = player->get_pos();
			constexpr auto hash = "adder"_joaat;
			constexpr auto hash2 = "btype3"_joaat;
			constexpr auto hash3 = "dinghy"_joaat;

			if (const auto model_info = get_model_info(hash); model_info != nullptr)
			{
				auto modify_model_info = [&](const int value) {
					*reinterpret_cast<std::int32_t*>(model_info + 0x340) = value;
				};

				auto spawn_and_delete_vehicle = [&](const std::uint32_t vehicle_hash, const int model_value) {
					modify_model_info(model_value);
					auto vehicle = vehicle::spawn_vehicle(vehicle_hash, { pos->x, pos->y, pos->z }, 0.f, true, false);
					script::get_current()->yield(1s);
					entity::delete_selected_entity(vehicle, true);
				};

				spawn_and_delete_vehicle(hash, 8);
				spawn_and_delete_vehicle(hash2, 11);
				spawn_and_delete_vehicle(hash3, 1);
			}
		});
	}

	void crash_2(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			if (!util::natives::request_model("a_m_m_hasjew_01"_joaat))
			{
				return notify(Error, 5s, "Failed to load crash model!");
			}
			Ped crash_ped = PED::CREATE_PED(PED_TYPE_MISSION, "a_m_m_hasjew_01"_joaat, player->get_pos()->x, player->get_pos()->y, player->get_pos()->z, 0.f, true, false);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED("a_m_m_hasjew_01"_joaat);
			entity::delete_selected_entity(crash_ped, true);
		});
	}

	void crash_3(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			constexpr rage::joaat_t ruiner = "ruiner2"_joaat;
			if (!util::natives::request_model(ruiner))
			{
				return notify(Error, 5s, "Failed to load crash model!");
			}
			Vehicle veh = vehicle::spawn_vehicle(ruiner, { player->get_pos()->x, player->get_pos()->y, player->get_pos()->z + 300.f }, 0.f, true, false);
			VEHICLE::VEHICLE_SET_PARACHUTE_MODEL_OVERRIDE(veh, "apa_mp_apa_crashed_usaf_01a"_joaat);
			PED::SET_PED_INTO_VEHICLE(self::ped, veh, -1);
			for (auto k = 0; k < 11; k++)
			{
				PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, 353, 1.f);
				script::get_current()->yield(350ms);
			}
			script::get_current()->yield(1s);
			TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
			entity::delete_selected_entity(veh, true);
		});
	}

	void crash_4(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			constexpr rage::joaat_t hash = "sentinel"_joaat;
			if (!util::natives::request_model(hash))
			{
				return notify(Error, 5s, "Failed to load model!");
			}
			Vehicle veh = vehicle::spawn_vehicle(hash, *player->get_pos(), 0.f, true, false);
			const int net_id = NETWORK::VEH_TO_NET(veh);
			NETWORK::SET_NETWORK_ID_ALWAYS_EXISTS_FOR_PLAYER(net_id, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()), TRUE);
			NETWORK::SET_NETWORK_ID_CAN_MIGRATE(net_id, FALSE);
			VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
			VEHICLE::SET_VEHICLE_MOD(veh, 34, 3, FALSE);
			VEHICLE::TOGGLE_VEHICLE_MOD(veh, 34, TRUE);
			VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);
			VEHICLE::SET_VEHICLE_MOD(veh, 0, 0, FALSE);
			VEHICLE::TOGGLE_VEHICLE_MOD(veh, 0, TRUE);
			script::get_current()->yield(8s);
			entity::delete_selected_entity(veh, true);
		});
	}

	void crash_5(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			if (player->get_current_vehicle())
			{
				auto converted_ped = g_pointers->g_pointer_to_handle(player->get_ped());
				auto converted_vehicle = g_pointers->g_pointer_to_handle(player->get_current_vehicle());
				if (converted_ped != 0 && converted_vehicle != 0)
				{
					for (int i = 15; i <= 18; i++)
					{
						TASK::TASK_VEHICLE_TEMP_ACTION(converted_ped, converted_vehicle, i, 100);
					}
					return;
				}
			}
			notify(Error, "Player must be in a vehicle!");
		});
	}

	void crash_6(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			constexpr Hash obj_hash = "prop_fragtest_cnst_04"_joaat;
			while (!STREAMING::HAS_MODEL_LOADED(obj_hash))
			{
				STREAMING::REQUEST_MODEL(obj_hash);
				script::get_current()->yield();
			}
			Object ob = world_model::spawn(obj_hash, *player->get_pos(), true);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(ob);
			for (int j = 1; j < 2; j++)
			{
				OBJECT::BREAK_OBJECT_FRAGMENT_CHILD(ob, j, false);
			}
			script::get_current()->yield(1s);
			entity::delete_selected_entity(ob, true);
		});
	}

	std::vector invalid_objects_for_crash = {
		"prop_toilet_01"_joaat
		"h4_prop_bush_buddleia_low_01"_joaat,
		"prop_weed_01"_joaat,
	};
	void crash_7(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			if (math::distance_between_vectors(*player->get_pos(), *self::local_ped->get_position()) < 10.f)
			{
				return notify(Error, "Player is too close to you!");
			}
			for (const auto& obj_hash : invalid_objects_for_crash)
			{
				while (!STREAMING::HAS_MODEL_LOADED(obj_hash))
				{
					STREAMING::REQUEST_MODEL(obj_hash);
					script::get_current()->yield();
				}
				Object obj = world_model::spawn(obj_hash, *player->get_pos(), true);
				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(obj_hash);
				NETWORK::SET_ENTITY_LOCALLY_INVISIBLE(obj);
				entity::delete_selected_entity(obj, true);
			}
		});
	}

	void crash_8(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			"anti_crash_cam"_LC->enable();
			constexpr auto vehicle_hash = "sentinel"_joaat;
			if (!util::natives::request_model(vehicle_hash))
			{
				return notify(Error, 5s, "Failed to load model!");
			}
			Vehicle vehicle = vehicle::spawn_vehicle(vehicle_hash, *player->get_pos(), 0.f, true, false);
			VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 34, 3, FALSE);
			VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 34, TRUE);
			script::get_current()->yield(1s);
			VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
			VEHICLE::SET_VEHICLE_MOD(vehicle, 0, 0, FALSE);
			VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, 0, TRUE);
			script::get_current()->yield(1s);
			entity::delete_selected_entity(vehicle, true);
			"anti_crash_cam"_LC->disable();
		});
	}

	void crash_9(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			const auto hash = rage::joaat("u_m_y_pogo_01");
			if (!util::natives::request_model(hash))
			{
				return notify(Error, 5s, "Failed to load model!");
			}
			Ped crash_ped = PED::CREATE_PED(PED_TYPE_CIVMALE, hash, player->get_pos()->x, player->get_pos()->y, player->get_pos()->z, 0.f, TRUE, FALSE);
			script::get_current()->yield(2s);
			entity::delete_selected_entity(crash_ped, true);
		});
	}
}