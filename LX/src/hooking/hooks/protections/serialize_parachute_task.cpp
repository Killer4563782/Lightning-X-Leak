#include "hooking/hooking.hpp"
#include "memory/pointers.h"
#include "services/reactions/reactions.h"

#pragma pack(push, 8)
struct TaskObject
{
	uint16_t m_net_id;
	rage::CDynamicEntity* m_entity;
};
#pragma pack(pop)

void hooks::serialize_parachute_task(__int64 info, rage::CSyncDataBase* serializer)
{
	const auto object = reinterpret_cast<TaskObject*>(info + 0x30);

	if (object->m_entity && object->m_entity->m_entity_type != 5)
	{
		if (crash.process(g_syncing_player, "Crash P5"))
		{
			object->m_entity = nullptr;
			object->m_net_id = 0;
			g_pointers->g_remove_reference(object->m_entity, &object->m_entity);
		}
	}

	return g_hooking->get_original<serialize_parachute_task>()(info, serializer);
}
