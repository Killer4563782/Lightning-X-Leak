#include "commands/manager.hpp"
#include "commands/settings.h"
#include "hooking/hooking.hpp"
#include "rage/gta/net_game_event.hpp"

inline void gamer_handle_serialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
{
	constexpr int PC_PLATFORM = 3;
	buf.Write<uint8_t>(PC_PLATFORM, 8);
	buf.WriteInt64(*(int64_t*)&hnd.m_rockstar_id, 64);
	buf.Write<uint8_t>(hnd.m_padding, 8);
}

bool hooks::send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team)
{
	if (commands::settings::misc::run_commands_through_chat && message[0] == '/')
	{
		//vali: Please don't do this....
		//vali: Use the net message, it's much easier. You can also yoink the scaleform.
		commands::manager::execute(std::string(message + 1));
		return false;
	}

	//chat::send_message(message, nullptr, false, is_team);
	//return true;

	return g_hooking->get_original<send_chat_message>()(team_mgr, local_gamer_info, message, is_team);
}
