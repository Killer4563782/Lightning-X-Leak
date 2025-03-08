#include "network.h"
#include "casino/casino.hpp"
#include "players/players.h"
#include "protections/protections_sub.h"
#include "session_starter/session_starter.h"
#include "friends/friends.h"
#include "session/session.h"
#include "spoofing/spoofing.h"

namespace ui::submenus
{
	void network_submenu::on_init()
	{
		this->add(&g_players_submenu);
		this->add(&g_friends_submenu);
		this->add(&g_session_starter_submenu);
		this->add(&g_protections_submenu);
		this->add(&g_session_submenu);
		this->add(&g_casino_submenu);
		this->add(&g_spoofing_submenu);
	}

	network_submenu g_network_submenu("Network", "Options for players and online");
}
