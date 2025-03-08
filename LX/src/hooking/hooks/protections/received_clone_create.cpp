#include "commands/settings.h"
#include "hooking/hooking.hpp"
#include "services/reactions/reactions.h"
#include <memory/pointers.h>
#include <rage/util.h>

void hooks::received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, int32_t object_id, int32_t object_flag, rage::datBitBuffer* buffer, int32_t timestamp)
{
#ifdef _DEBUG
	if (commands::settings::debug::log_clone_create)
		LOG_DEBUG("Clone Create from '{} ({})', Type: {}, Object ID: {}, Flags: {}", src->get_name(), src->m_account_id, static_cast<uint16_t>(object_type), object_id, object_flag);
#endif

	auto plyr = g_player_manager->get_by_id(src->m_player_id);

	if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
	{
		if (crash.process(src, "Crash C2")) // out of bounds object type
			return;
	}

	// can delete objects here too
	if (dst && dst->m_player_info && dst->m_player_info->m_ped && dst->m_player_info->m_ped->m_net_object && object_id == dst->m_player_info->m_ped->m_net_object->m_object_id)
	{
		return;
	}

	if (self::local_ped && self::local_ped->m_vehicle && self::local_ped->m_vehicle->m_net_object && object_id == self::local_ped->m_vehicle->m_net_object->m_object_id)
	{
		if (plyr != nullptr)
		{
			//g.reactions.delete_vehicle.process(plyr);
		}
		return;
	}

	if (auto object = g_pointers->g_get_net_object(*g_pointers->g_network_object_mgr, object_id, true))
	{
		if (object->m_object_type == (int)eNetObjType::NET_OBJ_TYPE_PLAYER && object->m_owner_id != src->m_player_id)
		{
			std::string target = "<UNKNOWN>";

			if (auto tgt = g_player_manager->get_by_id(object->m_owner_id))
				target = tgt->get_name();

			//LOGF(stream::net_sync, WARNING, "Rejecting clone create from {}, who is trying to delete {}'s player ped", src->get_name(), target);
			return;
		}
	}

	switch (object_type)
	{
		case eNetObjType::NET_OBJ_TYPE_AUTOMOBILE:
		case eNetObjType::NET_OBJ_TYPE_BIKE:
		case eNetObjType::NET_OBJ_TYPE_BOAT:
		case eNetObjType::NET_OBJ_TYPE_HELI:
		case eNetObjType::NET_OBJ_TYPE_PLANE:
		case eNetObjType::NET_OBJ_TYPE_SUBMARINE:
		case eNetObjType::NET_OBJ_TYPE_TRAILER:
		case eNetObjType::NET_OBJ_TYPE_TRAIN:
		if ((*g_pointers->g_vehicle_allocator)->m_size < 10)
		{
			// We don't have enough memory to handle this
			//g_notification_service.push_warning("Protections", "Low vehicle allocator size");
			return;
		}
	}

	if (get_net_object_ids()->is_object_id_usable(object_id))
	{
		//LOGF(stream::net_sync, WARNING, "{} sent us an object create request with an object ID that is in our usable object ID list. Somebody lied to us...", src->get_name());
		get_net_object_ids()->remove_object_id(object_id);
	}

	//if (plyr && plyr->block_clone_create)
	//	return;

	g_syncing_player = src;
	g_syncing_object_type = object_type;

	g_hooking->get_original<received_clone_create>()(mgr, src, dst, object_type, object_id, object_flag, buffer, timestamp);
}
