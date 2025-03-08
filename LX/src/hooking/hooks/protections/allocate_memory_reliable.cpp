#include "hooking/hooking.hpp"
#include "memory/pointers.h"
#include "rage/gta/net_game_event.hpp"

bool is_reliable_message(const rage::netQueuedMessage* msg)
{
	rage::datBitBuffer buffer(msg->m_data_buffer, INT_MAX);
	buffer.m_flagBits |= 1;    // read
	buffer.Seek(10);           // size
	return buffer.ReadBits(1); // flags
}

void free_message(rage::netQueuedMessage* msg, rage::sysMemAllocator* allocator)
{
	if (msg->m_data_buffer)
		allocator->TryFree(msg->m_data_buffer);

	allocator->TryFree(msg);
}

void* hooks::allocate_memory_reliable(rage::netConnection* cxn, int required_memory)
{
	if (!cxn || !required_memory)
		return nullptr;

	auto memory = reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator)->Allocate(required_memory, 0, 0);

	if (memory)
		return memory;

	LOG_DEBUG("Failed to allocate {}, bytes for reliable message, free space: {}. Trying to free some memory", required_memory, reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator)->GetMemoryAvailable());

	g_pointers->g_connection_manager_try_free_memory(cxn->m_net_connection_mgr);

	memory = reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator)->Allocate(required_memory, 0, 0);

	if (memory)
		return memory;

	LOG_DEBUG("Failed to allocate {} bytes for reliable message, free space: {}. Failed to free some memory, clearing all messages (including reliables) for connection", required_memory, reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator)->GetMemoryAvailable());

	while (cxn->m_normal_message_queue.m_count)
	{
		const auto msg = cxn->m_normal_message_queue.m_first;

		g_pointers->g_remove_message_from_queue(&cxn->m_normal_message_queue, msg);

		if (is_reliable_message(msg))
			g_pointers->g_remove_message_from_unacked_reliables(&cxn->m_unacked_reliable_message_list, &msg->word4C);

		free_message(msg, reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator));
	}

	while (cxn->m_reliables_resend_queue.m_count)
	{
		const auto msg = cxn->m_reliables_resend_queue.m_first;

		g_pointers->g_remove_message_from_queue(&cxn->m_reliables_resend_queue, msg);
		g_pointers->g_remove_message_from_unacked_reliables(&cxn->m_unacked_reliable_message_list, &msg->word4C); // messages in this queue are always reliables
		free_message(msg, reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator));
	}


	memory = reinterpret_cast<rage::sysMemAllocator*>(cxn->m_allocator)->Allocate(required_memory, 0, 0);

	if (memory)
		return memory;

	LOG_DEBUG("The network message allocator is out of memory");

	return nullptr;
}
