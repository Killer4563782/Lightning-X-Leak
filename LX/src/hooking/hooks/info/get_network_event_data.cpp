#include "commands/settings.h"
#include "fiber/fiber_pool.hpp"
#include "helpers/networkHelpers.h"
#include "hooking/hooking.hpp"
#include "rage/gta/net_game_event.hpp"
#include "services/api/api_service.hpp"
#include "services/online/online.h"
#include "services/players/player_manager.hpp"
#include "services/reactions/reactions.h"
#include "util/globals.h"
#include "util/util.h"

rage::CEventNetwork* hooks::get_network_event_data(int64_t unk, rage::CEventNetwork* net_event)
{
	switch (net_event->get_type())
	{
		case rage::eEventNetworkType::EVENT_NETWORK_PLAYER_COLLECTED_AMBIENT_PICKUP:
		{
			rage::sAmbientPickupCollectedData pickup_data;
			net_event->get_extra_information(&pickup_data, sizeof(pickup_data));

			if (const auto sender = g_pointers->g_handle_to_pointer(pickup_data.m_player_index); sender && sender->m_entity_type == 4 && reinterpret_cast<CPed*>(sender)->m_player_info)
			{
				if (const auto player = g_player_manager->get_by_host_token(reinterpret_cast<CPed*>(sender)->m_player_info->m_net_player_data.m_host_token))
				{
					for (const auto& rp_figures : util::rp_model_list)
					{
						if (pickup_data.m_pickup_amount > 99 && pickup_data.m_pickup_model == rp_figures)
						{
							if (kick.process(player, "Kick NL"))
							{
								pickup_data.m_pickup_amount = 69;
								pickup_data.m_pickup_collected = 69;
							}
						}
					}
				}
			}
			break;
		}
		case rage::eEventNetworkType::EVENT_NETWORK_REMOVED_FROM_SESSION_DUE_TO_STALL:
		case rage::eEventNetworkType::EVENT_NETWORK_REMOVED_FROM_SESSION_DUE_TO_COMPLAINTS:
		{
			if (commands::settings::session::rejoin_desync_kick && !NETWORK::NETWORK_IS_ACTIVITY_SESSION() && SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH("maintransition"_joaat) == 0 && !STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
			{
				g_fiber_pool->queue_job([&]
				{
					g_api_service->join_session(get_network()->m_last_joined_session.m_session_info);
				});
				notify(Success, 7s, "Rejoining back to desynced session");
			}
			else
			{
				notify(Warning, 7s, "You have been desync kicked");
			}
			break;
		}
		case rage::eEventNetworkType::EVENT_NETWORK_DAMAGE_ENTITY:
		{
			rage::sEntityDamagedData damage_data;
			net_event->get_extra_information(&damage_data, sizeof(damage_data));

			if (static_cast<uint32_t>(damage_data.m_weapon_used) == "WEAPON_STICKYBOMB"_joaat ||
				static_cast<uint32_t>(damage_data.m_weapon_used) == "VEHICLE_WEAPON_MINE_KINETIC_RC"_joaat ||
				static_cast<uint32_t>(damage_data.m_weapon_used) == "VEHICLE_WEAPON_MINE_EMP_RC"_joaat ||
				static_cast<uint32_t>(damage_data.m_weapon_used) == "VEHICLE_WEAPON_MINE_KINETIC"_joaat ||
				static_cast<uint32_t>(damage_data.m_weapon_used) == "VEHICLE_WEAPON_MINE_EMP"_joaat ||
				static_cast<uint32_t>(damage_data.m_weapon_used) == "VEHICLE_WEAPON_MINE_SPIKE"_joaat)
				break;

			if (const auto attacker = g_pointers->g_handle_to_pointer(damage_data.m_damager_index); attacker && attacker->
				m_entity_type == 4 && reinterpret_cast<CPed*>(attacker)->m_player_info)
			{
				if (const auto player = g_player_manager->get_by_host_token(
					reinterpret_cast<CPed*>(attacker)->m_player_info->m_net_player_data.m_host_token))
				{
					if (PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
					{
						if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].PlayerBlip.
							PlayerVehicleBlipType == eBlipType::SUBMARINE)
							break;

						if (network_helpers::is_using_rc_vehicle(player))
							break;
					}
					else
					{
						if (const auto vehicle = player->get_current_vehicle())
							if (const auto model_info = vehicle->m_model_info)
								if (static_cast<uint32_t>(model_info->m_hash) == "rcbandito"_joaat ||
									static_cast<uint32_t>(model_info->m_hash) == "minitank"_joaat ||
									static_cast<uint32_t>(model_info->m_hash) == "kosatka"_joaat)
									break;
					}

					if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
						break;

					if (!NETWORK::NETWORK_ARE_PLAYERS_IN_SAME_TUTORIAL_SESSION(self::player, player->id()))
						break;

					if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[player->id()].CurrentInteriorIndex
						!= 0)
						break;

					if (player->get_player_info() && player->get_player_info()->m_game_state ==
						eGameState::InMPCutscene)
						break;

					if (const auto victim = g_pointers->g_handle_to_pointer(damage_data.m_victim_index); victim && victim
						->m_entity_type == 4)
					{
						if (network_helpers::is_in_godmode(player))
						{
							//session::add_infraction(player, Infraction::ATTACKING_WITH_GODMODE);
						}

						if (network_helpers::is_invisible(player))
						{
							if (!reinterpret_cast<CPed*>(victim)->m_player_info)
								break;

							if (static_cast<uint32_t>(damage_data.m_weapon_used) == "WEAPON_EXPLOSION"_joaat ||
								static_cast<uint32_t>(damage_data.m_weapon_used) == "WEAPON_RAMMED_BY_CAR"_joaat ||
								static_cast<uint32_t>(damage_data.m_weapon_used) == "WEAPON_RUN_OVER_BY_CAR"_joaat)
								break;

							//session::add_infraction(player, Infraction::ATTACKING_WITH_INVISIBILITY);
						}

						if (network_helpers::is_hidden_from_player_list(player))
						{
							//session::add_infraction(player, Infraction::ATTACKING_WHEN_HIDDEN_FROM_PLAYER_LIST);
						}

						if (network_helpers::is_using_orbital_cannon(player))
						{
							//session::add_infraction(player, Infraction::SPOOFED_DATA);
						}
					}
				}
			}
			break;
		}
		default:;
	}

	return g_hooking->get_original<get_network_event_data>()(unk, net_event);
}
