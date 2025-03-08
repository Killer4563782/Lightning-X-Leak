#include "battleye.h"
#include "thread/threadpool.h"
#include "fiber/script_mgr.hpp"
#include <fiber/script.hpp>
#include "commands/types/looped_command.hpp"
#include "commands/types/player/player_command.hpp"
#include <util/packet.hpp>
#include "commands/settings.h"
#include "commands/manager.hpp"

using namespace commands::settings;

namespace
{
	static std::string base64_encode(const std::string& data)
	{
		static constexpr char sEncodingTable[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

		size_t in_len = data.size();
		size_t out_len = 4 * ((in_len + 2) / 3);
		std::string ret(out_len, '\0');
		size_t i;
		char* p = ret.data();

		for (i = 0; i < in_len - 2; i += 3)
		{
			*p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
			*p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
			*p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int)(data[i + 2] & 0xC0) >> 6)];
			*p++ = sEncodingTable[data[i + 2] & 0x3F];
		}
		if (i < in_len)
		{
			*p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
			if (i == (in_len - 1))
			{
				*p++ = sEncodingTable[((data[i] & 0x3) << 4)];
				*p++ = '=';
			}
			else
			{
				*p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
				*p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
			}
			*p++ = '=';
		}

		return ret;
	}
}

namespace commands
{


	bool battleye_service::is_running()
	{
		return m_battleye_api.m_shutdown != nullptr;
	}

	void battleye_service::send_message_to_client(std::uint64_t token, void* message, int size)
	{
		packet pkt;

		char header_buf[32];
		rage::datBitBuffer header(header_buf, sizeof(header_buf));

		header.Write<int>(size, 11);
		header.Write<bool>(false, 1);

		pkt.write_message(rage::eNetMessage::MsgBattlEyeCmd);
		pkt.m_buffer.WriteArray(&header_buf, header.GetDataLength() * 8);
		pkt.m_buffer.WriteArray(message, size * 8);


		if (auto plyr = g_player_manager->get_by_host_token(token); plyr && plyr->get_session_player())
		{
			pkt.send(plyr->get_session_player()->m_msg_id, true);
		}
	}

	void battleye_service::send_message_to_server(std::uint64_t token, void* message, int size)
	{
		packet pkt;

		char header_buf[32];
		rage::datBitBuffer header(header_buf, sizeof(header_buf));

		header.Write<int>(size, 11);
		header.Write<bool>(true, 1);

		pkt.write_message(rage::eNetMessage::MsgBattlEyeCmd);
		pkt.m_buffer.WriteArray(&header_buf, header.GetDataLength() * 8);
		pkt.m_buffer.WriteArray(message, size * 8);


		if (auto plyr = g_player_manager->get_by_host_token(token); plyr && plyr->get_session_player())
		{
			pkt.send(plyr->get_session_player()->m_msg_id, true);
		}
	}

	void battleye_service::kick_player(std::uint64_t token, const char* reason)
	{
		if (auto plyr = g_player_manager->get_by_host_token(token))
		{
			LOG(warn, "BattlEye wants us to kick {} for {}", plyr->get_name(), reason);
			if (!plyr->tampered_with_be)
			{
				g_fiber_pool->queue_job([&]
				{
					add_player(plyr->get_net_game_player()->get_host_token(), plyr->get_rockstar_id(), plyr->get_name());
				});
			}
		}
	}

	void battleye_service::script_func()
	{
		bool was_host = false;
		while (is_running())
		{
			if (g_player_manager->get_self()->is_valid())
			{
				bool is_host = g_player_manager->get_self()->is_host();

				if (is_host != was_host)
				{
					if (is_host)
					{
						for (auto& [_, player] : g_player_manager->players())
						{
							add_player(player->get_net_game_player()->get_host_token(), player->get_rockstar_id(), player->get_name());
						}
					}
					was_host = is_host;
				}
			}

			script::get_current()->yield();
		}
	}

	void battleye_service::thread_func()
	{
		while (is_running() && g_running)
		{
			// we need to run this every frame (lockstep with game or otherwise)
			{
				std::lock_guard lock(m_mutex);
				if (is_running() && !m_battleye_api.m_run())
				{
					LOG(warn, "BE::Run failed");
				}
			}

			std::this_thread::yield();
		}
	}

	battleye_service::~battleye_service()
	{
		stop();
	}

	void battleye_service::start() {
		std::lock_guard lock(m_mutex);

		if (is_running()) {
			LOG(warn, "BattlEye service is already running");
			return;
		}

		// Load the server DLL
		auto server_handle = LoadLibraryA("BattlEye\\BEServer_x64.dll");
		if (!server_handle) {
			LOG(warn, "Failed to load BEServer_x64.dll");
			return;
		}

		auto client_handle = LoadLibraryA("BattlEye\\BEClient_x64.dll");
		if (!client_handle) {
			LOG(warn, "Failed to load BEClient_x64.dll");

			return;
		}

		auto server_init_func = reinterpret_cast<init_t>(GetProcAddress(server_handle, "Init"));
		if (!server_init_func) {
			LOG(warn, "Failed to retrieve from BEServer_x64.dll");

			return;
		}

		auto client_init_func = reinterpret_cast<init_t>(GetProcAddress(client_handle, "Init"));
		if (!client_init_func) {
			LOG(warn, "Failed to retrieve from BEClient_x64.dll");

			return;
		}

		m_battleye_user_data.m_game_name = "paradise";
		m_battleye_user_data.m_log_func = [] (const char* message, int level) {
			LOG(info, "BattlEye: {}", message);
		};
		m_battleye_user_data.m_kick_player = [] (std::uint64_t player, const char* reason) {
			g_battleye_service.kick_player(player, reason);
		};
		m_battleye_user_data.m_send_message = [] (std::uint64_t player, const void* pkt_data, int size) {
			g_battleye_service.send_message_to_client(player, const_cast<void*>(pkt_data), size);
		};

		bool server_initialized = server_init_func(1, &m_battleye_user_data, &m_battleye_api);
		bool client_initialized = client_init_func(1, &m_battleye_user_data, &m_battleye_api);

		if (server_initialized && client_initialized) {
			LOG(info, "BattlEye successfully initialized");
			g_thread_pool->push([this] { this->thread_func(); });
			g_script_mgr.add_script(std::make_unique<script>([this] { this->script_func(); }, "BE Background Script", false));
		}
		else {
			LOG(warn, "Failed to initialize BattlEye");
		}


	}


	void battleye_service::stop()
	{
		std::lock_guard lock(m_mutex);

		if (!is_running())
		{
			return;
		}

		m_battleye_api.m_shutdown();
		memset(&m_battleye_api, 0, sizeof(CApi));
	}

	void battleye_service::add_player(std::uint64_t token, std::uint64_t rockstar_id, const char* name)
	{
		std::lock_guard lock(m_mutex);

		if (!is_running() || !g_player_manager->get_self()->is_host())
		{
			return;
		}

		char string[32]{};

		snprintf(string, sizeof(string), "%I64d", rockstar_id);
		auto guid = base64_encode(string);

		m_battleye_api.m_add_player(token, -1, 0, name, false);
		m_battleye_api.m_assign_guid(token, guid.data(), guid.length());
		m_battleye_api.m_assign_guid_verified(token, guid.data(), guid.length());
		m_battleye_api.m_set_player_state(token, 1);
	}

	void battleye_service::remove_player(std::uint64_t token)
	{
		std::lock_guard lock(m_mutex);

		if (!is_running())
		{
			return;
		}

		m_battleye_api.m_set_player_state(token, -1);
	}

	void battleye_service::receive_message(std::uint64_t token, void* message, int size)
	{
		std::lock_guard lock(m_mutex);

		if (!is_running())
		{
			return;
		}

		m_battleye_api.m_receive_message(token, const_cast<const void*>(message), size);
	}

	void battleye_service::on_receive_message_from_server(std::uint64_t token, void* message, int size)
	{
		if (size == 0)
		{
			return;
		}

		auto msg = reinterpret_cast<unsigned char*>(message);
		auto op = msg[0];

		switch (op)
		{
		case INIT:
		{
			std::uint8_t payload[] = { 0x0, 0x5 };
			send_message_to_server(token, payload, sizeof(payload));
			break;
		}
		case START:
		{
			LOG(info, "BattlEye: Our GUID: {}", (char*)&msg[3]);
			send_message_to_server(token, message, 2);
			break;
		}
		case HEARTBEAT:
		{
			send_message_to_server(token, message, size);

			if (commands::settings::session::kick_host_to_stay_in_session && msg[1] == 5)
			{
				if (auto player = g_player_manager->get_by_host_token(token))
				{
					"nfkick"_PC->call(player, {});
				}
			}

			break;
		}
		case REQUEST:
		{
			if (size == 1028)
			{
				break;
			}
			send_message_to_server(token, message, 2);
			break;
		}
		}
	}


}