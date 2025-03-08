#include "helpers/playerInfo.h"
#include "hooking/hooking.hpp"
#include "services/players/player_manager.hpp"

bool hooks::network_player_mgr_init(CNetworkPlayerMgr* _this, uint64_t a2, uint32_t a3, uint32_t a4[4])
{
	LOG_DEBUG("CNetworkPlayerMgr#init got called, we're probably entering a session.");

	const bool result = g_hooking->get_original<network_player_mgr_init>()(_this, a2, a3, a4);

	g_player_manager->player_join(_this->m_local_net_player);

	return result;
}

void hooks::network_player_mgr_shutdown(CNetworkPlayerMgr* _this)
{
	LOG_DEBUG("CNetworkPlayerMgr#shutdown got called, we're probably leaving our session.");
	g_player_manager->do_cleanup();
	self::spawned_vehicles.clear();
	player_info::geo_location_cache.clear();

	g_hooking->get_original<network_player_mgr_shutdown>()(_this);
}
