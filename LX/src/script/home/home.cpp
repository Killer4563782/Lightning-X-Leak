#include "home.h"

#include "debug/debug_sub.h"
#include "developer/developer.h"
#include "miscellaneous/miscellaneous.h"
#include "network/network.h"
#include "recovery/recovery.h"
#include "self/self.h"
#include "settings/settings.h"
#include "teleport/teleport.h"
#include "vehicle/vehicle.h"
#include "visuals/visuals_sub.h"
#include "weapons/weapons.h"
#include "world/world.h"
#include "developer/scripts/scripts.h"

namespace ui::submenus
{
	void home_submenu::on_init()
	{
		this->add(&g_self_submenu);
		this->add(&g_network_submenu);
		this->add(&g_visuals_submenu);
		this->add(&g_weapons_submenu);
		this->add(&g_vehicle_submenu);
		this->add(&g_teleport_submenu);
		this->add(&g_recovery_submenu);
		this->add(&g_world_submenu);
		this->add(&g_miscellaneous_submenu);
#ifdef ENABLE_DEVELOPER_MODE
		this->add(&g_developer_submenu); 
#endif
#ifdef _DEBUG
		this->add(&g_debug_submenu);
#endif
		this->add(scripts_submenu("Scripts"));
		this->add(&g_settings_submenu);  
	}

	home_submenu g_home_submenu("Home");
}
