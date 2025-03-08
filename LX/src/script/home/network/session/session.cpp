#include "session.h"
#include "gui/types/toggle_option.hpp"
#include "commands/manager.hpp"
#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/icon_option.hpp"
#include <helpers/inputs.h>
#include "commands/settings.h"
#include "helpers/playerInfo.h"
#include "thread/threadpool.h"
#include <services/api/api_service.hpp>
#include "util/scripts.h"
#include <script/home/network/region.h>
#include "services/online/battleye.h"

namespace ui::submenus
{
	void session_submenu::on_init()
	{
		
		this->add(toggle_option("off_radar"_LC));
		this->add(toggle_option("reveal_players"_BC));
		this->add(toggle_option("block_muggers"_BC));
		this->add(toggle_option("block_ceo_raids"_BC));
		this->add(toggle_option("hide_from_player_list"_BC));
		this->add(toggle_option("Rejoin From kicked Session", "Rejoin from kicked session", &commands::settings::session::rejoin_desync_kick));
		this->add(toggle_option("Prevent Network Bail", "Prevents network bail on session join or in freemode", &commands::settings::session::prevent_network_bail));
		this->add(break_option("Transition"));
		this->add(toggle_option("force_script_host"_BC));
		this->add(toggle_option("fast_join"_BC));
		this->add(toggle_option("player_magnet"_BC));
		this->add(&g_region_submenu);
		/*this->add(toggle_option("battle_server"_LC));*/
		this->add(toggle_option("seamless_joining"_BC));
		this->add(toggle_option("transition_state"_BC));
		this->add(function_option("Bail", "Bail", [this]
		{
			g_fiber_pool->queue_job([&]
			{
				if (NETWORK::NETWORK_CAN_BAIL())
				{
					NETWORK::NETWORK_BAIL(16, 0, 0);
				}
			});
		}));
		this->add(function_option("Force Script Host", "Forcefully take (freemode) script host", [this]
		{
			g_fiber_pool->queue_job([&]
			{
				scripts::force_host("freemode"_joaat);
			});
		}));
		this->add(break_option("RID Options"));
		this->add(icon_option("RID Joiner", "Input Name or RID", ICON_FA_KEYBOARD, [this]
		{
			get_input("Input Name or RID", {}, text_input::InputFlags_Dont_Add_History, [&](const std::string& input)
			{
				g_thread_pool->push([&]
				{
					const auto rid = !contains_a_character(input) ? stoi(input) : g_api_service->get_rid_from_username(input);
					g_api_service->rid_join(rid);
				});
			});
		}));
	}

	void session_submenu::on_tick()
	{
		player_info::session_host_queue_info();
	}

	session_submenu g_session_submenu("Session", "Session options");
}
