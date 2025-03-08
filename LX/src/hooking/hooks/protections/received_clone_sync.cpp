#include "commands/settings.h"
#include "hooking/hooking.hpp"
#include "memory/pointers.h"
#include "services/reactions/reactions.h"

NetObjectAckCode hooks::received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, eNetObjType object_type, uint16_t object_id, rage::datBitBuffer* buffer, uint16_t unk, uint32_t timestamp)
{
#ifdef _DEBUG
	if (commands::settings::debug::log_clone_sync)
		LOG_DEBUG("Clone Sync from '{} ({})', Type: {}, Object ID: {}", src->get_name(), src->m_account_id, static_cast<uint16_t>(object_type), object_id);
#endif

	if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
	{
		if (crash.process(src, "Crash C5")) // out of bounds object type
			return NetObjectAckCode::FAIL;
	}

	if (const auto net_obj = g_pointers->g_get_net_object(mgr, object_id, true); net_obj && net_obj->m_object_type != static_cast<int16_t>(object_type))
	{
		if (crash.process(src, "Crash C3")) // incorrect object type
			return NetObjectAckCode::FAIL;
	}

	// can be used to crash or reverse sync player data
	if (dst && dst->m_player_info && dst->m_player_info->m_ped && dst->m_player_info->m_ped->m_net_object && object_id == dst->m_player_info->m_ped->m_net_object->m_object_id)
	{
		//notify::crash_blocked(src, "player sync");
		return NetObjectAckCode::FAIL;
	}

	if (auto object = g_pointers->g_get_net_object(*g_pointers->g_network_object_mgr, object_id, true))
	{
		if (object->m_object_type == (int)eNetObjType::NET_OBJ_TYPE_PLAYER && object->m_owner_id != src->m_player_id)
		{
			std::string target = "<UNKNOWN>";

			if (auto tgt = g_player_manager->get_by_id(object->m_owner_id))
				target = tgt->get_name();

			//LOGF(stream::net_sync, WARNING, "Rejecting clone sync from {}, who is trying to sync to {}'s player ped", src->get_name(), target);
			return NetObjectAckCode::FAIL;
		}
	}

	const auto ret = g_hooking->get_original<received_clone_sync>()(mgr, src, dst, object_type, object_id, buffer, unk, timestamp);

	return ret;
}
