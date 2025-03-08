#pragma once
#include "rage/util.h"
#include "rage/gta/net_game_event.hpp"
#include "services/players/player_manager.hpp"

class packet
{
public:
	char m_data[0x400]{};
	rage::datBitBuffer m_buffer;

	packet() : m_buffer(m_data, sizeof(m_data))
	{}

	void send(uint64_t msg_id)
	{
		g_pointers->g_queue_packet(get_network()->m_game_session_ptr->m_net_connection_mgr, static_cast<int>(msg_id), m_data, m_buffer.m_curBit + (7 >> 3), 1, nullptr);
	}

	void send(const player_ptr& player, int connection_id)
	{
		send(player->get_session_player()->m_player_data.m_peer_id_2, connection_id);
	}

	void send(uint32_t peer_id, int connection_id)
	{
		auto mgr = get_network()->m_game_session_ptr->m_net_connection_mgr;
		auto peer = g_pointers->g_get_connection_peer(mgr, peer_id);
		g_pointers->g_send_packet(mgr, &peer->m_peer_address, connection_id, m_data, m_buffer.m_curBit + (7 >> 3), 0x1000000);
	}

	explicit operator rage::datBitBuffer& ()
	{
		return m_buffer;
	}

	template<typename T>
	void write(T data, int length)
	{
		m_buffer.Write<T>(data, length);
	}

	void write_message(rage::eNetMessage message)
	{
		write<int>(0x3246, 14);
		if (static_cast<int>(message) > 0xFF)
		{
			write<bool>(true, 1);
			write<rage::eNetMessage>(message, 16);
		}
		else
		{
			write<bool>(false, 1);
			write<rage::eNetMessage>(message, 8);
		}
	}

	void write_peer_id(uint64_t peer_id)
	{
		char b[8];
		rage::datBitBuffer buf(b, 8);
		buf.WriteQWord(peer_id, 64);
		m_buffer.WriteArray(b, 8 * buf.GetDataLength());
	}
};