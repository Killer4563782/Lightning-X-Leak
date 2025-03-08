#include "script_mgr.hpp"
#include "rage/util.h"

void script_mgr::add_script(std::unique_ptr<script> script)
{
	std::lock_guard lock(m_mutex);

	m_scripts.push_back(std::move(script));
}


void script_mgr::remove_all_scripts()
{
	std::lock_guard lock(m_mutex);

	m_scripts.clear();
}

void script_mgr::for_each_script(auto func)
{
	std::lock_guard lock(m_mutex);

	for (const auto& script : m_scripts)
	{
		func(script);
	}
}

void script_mgr::tick()
{
	execute_as_script(find_script_thread("main_persistent"_joaat), std::mem_fn(&script_mgr::tick_internal), this);
}

void script_mgr::ensure_main_fiber()
{
	ConvertThreadToFiber(nullptr);

	m_can_tick = true;
}

void script_mgr::tick_internal()
{
	static bool ensure_it = (ensure_main_fiber(), true);

	std::lock_guard lock(m_mutex);

	for (const auto& script : m_scripts)
	{
		if (script->is_enabled())
			script->tick();
	}
}
