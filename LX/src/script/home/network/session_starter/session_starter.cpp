#include "session_starter.h"

#include "gui/types/function_option.hpp"
#include "helpers/sessionStarterHelpers.h"

namespace ui::submenus
{
	void session_starter_submenu::on_init()
	{
		for (const auto& [id, name] : sessions)
		{
			this->add(function_option(name, name, [&]
			{
				g_fiber_pool->queue_job([&]
				{
					session_starter::join_type(id);
				});
			}));
		}
	}

	session_starter_submenu g_session_starter_submenu("Session Starter", {});
}
