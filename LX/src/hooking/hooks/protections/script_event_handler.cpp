#include "commands/math.h"
#include "commands/settings.h"
#include "helpers/networkHelpers.h"
#include "hooking/hooking.hpp"
#include "rage/util.h"
#include "rage/gta/net_game_event.hpp"
#include "services/players/player_manager.hpp"
#include "services/reactions/reactions.h"
#include "util/globals.h"
#include "util/util.h"

bool is_player_driver_of_local_vehicle(const player_ptr& sender)
{
	if (!sender || !sender->get_current_vehicle() || !self::local_ped || !self::local_ped->m_vehicle)
		return false;

	return self::local_ped->m_vehicle->m_driver == sender->get_ped();
}

bool is_player_our_goon(Player sender_id)
{
	auto& boss_goon = scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::player].BossGoon;

	if (boss_goon.Boss != self::player)
		return false;

	for (int i = 0; i < boss_goon.Goons.Size; i++)
	{
		if (boss_goon.Goons[i] == sender_id)
		{
			return true;
		}
	}

	return false;
}

bool is_player_our_boss(const player_ptr& sender)
{
	return sender->id() == scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::player].BossGoon.Boss;
}

bool hooks::scripted_game_event(CScriptedGameEvent* scripted_game_event, CNetGamePlayer* player)
{
	const auto args = scripted_game_event->m_args;
	const auto args_count = scripted_game_event->m_args_size / 8;

	const auto hash = static_cast<eScriptEvents>(args[0]);
	const auto player_name = player->get_name();
	const auto event_name = scripted_game_event->get_name();

	auto plyr = g_player_manager->get_by_id(player->m_player_id);


#ifdef _DEBUG
	if (commands::settings::debug::log_script_events)
	{
		std::string script_args;
		for (size_t i = 0; i < args_count; i++)
		{
			script_args += "\t" + std::to_string(i) + ": " + std::to_string(args[i]) + "\n";
		}

		if (event_name)
			LOG_DEBUG("Script Event {} | ID: {} | Player: {} | Args({}):\n{}", event_name, args[0], plyr->get_name(), args_count, script_args.c_str());
		else
			LOG_DEBUG("Script Event ID: {} | Player: {} | Args({}):\n{}", args[0], plyr->get_name(), args_count, script_args.c_str());
	}
#endif

	// detect pasted menus setting args[1] to something other than PLAYER_ID()
	if (*reinterpret_cast<int*>(&args[1]) != player->m_player_id && player->m_player_id != -1)
	{
		LOG_DEBUG("Script Event: TSE Sender Mismatch\nHash: %d\nSender: %d", static_cast<int>(args[0]), args[1]);

		return sender_mismatch.process(plyr, "Event (Sender Mismatch)");
	}

	for (constexpr int64_t se_crashes[6] = { -3971681992, 879177392, -904555865, -1990614866, 697566862, 548471420 };
		 const auto & se_crash : se_crashes)
	{
		if (args[0] == se_crash)
			return crash.process(plyr, "Crash S3");
	}

	// ReSharper disable once CppIncompleteSwitchStatement
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	switch (hash)
	{
		case eScriptEvents::ScriptEventSetBountyOnPlayer:
		if (args[3] == self::player)
		{
			return bounty.process(plyr, "Bounty");
		}
		break;
		case eScriptEvents::ScriptEventGbBossStartedUsingSctv:
		if (player->m_player_id != scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::player].BossGoon.Boss)
		{
			return ceo_kick.process(plyr, "Ceo Kick");
		}
		break;
		case eScriptEvents::ScriptEventBeenPaidGoonCash:
		if (player->m_player_id != scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[self::player].BossGoon.Boss)
		{
			return ceo_money_not_boss.process(plyr, "Ceo Money (Not my Boss)");
		}
		break;
		case eScriptEvents::ScriptEventRemoveWantedLevel:
		if (!is_player_driver_of_local_vehicle(plyr))
		{
			return clear_wanted_level.process(plyr, "Clear Wanted Level");
		}
		break;
		case eScriptEvents::ScriptEventFreemodeContentGiveWantedLevel: return alter_wanted_level.process(plyr, "Give Wanted Level");
		case eScriptEvents::ScriptEventLaunchSyncedInteraction: return crash.process(plyr, "Crash S5");
		case eScriptEvents::ScriptEventIslandBackupHeliLaunch:
		if (args[3] > 32) // actual crash condition is if args[2] is above 255
		{
			return crash.process(plyr, "Crash S7");
		}
		break;
		case eScriptEvents::ScriptEventRequestToSpawnVehicle:
		{
			if (isnan(*reinterpret_cast<float*>(&args[4])) || isnan(*reinterpret_cast<float*>(&args[5])))
			{
				return crash.process(plyr, "Crash S4");
			}
			if (args[3] == -4640169 && args[7] == -36565476 && args[8] == -53105203)
			{
				return crash.process(plyr, "Crash S6");
			}
			break;
		}
		case eScriptEvents::ScriptEventTickerMessage:
		{
			switch (static_cast<eScriptEvents>(args[3]))
			{
				case eScriptEvents::TickerEventTeamCashBanked: // never used
				case eScriptEvents::TickerEventTeamCashRemoved:
				case eScriptEvents::TickerEventTeamCashStolen: return fake_deposit.process(plyr, "Fake Deposit");
				case eScriptEvents::TickerEventOvertakeInChallenge: return crash.process(plyr, "Crash S9"); // this isn't used by the game 
				case eScriptEvents::TickerEventSalvageCheckpointCollected:
				{
					if (!find_script_thread("gb_salvage"_joaat))
					{
						return crash.process(plyr, "Crash S1");
					}
					break;
				}
				default:;
			}
			break;
		}
		case eScriptEvents::ScriptEventBossShouldLaunchWvm: return force_mission.process(plyr, "Force Mission");
		case eScriptEvents::ScriptEventCollectibleCollected: return give_collectible.process(plyr, "Give Collectible");
		case eScriptEvents::ScriptEventFmDeliverableDelivery: return gta_banner.process(plyr, "GTA Banner");
		case eScriptEvents::ScriptEventGroupWarp:
		if (args[4] <= 32 && !is_player_our_boss(plyr))
		{
			for (int i = 0; i < 32; i++)
			{
				if (args[5 + i] == NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(self::player))
				{
					return mc_teleport.process(plyr, "MC Teleport (Not my Boss)");
				}
			}
		}
		else if (args[4] > 32)
		{
			return crash.process(plyr, "Crash S8");
		}
		break;
		case eScriptEvents::ScriptEventCarInsurance:
		if (args[3] == self::player)
		{
			return personal_vehicle_destroyed.process(plyr, "Personal Vehicle Destroy");
		}
		break;
		case eScriptEvents::ScriptEventOffTheRadar:
		if (!is_player_our_boss(plyr) && !is_player_driver_of_local_vehicle(plyr))
		{
			return remote_off_radar.process(plyr, "Remote Off Radar (Not my Boss)");
		}
		break;
		case eScriptEvents::ScriptEventGeneral:
		if (static_cast<eScriptEvents>(args[3]) == eScriptEvents::GeneralEventHeistPreplanExitGuestMode && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
		{
			return rotate_cam.process(plyr, "Rotate Camera");
		}
		break;
		case eScriptEvents::ScriptEventInviteToHeistIslandBeachParty:
		if (args[4] == 0)
		{
			return send_to_location.process(plyr, "Send To Cayo Perico");
		}
		break;
		case eScriptEvents::ScriptEventCasinoPlayHeistCutscene:
		if (!is_player_our_boss(plyr))
		{
			return send_to_cutscene.process(plyr, "Send To Cutscene");
		}
		break;
		case eScriptEvents::ScriptEventHeistIslandWarpTransition:
		{
			if (is_player_our_boss(plyr))
				break;

			bool known_location = false;

			if (args[3] == 0 && args[4] == 0)
			{
				if (args[5] == 4 && args[6] == 0)
				{
					known_location = true;

					if (const auto block = send_to_location.process(plyr, "Send To Location"))
					{
						return block;
					}
				}
				else if ((args[5] == 3 || args[5] == 4) && args[6] == 1)
				{
					known_location = true;

					if (const auto block = send_to_location.process(plyr, "Send To Location"))
					{
						return block;
					}
				}
			}

			if (!known_location)
			{
				return freeze.process(plyr, "Freeze S2");
			}
			break;
		}
		case eScriptEvents::ScriptEventInviteToApartment:
		{
			if (plyr->m_invites_rate_limit.process())
				return apartment_invite.process(plyr, "Apartment Invite Spam");

			break;
		}
		case eScriptEvents::TickerEventSpectatorStart: return spectate.process(plyr, "Spectate");
		case eScriptEvents::ScriptEventInviteNearbyPlayersIntoApartment:
		{
			if (force_teleport.m_block)
			{
				return force_teleport.process(plyr, "Force Teleport");
			}
			break;
		}
		case eScriptEvents::ScriptEventGivePlayerCashFromLastJob: return transaction_error.process(plyr, "Transaction Error");
		case static_cast<eScriptEvents>(3791641330):
		{
			if (args[7] == 1)
				return vehicle_kick.process(plyr, "Vehicle Kick");

			break;
		}
		case eScriptEvents::ScriptEventLeaveVehicle: return vehicle_kick.process(plyr, "Vehicle Kick");
		case eScriptEvents::ScriptEventBailMeForSctv: return network_bail.process(plyr, "Network Bail"); //session::add_infraction(plyr, Infraction::TRIED_KICK_PLAYER);
		case eScriptEvents::ScriptEventWarpToQuickTravelDestination:
		if (!is_player_driver_of_local_vehicle(plyr))
		{
			return send_to_location.process(plyr, "Teleport To Warehouse");
		}
		break;
		case eScriptEvents::ScriptEventGbNonBossChallengeRequest:
		{
			const auto activity = static_cast<eActivityType>(args[3]);
			if (start_activity.m_block)
			{
				if (activity == eActivityType::Survival || activity == eActivityType::Mission || activity == eActivityType::Deathmatch || activity == eActivityType::BaseJump || activity == eActivityType::Race)
				{
					return freeze.process(plyr, "Freeze ST");
				}
				if (activity == eActivityType::Darts)
				{
					return start_activity.process(plyr, "Start Activity");
				}
				if (activity == eActivityType::PilotSchool)
				{
					return start_activity.process(plyr, "Start Activity");
				}
				if (activity == eActivityType::ImpromptuDeathmatch)
				{
					return start_activity.process(plyr, "Start Activity");
				}
				if (activity == eActivityType::DefendSpecialCargo || activity == eActivityType::GunrunningDefend || activity == eActivityType::BikerDefend || args[3] == 238)
				{
					return trigger_business_raid.process(plyr, "Trigger Business Raid");
				}
			}
			else if (activity == eActivityType::Tennis)
			{
				return crash.process(plyr, "Crash S2");
			}

			if (!is_player_our_goon(player->m_player_id))
			{
				return start_activity.process(plyr, "Start Activity (Not my Boss)");
			}

			break;
		}
		case eScriptEvents::ScriptEventForcePlayerOntoMission:
		{
			static const std::set<int64_t> bad_script_ids = {
				17 /*AM_PI_MENU*/, 20 /*fm_intro*/, 212 /*golf_mp*/, 214 /*tennis_network_mp*/,
				215 /*Pilot_School_MP*/, 216 /*FM_Impromptu_DM_Controler*/, 217 /*fm_Bj_race_controler*/,
				218 /*fm_deathmatch_controler*/,
				221 /*FM_Race_Controler*/, 222 /*FM_Horde_Controler*/, 226 /*grid_arcade_cabinet*/,
				227 /*scroll_arcade_cabinet*/,
				229 /*road_arcade*/, 231 /*wizard_arcade*/, 235 /*ggsm_arcade*/, 236 /*puzzle*/, 238 /*SCTV*/
			};
			if (const auto script_id = args[3]; bad_script_ids.contains(script_id))
			{
				return start_activity.process(plyr, "Force Into Mission");
			}
			break;
		}
		case eScriptEvents::ScriptEventPlayersWarpInsideSimpleInterior:
		{
			if (auto interior = static_cast<int>(args[3]); interior < 0 || interior > 173) // the upper bound will change after an update
			{
				//if (auto plyr = get_player_by_id(player->m_player_id))
					//session::add_infraction(plyr, Infraction::TRIED_KICK_PLAYER);

				return kick.process(plyr, "Kick S8"); // null function kick
			}

			if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				break;

			if (!self::local_ped)
				break;

			if (is_player_our_boss(plyr))
				break;

			if (is_player_driver_of_local_vehicle(plyr))
				break;

			if (!plyr->get_ped() || math::distance_between_vectors(plyr->get_ped()->get_position(), self::local_ped->get_position()) > 75.0f)
			{
				// g.reactions.send_to_interior.process(plyr); false positives
				return true; // this is fine, the game will reject our false positives anyway
			}

			if (args[4] == -4294967296 && args[5] == -4294967296)
			{
				return send_to_location.process(plyr, "Send To Location");
			}
		}
		case eScriptEvents::ScriptEventPlayersWarpInsideSimpleInteriorOnDelivery:
		{
			return kick.process(plyr, "Kick S5");
		}
		case eScriptEvents::ScriptEventWarpBuysellScriptVehicle:
		{
			return kick.process(plyr, "Kick S3");
		}
		case eScriptEvents::ScriptEventPersonalVehicleStolen: return personal_vehicle_stolen.process(plyr, "Stolen Personal Vehicle");
		case eScriptEvents::ScriptEventTriggerExitAllFromSimpleInterior:
		if (scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].SimpleInteriorData.Owner != plyr->id())
		{
			return kick_from_interior.process(plyr, "Kick From Interior");
		}
		break;
		case eScriptEvents::ScriptEventGbTriggerDefendMission:
		{
			if (auto script = find_script_thread("freemode"_joaat); script && script->m_net_component)
			{
				if (auto host = reinterpret_cast<CGameScriptHandlerNetComponent*>(script->m_net_component)->m_host)
					if (host->m_net_game_player != player)
						return trigger_business_raid.process(plyr, "Trigger Business Raid");
			}

			return true;
		}
		case eScriptEvents::ScriptEventConfirmationLaunchMission:
		{
			// TODO: Breaks stuff
			if (util::network::get_script_host_net_game_player() != player)
			{
				return start_script.process(plyr, "Start Script");
			}
			break;
		}
		case eScriptEvents::ScriptEventGbBribePolice:
		{
			if (is_player_our_boss(plyr))
				break;

			if (args[4] == -4294967296 && args[5] == -4294967296)
				return alter_wanted_level.process(plyr, "Remote Bribe Authorities");

			break;
		}
		case static_cast<eScriptEvents>(-3494809739): //Force Camera Forward
		{
			return rotate_cam.process(plyr, "Remove Godmode");
		}
		case static_cast<eScriptEvents>(3652262909):
		{
			if (plyr->m_play_sound_rate_limit.process())
			{
				return sound_spam.process(plyr, "Sound Spam");
			}
			break;
		}
		case static_cast<eScriptEvents>(3041725881):
		{
			return freeze.process(plyr, "Freeze");
		}
		case static_cast<eScriptEvents>(-3345302900):
		{
			return passive_mode_block.process(plyr, "Passive Mode Block");
		}
		case static_cast<eScriptEvents>(2973309330):
		{
			if (args[4] == 4294967295 && args[6] == 115)
				return disable_driving_vehicle.process(plyr, "Disable Driving Vehicle");

			break;
		}
		case static_cast<eScriptEvents>(2158384489538):
		case static_cast<eScriptEvents>(-2844851317):
		{
			if (args[3] == -4294967033)
				return force_mission.process(plyr, "Force Mission");

			break;
		}
		case static_cast<eScriptEvents>(-2777415749):
		{
			return bounty.process(plyr, std::format("Bounty (${})", args[5]));
		}
		case static_cast<eScriptEvents>(-4237473601):
		{
			if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
				break;

			if (is_player_our_boss(plyr))
				break;

			if (args[5] == -4294967295 && args[6] == -4294967295)
				return alter_wanted_level.process(plyr, "Remote Off Radar");

			break;
		}
	}

	return false;
}