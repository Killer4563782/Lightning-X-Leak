#pragma once
#include "memory/pointers.h"

inline CPed* get_local_ped()
{
	if (const auto ped_factory = *g_pointers->g_ped_factory)
	{
		return ped_factory->m_local_ped;
	}

	return nullptr;
}

inline CVehicle* get_local_vehicle()
{
	if (const auto ped = get_local_ped(); ped)
	{
		if (const auto veh = ped->m_vehicle; veh)
		{
			return veh;
		}
	}
	return nullptr;
}

inline CPlayerInfo* get_local_playerinfo()
{
	if (const auto ped_factory = *g_pointers->g_ped_factory)
	{
		if (const auto ped = ped_factory->m_local_ped)
		{
			return ped->m_player_info;
		}
	}

	return nullptr;
}

inline CNetworkPlayerMgr* get_network_player_mgr()
{
	return *g_pointers->g_network_player_mgr;
}

inline Network* get_network()
{
	return *g_pointers->g_network;
}

inline CNetworkObjectMgr* get_object_mgr() {
	return *g_pointers->g_network_object_mgr;
}

inline rage::netObjectIds* get_net_object_ids()
{
	if (!get_object_mgr())
		return nullptr;

	return (rage::netObjectIds*)(((std::uintptr_t)get_object_mgr()) + *g_pointers->g_object_ids_offset); // TODO: map out CNetworkObjectMgr eventually
}

template <typename F, typename... Args>
void execute_as_script(rage::scrThread* thread, F&& callback, Args&&... args)
{
	const auto tls_ctx = rage::tlsContext::get();
	const auto og_thread = tls_ctx->m_script_thread;

	tls_ctx->m_script_thread = thread;
	tls_ctx->m_is_script_thread_active = true;

	std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

	tls_ctx->m_script_thread = og_thread;
	tls_ctx->m_is_script_thread_active = og_thread != nullptr;
}

template <typename F, typename... Args>
void execute_as_script(const rage::joaat_t script_hash, F&& callback, Args&&... args)
{
	for (auto thread : *g_pointers->g_script_threads)
	{
		if (!thread || !thread->m_context.m_thread_id || thread->m_context.m_script_hash != script_hash)
			continue;

		execute_as_script(thread, callback, args...);

		return;
	}
}

inline GtaThread* find_script_thread(const rage::joaat_t hash)
{
	for (const auto thread : *g_pointers->g_script_threads)
	{
		if (thread && thread->m_context.m_thread_id && thread->m_handler && thread->m_script_hash == hash)
		{
			return thread;
		}
	}

	return nullptr;
}

inline GtaThread* find_script_thread_by_id(const uint32_t id)
{
	for (const auto thread : *g_pointers->g_script_threads)
	{
		if (thread && thread->m_handler && thread->m_context.m_thread_id == id)
		{
			return thread;
		}
	}

	return nullptr;
}

inline rage::scrProgram* find_script_program(const rage::joaat_t hash)
{
	for (const auto& [m_program, m_Pad1, m_hash] : *g_pointers->g_scr_program_table)
	{
		if (m_program && m_program->m_name_hash == hash)
			return m_program;
	}

	return nullptr;
}

template <typename T>
void clear_bit(T* address, int pos)
{
	*address &= ~(1 << pos);
}

template <typename T>
void clear_bits(T* address, int bits)
{
	*address &= ~bits;
}

template <typename T>
bool has_bit_set(T* address, int pos)
{
	return *address & 1 << pos;
}

template <typename T>
bool has_bits_set(T* address, T bits)
{
	return (*address & bits) == bits;
}

template <typename T>
void set_bit(T* address, int pos)
{
	*address |= 1 << pos;
}

template <typename T>
void set_bits(T* address, int bits)
{
	*address |= bits;
}