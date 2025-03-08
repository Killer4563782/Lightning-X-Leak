#include "commands/manager.hpp"
#include "commands/settings.h"
#include "hooking/hooking.hpp"
#include "rage/gta/net_game_event.hpp"
#include "services/players/player_manager.hpp"
#include "services/reactions/reactions.h"
#include "util/util.h"

inline void gamer_handle_deserialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
{
	if ((hnd.m_platform = buf.Read<uint8_t>(sizeof(hnd.m_platform) * 8)) != rage::rlPlatforms::PC)
		return;

	buf.ReadRockstarId(&hnd.m_rockstar_id);
	hnd.m_padding = buf.Read<uint8_t>(sizeof(hnd.m_padding) * 8);
}

inline bool is_kick_instruction(rage::datBitBuffer& buffer)
{
	rage::rlGamerHandle sender, receiver;
	char name[18];
	gamer_handle_deserialize(sender, buffer);
	gamer_handle_deserialize(receiver, buffer);
	buffer.ReadString(name, 17);
	int instruction = buffer.Read<int>(32);
	return instruction == 8;
}

bool try_read_secondary_header(rage::datBitBuffer& buffer)
{
	auto data = buffer.Read<std::uint32_t>(20);
	return data == 0x8C253 || data == 0x8924F;
}

bool get_msg_type(rage::eNetMessage& msgType, rage::datBitBuffer& buffer)
{
	uint32_t pos;
	uint32_t magic;
	uint32_t extended{};
	if ((buffer.m_flagBits & 2) != 0 || (buffer.m_flagBits & 1) == 0
		    ? (pos = buffer.m_curBit)
		    : pos = buffer.m_maxBit,
		buffer.m_bitsRead + 15 > pos || !buffer.ReadDword(&magic, 14) || magic != 0x3246 || !buffer.ReadDword(
			&extended, 1))
	{
		msgType = rage::eNetMessage::MSG_INVALID;
		return false;
	}
	uint32_t length = extended ? 16 : 8;
	if ((buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : pos = buffer.m_maxBit,
		length + buffer.m_bitsRead <= pos && buffer.ReadDword(reinterpret_cast<uint32_t*>(&msgType), length))
		return true;
	return false;
}

static void script_id_deserialize(CGameScriptId& id, rage::datBitBuffer& buffer)
{
	id.m_hash = buffer.Read<uint32_t>(32);
	id.m_timestamp = buffer.Read<uint32_t>(32);

	if (buffer.Read<bool>(1))
		id.m_position_hash = buffer.Read<uint32_t>(32);
	else
		id.m_position_hash = 0;

	if (buffer.Read<bool>(1))
		id.m_instance_id = buffer.Read<int32_t>(8);
	else
		id.m_instance_id = -1;
}

static void script_id_serialize(CGameScriptId& id, rage::datBitBuffer& buffer)
{
	buffer.Write<uint32_t>(id.m_hash, 32);
	buffer.Write<uint32_t>(id.m_timestamp, 32);

	if (id.m_position_hash != 0)
	{
		buffer.Write<bool>(true, 1);
		buffer.Write<uint32_t>(id.m_position_hash, 32);
	}
	else
	{
		buffer.Write<bool>(false, 1);
	}

	if (id.m_instance_id != -1)
	{
		buffer.Write<bool>(true, 1);
		buffer.Write<int32_t>(id.m_instance_id, 8);
	}
	else
	{
		buffer.Write<bool>(false, 1);
	}
}

bool is_host_of_session(rage::snSession* session, std::uint32_t peer_id)
{
	if (!session || peer_id == -1)
		return false;

	if (auto player = session->get_player_by_token(session->m_host_token))
		return player->m_player_data.m_peer_id_2 == peer_id;

	return false;
}

bool hooks::receive_net_message(void* a1, rage::netConnectionManager* net_cxn_mgr, rage::netEvent* event)
{
	void* message_data;
	int message_size;
	rage::netConnection::InFrame* frame = nullptr;
	rage::netEventConnectionError* error = nullptr;

	if (event->get_event_type() == rage::netEvent::Type::NET_EVENT_CONNECTION_REQUESTED || event->get_event_type() ==
		rage::netEvent::Type::NET_EVENT_FRAME_RECEIVED)
	{
		frame = reinterpret_cast<rage::netConnection::InFrame*>(event);
		if (frame->m_data == nullptr || frame->m_length == 0 || frame->m_connection_identifier == 2)
			return g_hooking->get_original<receive_net_message>()(a1, net_cxn_mgr, event);

		message_data = frame->m_data;
		message_size = frame->m_length;
	}
	else if (event->get_event_type() == rage::netEvent::Type::NET_EVENT_CONNECTION_ERROR)
	{
		error = reinterpret_cast<rage::netEventConnectionError*>(event);
		if (error->m_size == 0)
			return g_hooking->get_original<receive_net_message>()(a1, net_cxn_mgr, event);

		message_data = error->m_data;
		message_size = error->m_size;
	}
	else
	{
		return g_hooking->get_original<receive_net_message>()(a1, net_cxn_mgr, event);
	}

	rage::datBitBuffer buffer(message_data, message_size);
	buffer.m_flagBits = 1;

	if (try_read_secondary_header(buffer))
	{
		buffer = rage::datBitBuffer(static_cast<char*>(message_data) + 7, message_size - 7);
		buffer.m_flagBits = 1;
	}
	else
	{
		buffer.Seek(0);
	}

	rage::eNetMessage msg_type;

	if (!get_msg_type(msg_type, buffer))
	{
		LOG(warn, "Received message that we cannot parse from cxn id {}", event->m_connection_identifier);
		return g_hooking->get_original<receive_net_message>()(a1, net_cxn_mgr, event);
	}

	rage::snSession* session = nullptr; // game unless proven otherwise

	if (get_network()->m_transition_session_ptr
		&& get_network()->m_transition_session_ptr->m_connection_identifier == event->m_connection_identifier)
	{
		session = get_network()->m_transition_session_ptr;
	}
	else
	{
		session = get_network()->m_game_session_ptr;
	}

	player_ptr player = nullptr; // WILL be null until we get their physical

	for (uint32_t i = 0; i < get_network()->m_game_session_ptr->m_player_count; i++)
	{
		if (auto player_iter = get_network()->m_game_session_ptr->m_players[i])
		{
			if (player_iter->m_player_data.m_peer_id_2 == event->m_peer_id)
			{
				player = g_player_manager->get_by_host_token(
					get_network()->m_game_session_ptr->m_players[i]->m_player_data.m_host_token);
				break;
			}
		}
	}

	int sec_id = 0;

	if (frame)
		sec_id = frame->m_security_id;
	else
	{
		if (auto cxn = g_pointers->g_get_connection_peer(net_cxn_mgr, error->m_peer_id))
			sec_id = cxn->m_security_id;
	}

	auto peer = g_pointers->g_get_peer_by_security_id(sec_id);
	// shouldn't be null in most cases, contains unspoofable data

	if (error && msg_type != rage::eNetMessage::SN_MSG_JOIN_RESPONSE)
	{
		if (peer)
		{
			notify(Warning, 7s, "Received an error packet that isn't SN_MSG_JOIN_RESPONSE from {}", peer->m_info.name);
			LOG(warn, "Received an error packet that isn't SN_MSG_JOIN_RESPONSE from {}", peer->m_info.name);
		}

		return true;
	}

#ifdef _DEBUG
	if (commands::settings::debug::log_packets)
	{
		auto packet_type = "UNKNOWN";
		for (const auto& p : packet_types)
		{
			if (p.second == static_cast<uint32_t>(msg_type))
			{
				packet_type = p.first;
				break;
			}
		}

		LOG_DEBUG("Packet: Type: {} {} | Length: {} | Sender: {}", packet_type, static_cast<int>(msg_type), frame->m_length, player ? player->get_name() : std::format("<MSG:{}>, <CNCTN:0x{:X}>, <PEER:{}>", static_cast<int>(frame->m_msg_id), frame->m_connection_identifier, frame->m_peer_id).c_str());
	}
#endif

	switch (msg_type)
	{
	/*case rage::eNetMessage::MSG_TEXT_MESSAGE:
	case rage::eNetMessage::CMSG_TEXT_MESSAGE:
	{
		char message[256];
		buffer.ReadString(message, 256);
		bool is_team;
		buffer.ReadBool(&is_team);

		if (player->is_spammer)
			return true;

		if (auto spam_reason = chat::is_text_spam(message, player))
		{
			if (g.session.log_chat_messages)
				chat::log_chat(message, player, spam_reason, is_team);

			g_notification_service.push("PROTECTIONS"_T.data(), std::format("{} {}", player->get_name(), "IS_A_SPAMMER"_T.data()));
			player->is_spammer = true;
			if (g.session.kick_chat_spammers
				&& !(player->is_trusted || (player->is_friend() && g.session.trust_friends) || g.session.trust_session))
			{
				if (g_player_service->get_self()->is_host())
					dynamic_cast<player_command*>(command::get("breakup"_J))->call(player, {}),
					dynamic_cast<player_command*>(command::get("hostkick"_J))->call(player, {});

				dynamic_cast<player_command*>(command::get("endkick"_J))->call(player, {});
				dynamic_cast<player_command*>(command::get("nfkick"_J))->call(player, {});
			}
			return true;
		}
		else
		{
			if (g.session.log_chat_messages)
				chat::log_chat(message, player, SpamReason::NOT_A_SPAMMER, is_team);

			if (g.session.chat_commands && message[0] == g.session.chat_command_prefix)
				command::process(std::string(message + 1), std::make_shared<chat_command_context>(player));
			else
				g_lua_manager->trigger_event<menu_event::ChatMessageReceived>(player->id(), message);

			if (msgType == rage::eNetMessage::MsgTextMessage && g_pointers->m_gta.m_chat_data && player->get_net_data())
			{
				rage::rlGamerHandle temp{};
				gamer_handle_deserialize(temp, buffer);

				g_pointers->m_gta.m_handle_chat_message(*g_pointers->m_gta.m_chat_data,
					nullptr,
					&player->get_net_data()->m_gamer_handle,
					message,
					is_team);
				return true;
			}
		}
		break;
	}*/
	case rage::eNetMessage::MSG_SCRIPT_MIGRATE_HOST:
		{
			if (player && player->m_host_migration_rate_limit.process())
			{
				if (player->m_host_migration_rate_limit.exceeded_last_process())
				{
					return kick.process(player, "Kick N1"); // OOM Kick
				}
				return true;
			}
			break;
		}
	case rage::eNetMessage::MSG_SCRIPT_HOST_REQUEST:
		{
			CGameScriptId script;
			script_id_deserialize(script, buffer);

			if (script.m_hash == "freemode"_joaat && "force_script_host"_BC->m_toggle)
				return true;

			break;
		}
	/*case rage::eNetMessage::NET_TIME_SYNC_MSG:
	{
		int action = buffer.Read<int>(2);
		uint32_t counter = buffer.Read<uint32_t>(32);
		uint32_t token = buffer.Read<uint32_t>(32);
		uint32_t timestamp = buffer.Read<uint32_t>(32);
		uint32_t time_diff = (*g_pointers->m_gta.m_network_time)->m_time_offset + frame->m_timestamp;

		if (action == 0)
		{
			player->player_time_value = timestamp;
			player->player_time_value_received_time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
			if (!player->time_difference || time_diff > player->time_difference.value())
				player->time_difference = time_diff;
		}
		break;
	}*/
	case rage::eNetMessage::MSG_KICK_PLAYER:
		{
			auto reason = buffer.Read<KickReason>(3);

			if (!is_host_of_session(get_network()->m_game_session_ptr, event->m_peer_id))
			{
				notify(Warning, 7s, "Received a MSG_KICK_PLAYER from {} who is non-host peer", peer->m_info.name);
				LOG(warn, "Received a MSG_KICK_PLAYER from {} who is non-host peer", peer->m_info.name);
				return true;
			}

			if (reason == KickReason::VOTED_OUT)
			{
				notify(Warning, 7s, "Received a MSG_KICK_PLAYER from {} with reason VOTED_OUT", peer->m_info.name);
				LOG(warn, "Received a MSG_KICK_PLAYER from {} with reason VOTED_OUT", peer->m_info.name);
				return true;
			}

			notify(Warning, 7s, "Received a MSG_KICK_PLAYER from {} with reason: {}", peer->m_info.name,
			       static_cast<int>(reason));
			LOG(warn, "Received a MSG_KICK_PLAYER from {} with reason: {}", peer->m_info.name,
			    static_cast<int>(reason));
			break;
		}
	case rage::eNetMessage::MSG_RADIO_STATION_SYNC_REQUEST:
		{
			if (player)
			{
				if (player->m_radio_request_rate_limit.process())
				{
					if (player->m_radio_request_rate_limit.exceeded_last_process())
					{
						return kick.process(player, "Kick N7"); // OOM Kick
					}
					return true;
				}
			}
			else
			{
				static services::rate_limiter unk_player_radio_requests{1s, 5};

				if (unk_player_radio_requests.process())
				{
					if (unk_player_radio_requests.exceeded_last_process())
					{
						return kick.process(player, "Kick N7"); // OOM Kick
					}
					return true;
				}
			}

			if (!session || !session->is_host())
			{
				notify(Warning, 7s, "Received a MSG_RADIO_STATION_SYNC_REQUEST from {} who is a non-host peer");
				LOG(warn, "Received a MSG_RADIO_STATION_SYNC_REQUEST from {} who is a non-host peer");
				return true;
			}

			break;
		}
	case rage::eNetMessage::MSG_RADIO_STATION_SYNC:
		{
			if (!session || !is_host_of_session(session, event->m_peer_id))
			{
				notify(Warning, 7s, "Received a MSG_RADIO_STATION_SYNC from {} who is a non-host peer", peer->m_info.name);
				LOG(warn, "Received a MSG_RADIO_STATION_SYNC from {} who is a non-host peer", peer->m_info.name);
				return true;
			}

			break;
		}
	case rage::eNetMessage::CROAMING_JOIN_BUBBLE_ACK_MSG:
		{
			int status = buffer.Read<int>(2);
			int bubble = buffer.Read<int>(4);

			if (status == 0 && bubble == 10)
			{
				notify(Warning, 7s, "Received a CROAMING_JOIN_BUBBLE_ACK_MSG from {} with status 0", peer->m_info.name);
				if (player)
					return break_game.process(player, "Break Game"); // OOM Kick

				return true;
			}
			if (status == 0)
			{
				notify(Warning, 7s,
				       "{} has invited us to join their Bubble {}, but we don't believe it would be a wise choice.",
				       peer->m_info.name, bubble);
				LOG(warn, "{} has invited us to join their Bubble {}, but we don't believe it would be a wise choice.",
				    peer->m_info.name, bubble);
				return true;
			}

			break;
		}
	case rage::eNetMessage::MSG_REQUEST_KICK_FROM_HOST:
		{
			notify(Warning, 7s, "Preventing a MSG_REQUEST_KICK_FROM_HOST from {}", peer->m_info.name);
			LOG(warn, "Preventing a MSG_REQUEST_KICK_FROM_HOST from {}", peer->m_info.name);
			return true;
		}
	case rage::eNetMessage::SN_MSG_CONFIG_REQUEST:
		{
			notify(Warning, 7s, "Preventing a SN_MSG_CONFIG_REQUEST from {} ({})", peer->m_info.name, peer->m_info.handle.m_rockstar_id);
			LOG(warn, "Preventing a SN_MSG_CONFIG_REQUEST from {} ({})", peer->m_info.name, peer->m_info.handle.m_rockstar_id);
			return true;
		}
	default: ;
	}

	return g_hooking->get_original<receive_net_message>()(a1, net_cxn_mgr, frame);
}
