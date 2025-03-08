#include "commands/manager.hpp"
#include "helpers/playerInfo.h"
#include "hooking/hooking.hpp"
#include "services/players/player_manager.hpp"

char* hooks::player_tags_on_minimap(__int64 blip)
{
	const auto result = g_hooking->get_original<player_tags_on_minimap>()(blip);

	if ("player_tags_on_minimap"_BC->m_toggle && *reinterpret_cast<uint8_t*>(blip + 0x60) == 4 && *reinterpret_cast<uint16_t*>(blip + 48))
	{
		if (const auto name = *reinterpret_cast<char**>(blip + 40); *name)
		{
			const auto& players = g_player_manager->m_players | std::views::values;

			if (const auto it = std::ranges::find_if(players, [&](const auto& player) {
				return player->is_valid() && strcmp(player->get_name(), name) == 0;
			}); it != players.end())
			{
				std::string buffer;
				buffer.reserve(128);
				buffer = std::string(name) + " " + player_info::get_player_tags(*it);
				return _strdup(buffer.c_str());
			}
		}
	}

	return result;
}
