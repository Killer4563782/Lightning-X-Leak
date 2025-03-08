#include <algorithm>

#include "commands/math.h"
#include "commands/settings.h"
#include "helpers/networkHelpers.h"
#include "hooking/hooking.hpp"
#include "memory/pointers.h"
#include "rage/gta/net_game_event.hpp"
#include "services/players/player_manager.hpp"
#include "services/reactions/reactions.h"

static void script_id_deserialize(CGameScriptId& id, rage::datBitBuffer& buffer)
{
	id.m_hash = buffer.Read<uint32_t>(32);
	id.m_timestamp = buffer.Read<uint32_t>(32);

	if (buffer.Read<bool>(1))
		id.m_position_hash = buffer.Read<uint32_t>(32);

	if (buffer.Read<bool>(1))
		id.m_instance_id = buffer.Read<int32_t>(8);
}

static bool is_valid_weapon(rage::joaat_t hash)
{
	auto& item_infos = g_pointers->g_weapon_info_manager->m_item_infos;
	return std::ranges::any_of(item_infos, [hash](CWeaponInfo* item)
	{
		return item && item->m_name == hash && item->GetClassId() == "cweaponinfo"_joaat;
	});
}

// Returns true if bad event
bool scan_weapon_damage_event(rage::netEventMgr* event_manager, CNetGamePlayer* player, CNetGamePlayer* target_player, int event_index, int event_handled_bitset, rage::datBitBuffer* buffer)
{
	uint8_t damageType;
	uint32_t weaponType; // weaponHash

	bool overrideDefaultDamage;
	bool hitEntityWeapon;
	bool hitWeaponAmmoAttachment;
	bool silenced;

	uint32_t damageFlags;
	bool hasActionResult;

	uint32_t actionResultName;
	uint16_t actionResultId;
	uint32_t f104;

	uint32_t weaponDamage;
	bool isNetTargetPos;

	rage::fvector3 localPos;

	bool f112;

	uint32_t damageTime;
	bool willKill;
	//uint32_t f120;
	bool hasVehicleData;

	uint16_t f112_1;

	uint16_t parentGlobalId; // Source entity?
	uint16_t hitGlobalId; // Target entity?

	uint8_t tyreIndex;
	uint8_t suspensionIndex;
	uint8_t hitComponent;

	bool f133;
	bool hasImpactDir;

	rage::fvector3 impactDir;

	damageType = buffer->Read<uint8_t>(2);
	weaponType = buffer->Read<uint32_t>(32);

	if (!is_valid_weapon(weaponType))
	{
		if (crash.process(g_player_manager->get_by_id(player->m_player_id), "Crash E8")) // Invalid Weapon Type 
		{
			g_pointers->g_send_event_ack(event_manager, player, target_player, event_index, event_handled_bitset);
			return true;
		}
	}

	overrideDefaultDamage = buffer->Read<uint8_t>(1);
	hitEntityWeapon = buffer->Read<uint8_t>(1);
	hitWeaponAmmoAttachment = buffer->Read<uint8_t>(1);
	silenced = buffer->Read<uint8_t>(1);

	damageFlags = buffer->Read<uint32_t>(24);
	// (damageFlags >> 1) & 1
	hasActionResult = buffer->Read<uint8_t>(1);

	if (hasActionResult)
	{
		actionResultName = buffer->Read<uint32_t>(32);
		actionResultId = buffer->Read<uint16_t>(16);
		f104 = buffer->Read<uint32_t>(32);
	}

	if (overrideDefaultDamage)
	{
		weaponDamage = buffer->Read<uint32_t>(17);
	}
	else
	{
		weaponDamage = 0;
	}

	bool _f92 = buffer->Read<uint8_t>(1);

	if (_f92)
	{
		buffer->Read<uint8_t>(4);
	}

	isNetTargetPos = buffer->Read<uint8_t>(1);

	if (isNetTargetPos)
	{
		localPos.x = buffer->ReadSignedFloat(16, 55.f); // divisor: 0x425C0000
		localPos.y = buffer->ReadSignedFloat(16, 55.f);
		localPos.z = buffer->ReadSignedFloat(16, 55.f);
	}

	if (damageType == 3)
	{
		damageTime = buffer->Read<uint32_t>(32);
		willKill = buffer->Read<uint8_t>(1);

		if (hasActionResult)
		{
			hitGlobalId = buffer->Read<uint16_t>(13);
		}
		else
		{
			hitGlobalId = 0;
		}

		f112 = buffer->Read<uint8_t>(1);

		if (!f112)
		{
			f112_1 = buffer->Read<uint16_t>(11);
		}
		else
		{
			f112_1 = buffer->Read<uint16_t>(16);
		}
	}
	else
	{
		parentGlobalId = buffer->Read<uint16_t>(13); // +118
		hitGlobalId = buffer->Read<uint16_t>(13); // +120
	}

	if (damageType < 2)
	{
		localPos.x = buffer->ReadSignedFloat(16, 55.f); // divisor: 0x425C0000
		localPos.y = buffer->ReadSignedFloat(16, 55.f);
		localPos.z = buffer->ReadSignedFloat(16, 55.f);

		if (damageType == 1)
		{
			hasVehicleData = buffer->Read<uint8_t>(1);

			if (hasVehicleData)
			{
				tyreIndex = buffer->Read<uint8_t>(4); // +122
				suspensionIndex = buffer->Read<uint8_t>(4); // +123
			}
		}
	}
	else
	{
		hitComponent = buffer->Read<uint8_t>(5); // +108
	}

	f133 = buffer->Read<uint8_t>(1);
	hasImpactDir = buffer->Read<uint8_t>(1);

	if (hasImpactDir)
	{
		impactDir.x = buffer->ReadSignedFloat(16, 6.2831854820251f); // divisor: 0x40C90FDB
		impactDir.y = buffer->ReadSignedFloat(16, 6.2831854820251f);
		impactDir.z = buffer->ReadSignedFloat(16, 6.2831854820251f);
	}

	buffer->Seek(0);

	// LOG(INFO) << localPos.x << " " << localPos.y << " " << localPos.z << ". " << math::distance_between_vectors(localPos, *g_local_player->m_navigation->get_position()) << " " << (g_local_player->m_net_object->m_object_id == hitGlobalId ? "TRUE" : "FALSE") << " " << damageType << " " << damageFlags;

	if (damageType == 3 && (damageFlags & (1 << 1)) == 0)
		hitGlobalId = self::local_ped ? self::local_ped->m_net_object->m_object_id : 0;

	/*if (g.session.damage_karma && g_local_player && g_local_player->m_net_object
		&& (g_local_player->m_net_object->m_object_id == hitGlobalId
			|| math::distance_between_vectors(localPos, *g_local_player->m_navigation->get_position()) < 1.5f))
	{
		int id = player->m_player_id;
		g_fiber_pool->queue_job([id, hitComponent, overrideDefaultDamage, weaponType, weaponDamage, tyreIndex, suspensionIndex, damageFlags, actionResultName, actionResultId, f104, hitEntityWeapon, hitWeaponAmmoAttachment, silenced, hasImpactDir, impactDir, localPos] {
			auto player = g_player_service->get_by_id(id);

			if (!player->is_valid() || !player->get_ped())
				return;

			g_pointers->m_gta.m_send_network_damage(g_player_service->get_self()->get_ped(),
				player->get_ped(),
				(rage::fvector3*)&localPos,
				hitComponent,
				overrideDefaultDamage,
				weaponType,
				weaponDamage,
				tyreIndex,
				suspensionIndex,
				damageFlags,
				actionResultName,
				actionResultId,
				f104,
				hitEntityWeapon,
				hitWeaponAmmoAttachment,
				silenced,
				false,
				player->get_ped()->m_navigation->get_position());
		});
	}*/

	return false;
}

void hooks::received_event(rage::netEventMgr* event_manager, CNetGamePlayer* source_player,
	CNetGamePlayer* target_player, uint16_t event_id, int event_index, int event_handled_bitset,
	int buffer_size, rage::datBitBuffer* buffer)
{
	if (event_id >= 91U)
	{
		LOG_DEBUG("Invalid Event Id '{}' from '{} ({})'", event_id, source_player->get_name(), source_player->m_account_id);
		return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
	}

	const auto event_name = *reinterpret_cast<char**>(reinterpret_cast<DWORD64>(event_manager) + 8ui64 * event_id + 0x3B6B0);
	if (event_name == nullptr || source_player == nullptr || source_player->m_player_id < 0 || source_player->m_player_id >= 32)
	{
		LOG_DEBUG("Invalid Event or Sender. Event: '{} | ID: {}' from '{} ({})'", event_name ? static_cast<char*>(event_name) : "Invalid", event_id, source_player ? source_player->get_name() : "Invalid", source_player ? source_player->m_account_id : 0);
		return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
	}

#ifdef _DEBUG
	if (commands::settings::debug::log_events && static_cast<eNetworkEvents>(event_id) != eNetworkEvents::SCRIPTED_GAME_EVENT)
		LOG_DEBUG("Event: '{} | ID: {}' from '{} ({})'", event_name, event_id, source_player->get_name(), source_player->m_account_id);
#endif

	const auto plyr = g_player_manager->get_by_id(source_player->m_player_id);

	/*if (player && player->block_net_events)
	{
		g_pointers->g_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
		return;
	}*/

	switch (static_cast<eNetworkEvents>(event_id))
	{
		case eNetworkEvents::SCRIPTED_GAME_EVENT:
		{
			auto scripted_game_event = std::make_unique<CScriptedGameEvent>();
			buffer->ReadDword(&scripted_game_event->m_args_size, 32);

			if (scripted_game_event->m_args_size > sizeof(scripted_game_event->m_args))
			{
				if (crash.process(plyr, "E8")) // out of bounds tse args size
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			
			buffer->ReadArray(&scripted_game_event->m_args, 8 * scripted_game_event->m_args_size);

			if (hooks::scripted_game_event(scripted_game_event.get(), source_player))
			{
				return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_PTFX_EVENT:
		{
			if (!plyr)
			{
				return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}

			if (plyr->m_ptfx_rate_limit.process())
			{
				if (ptfx_spam.process(plyr, "PTFX Spam"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			break;
		}
		case eNetworkEvents::KICK_VOTES_EVENT:
		{
			if (const auto player_bitfield = buffer->Read<uint32_t>(32); player_bitfield & 1 << target_player->m_player_id)
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				vote_kick.process(plyr, "Vote Kick");
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_INCREMENT_STAT_EVENT:
		{
			const auto increment_stat_event = std::make_unique<CNetworkIncrementStatEvent>();
			buffer->ReadDword(&increment_stat_event->m_stat, 0x20);
			buffer->ReadDword(&increment_stat_event->m_amount, 0x20);
			if (hooks::increment_stat_event(increment_stat_event.get(), source_player))
			{
				return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPT_ENTITY_STATE_CHANGE_EVENT:
		{
			if (const auto type = buffer->Read<ScriptEntityChangeType>(4); type == ScriptEntityChangeType::SettingOfTaskVehicleTempAction)
			{
				if (const auto action = buffer->Read<uint32_t>(8); (action >= 15 && action <= 18) || action == 33)
				{
					if (crash.process(plyr, "Crash E7")) // Vehicle Temp Action 
					{
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					}
				}
			}
			else if (type > ScriptEntityChangeType::SetVehicleExclusiveDriver || type < ScriptEntityChangeType::BlockingOfNonTemporaryEvents)
			{
				if (crash.process(plyr, "Crash E2")) // Invalid Script Entity Change Type 
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::NETWORK_CLEAR_PED_TASKS_EVENT:
		{
			if (const int net_id = buffer->Read<int>(13); self::local_ped && self::local_ped->m_net_object && self::local_ped->m_net_object->m_object_id == net_id)
			{
				if (clear_ped_tasks.process(plyr, "Clear Ped Tasks"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::RAGDOLL_REQUEST_EVENT:
		{
			if (const int net_id = buffer->Read<int>(13); self::local_ped && self::local_ped->m_net_object && self::local_ped->m_net_object->m_object_id == net_id)
			{
				if (ragdoll.process(plyr, "Ragdoll"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			buffer->Seek(0);
			break;
		}
		// Don't block this event, we still want to report this player
		// because if we still report others, our account seems less fishy
		case eNetworkEvents::REPORT_CASH_SPAWN_EVENT:
		{
			uint32_t money;
			buffer->Seek(64);
			buffer->ReadDword(&money, 32);
			buffer->Seek(0);
			if (money >= 2000)
			{
				if (cash_spawn.process(plyr, "Cash Spawn"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			break;
		}
		// player sending this event is a modder
		case eNetworkEvents::REPORT_MYSELF_EVENT:
		{
			if (report_my_self.process(plyr, "Report My Self Event"))
			{
				return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}
			break;
		}
		case eNetworkEvents::REQUEST_CONTROL_EVENT:
		{
			if (const auto net_id = buffer->Read<int>(13);
				self::local_ped && self::local_ped->m_vehicle &&
				self::local_ped->m_vehicle->m_net_object &&
				self::local_ped->m_vehicle->m_net_object->m_object_id == net_id)
				//The request is for a vehicle we are currently in.
			{
				const auto personal_vehicle = mechanic::get_personal_vehicle();
				const auto veh = g_pointers->g_pointer_to_handle(self::local_ped->m_vehicle);
				if (!NETWORK::NETWORK_IS_ACTIVITY_SESSION() || personal_vehicle == veh || self::spawned_vehicles.contains(net_id))
				{
					if (request_control.process(plyr, "Request Control"))
					{
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					}
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::SCRIPT_WORLD_STATE_EVENT:
		{
			const auto type = buffer->Read<WorldStateDataType>(4);
			buffer->Read<bool>(1);
			CGameScriptId id;
			script_id_deserialize(id, *buffer);

			if (type == WorldStateDataType::Rope)
			{
				buffer->Read<int>(9); // network rope id
				buffer->Read<float>(19); // pos x
				buffer->Read<float>(19); // pos y
				buffer->Read<float>(19); // pos z
				buffer->Read<float>(19); // rot x
				buffer->Read<float>(19); // rot y
				buffer->Read<float>(19); // rot z
				buffer->Read<float>(16); // length
				const auto int_type = buffer->Read<int>(4);
				const auto initial_length = buffer->Read<float>(16);

				if (const auto min_length = buffer->Read<float>(16); int_type == 0 || initial_length < min_length)
					// https://docs.fivem.net/natives/?_0xE832D760399EB220
				{
					if (crash.process(plyr, "Crash E3")) // Rope 
					{
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					}
				}
			}
			else if (type == WorldStateDataType::PopGroupOverride)
			{
				const int pop_group = buffer->Read<int>(32); // Pop Group
				if (const int percentage = buffer->Read<int>(7); pop_group == 0 && (percentage == 0 || percentage == 103))
				{
					if (crash.process(plyr, "Crash E1")) // Pop Group Override 
					{
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					}
				}
			}
			else if (type > WorldStateDataType::VehiclePlayerLocking || type < WorldStateDataType::CarGen)
			{
				if (crash.process(plyr, "Crash E5")) // Invalid World State Type 
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			/*else if (type == WorldStateDataType::PopMultiplierArea && g.protections.stop_traffic && !NETWORK::NETWORK_IS_ACTIVITY_SESSION())
			{
				g_pointers->g_sendEventAck(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}*/
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_WEAPON_EVENT:
		{
			const auto net_id = buffer->Read<int16_t>(13);

			if (const auto hash = buffer->Read<Hash>(32); hash == "WEAPON_UNARMED"_joaat)
			{
				if (crash.process(plyr, "Crash E6")) // Remove Unarmed 
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			if (self::local_ped && self::local_ped->m_net_object && self::local_ped->m_net_object->m_object_id == net_id)
			{
				if (remove_weapon.process(plyr, "Remove Weapon"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::REMOVE_ALL_WEAPONS_EVENT:
		{
			if (const auto net_id = buffer->Read<int16_t>(13); self::local_ped && self::local_ped->m_net_object && self::local_ped->m_net_object->m_object_id == net_id)
			{
				if (remove_weapon.process(plyr, "Remove All Weapons"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_WEAPON_EVENT:
		{
			if (const auto net_id = buffer->Read<int16_t>(13); self::local_ped && self::local_ped->m_net_object && self::local_ped->m_net_object->m_object_id == net_id)
			{
				if (give_weapon.process(plyr, "Give Weapon"))
				{
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}
			}
			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::GIVE_CONTROL_EVENT:
		{
			uint32_t timestamp                = buffer->Read<uint32_t>(32);
			int count                         = buffer->Read<int>(2);
			bool all_objects_migrate_together = buffer->Read<bool>(1);
			eNetObjType sync_type = {};
			count = std::min(count, 3);
			for (int i = 0; i < count; i++)
			{
				int net_id              = buffer->Read<int>(13);
				auto object_type = buffer->Read<eNetObjType>(4);
				int migration_type      = buffer->Read<int>(3);

				if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
				{
					if (crash.process(plyr, "Crash E9")) // Out Of Bounds Give Control Type
					{
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					}
				}

				sync_type = object_type;
			}

			buffer->Seek(0);

			if (count)
			{
				g_syncing_player      = source_player;
				g_syncing_object_type = sync_type;
			}
			break;
		}
		case eNetworkEvents::NETWORK_PLAY_SOUND_EVENT:
		{
			if (plyr)
			{
				if (plyr->m_play_sound_rate_limit.process())
				{
					if (sound_spam.process(plyr, "Sound Spam") || crash.process(plyr, "Crash E4"))
					{
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
					}
				}
			}

			const auto sound_hash = buffer->Read<uint32_t>(32);
			switch (sound_hash)
			{
				case 38209760u:
				case 2147921695u:
				case 39274731u:
				case 35490031u:
				case 36485353u:
				case 36432105u:
				case 36301033u:
				case 36133097u:
				case 36157673u:
				case 36149481u:
				case 36161769u:
				case 36153577u:
				case 33204461u:
				case "remote_ring"_joaat:
				{
					if (sound_spam.process(plyr, "Sound Spam"))
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);

					break;
				}
			default: ;
			}

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::EXPLOSION_EVENT:
		{
			auto f186 = buffer->Read<uint16_t>(16);
			auto f208 = buffer->Read<uint16_t>(13);
			auto ownerNetId = buffer->Read<uint16_t>(13);
			auto f214 = buffer->Read<uint16_t>(13);
			const auto explosion_tag = static_cast<eExplosionTag>(buffer->ReadSigned<int>(8));
			auto damage_scale = buffer->Read<int>(8) / 255.0f;

			auto pos_x = buffer->ReadSignedFloat(22, 27648.0f);
			auto pos_y = buffer->ReadSignedFloat(22, 27648.0f);
			auto pos_z = buffer->ReadFloat(22, 4416.0f) - 1700.0f;

			auto f242 = buffer->Read<uint8_t>(1);
			auto f104 = buffer->Read<uint16_t>(16);
			auto camera_shake = buffer->Read<int>(8) / 127.0f;

			const auto is_audible = buffer->Read<uint8_t>(1);
			auto f189 = buffer->Read<uint8_t>(1);
			const auto is_invisible = buffer->Read<uint8_t>(1);

			if (is_invisible && !is_audible)
			{
				if (explosion_tag == GRENADE && math::distance_between_vectors(self::position, { pos_x, pos_y, pos_z }) < 10.f)
				{
					if (ragdoll.process(plyr, "Ragdoll"))
						return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				}

				if (modded_explosion.process(plyr, "Modded Explosion"))
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}

			if (plyr->m_explosion_rate_limit.process())
			{
				if (explosion_spam.process(plyr, "Explosion Spam"))
					return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}

			if (explosion.process(plyr, "Explosion"))
			{
				return g_pointers->g_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			}
			//scan_explosion_event(source_player, buffer);

			buffer->Seek(0);
			break;
		}
		case eNetworkEvents::WEAPON_DAMAGE_EVENT:
		{
			if (scan_weapon_damage_event(event_manager, source_player, target_player, event_index, event_handled_bitset, buffer))
			{
				return;
			}
			break;
		}
	default: ;
	}
	return g_hooking->get_original<received_event>()(event_manager, source_player, target_player, event_id, event_index, event_handled_bitset, buffer_size, buffer);
}
