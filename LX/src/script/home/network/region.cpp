#include <util/script_local.hpp>
#include <gui/types/function_option.hpp>
#include <rage/util.h>
#include "region.h"
namespace ui::submenus
{
	void region_submenu::on_init()
	{
		for (const auto& region : tables::regions)
		{
			this->add(function_option(region.name, region.name, [this, region]
			{
				*g_pointers->g_region_code = region.id; 
			}));
		}
	}
	region_submenu g_region_submenu("Region Switcher", "Switchs region");
}