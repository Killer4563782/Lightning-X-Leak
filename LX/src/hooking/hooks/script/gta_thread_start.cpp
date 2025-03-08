#include "commands/settings.h"
#include "hooking/hooking.hpp"

GtaThread* hooks::gta_thread_start(unsigned int** a1, unsigned int a2)
{
	GtaThread* new_thread = g_hooking->get_original<gta_thread_start>()(a1, a2);
	const char* name = new_thread->m_name;

#ifdef _DEBUG
	if (commands::settings::debug::log_thread_start)
	{
		if (strlen(name) > 0)
			LOG_DEBUG("Script Thread '{}' Started", name);
		else
			LOG_DEBUG("Script Thread with no name Started");
	}
#endif

	return new_thread;
}
