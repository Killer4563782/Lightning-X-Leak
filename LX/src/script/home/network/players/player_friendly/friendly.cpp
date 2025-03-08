#include "friendly.h"

#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "player_give_collectibles/give_collectibles.h"

namespace ui::submenus
{
	void player_friendly_submenu::on_init()
	{
		this->add(give_collectibles_submenu("Give Collectibles"));
		this->add(toggle_option("rp_drop"_LC));
		this->add(function_option("Give All Weapons", {}, [this]
		{
			"give_all_weapons"_PC->call(g_player_manager->get_selected(), {});
		}));
		this->add(function_option("Give Heal", {}, [this]
		{
			"give_heal_and_armor"_PC->call(g_player_manager->get_selected(), {});
		}));
		/*this->add(function_option("Never Wanted", {}, [this]
		{
			constexpr size_t arg_count = 4;
			int64_t args[arg_count] = { static_cast<int64_t>(eScriptEvents::ScriptEventRemoveWantedLevel),
			self::player, 1 << PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id()),
			static_cast<int64_t>(scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id())].ScriptEventReplayProtectionCounter) };

			g_pointers->g_trigger_script_event(1, args, arg_count, 1 << PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id()), static_cast<int>(eScriptEvents::ScriptEventRemoveWantedLevel));
		}));
		this->add(function_option("Off Radar", {}, [this]
		{
			constexpr size_t arg_count = 8;
			int64_t args[arg_count] = { static_cast<int64_t>(eScriptEvents::ScriptEventOffTheRadar),
			static_cast<int64_t>(self::player),
			1 << PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id()),
			static_cast<int64_t>(NETWORK::GET_NETWORK_TIME() + 1),
			0,
			true,
			false,
			static_cast<int64_t>(scr_globals::gpbd_fm_3.as<GPBD_FM_3*>()->Entries[PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id())].ScriptEventReplayProtectionCounter) };
			g_pointers->g_trigger_script_event(1, args, arg_count, 1 << PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id()), static_cast<int>(eScriptEvents::ScriptEventOffTheRadar));
		}));*/
		//this->add(toggle_option("give_snacks_to_player"_LC));
		//this->add(function_option("Give Script Host")); Will add soon!
	}
}
