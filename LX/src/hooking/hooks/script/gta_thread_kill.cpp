#include "commands/settings.h"
#include "hooking/hooking.hpp"

rage::eThreadState hooks::gta_thread_kill(GtaThread* thread)
{
	const auto result = g_hooking->get_original<gta_thread_kill>()(thread);

#ifdef _DEBUG
	if (commands::settings::debug::log_thread_stop)
		LOG_DEBUG("Script Thread '{}' Terminated. Reason: '{}'", thread->m_name, thread->m_exit_message);
#endif

	/*if (thread == g.m_hunt_the_beast_thread)
		g.m_hunt_the_beast_thread = nullptr;

	if (thread == g.m_dance_thread)
		g.m_dance_thread = nullptr;

	if (thread == g.m_mission_creator_thread)
		g.m_mission_creator_thread = nullptr;

	if (thread == g.m_modshop_thread)
		g.m_modshop_thread = nullptr;*/

	return result;
}
