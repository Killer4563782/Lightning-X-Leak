#include "invoker.h"

#include "memory/pointers.h"
#include "rage/commands/conversion_table.h"

void native_invoker::fix_vectors()
{
	m_call_context.copy_params_out();
}

void native_invoker::cache_handlers()
{
	if (m_are_handlers_cached)
		return;
	
	for (size_t i = 0; i < g_crossmap.size(); i++)
	{
		m_handlers[i] = g_pointers->g_get_native_handler(g_pointers->g_native_registration_table, g_crossmap[i]);
	}

	LOG(info, "Cached {} native handlers", g_crossmap.size());

	m_are_handlers_cached = true;
}