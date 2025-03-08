#ifdef ENABLE_DEVELOPER_MODE 
#include "developer.h"

#include "gui/types/break_option.hpp"
#include "particles/particles_submenu.h"
#include "scripts/scripts.h"

namespace ui::submenus
{
	void developer_submenu::on_init()
	{
		this->add(particle_submenu("Particles"));
		this->add(break_option("Options"));
	}

	developer_submenu g_developer_submenu("Developer", {});
}
#endif