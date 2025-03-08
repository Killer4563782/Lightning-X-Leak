#include "commands/settings.h"
#include "hooking/hooking.hpp"
#include "services/reactions/reactions.h"
#include <util/util.h>
#include <helpers/VehicleHelpers.h>

void hooks::received_clone_remove(CNetworkObjectMgr* mgr, CNetGamePlayer* src, CNetGamePlayer* dst, int16_t object_id, uint32_t ownership_token)
{
#ifdef _DEBUG
	if (commands::settings::debug::log_clone_remove)
		LOG_DEBUG("Clone Remove from '{} ({})', Object ID: {}", src->get_name(), src->m_account_id, object_id);
#endif

	auto plyr = g_player_manager->get_by_id(src->m_player_id);

	if (!dst || !dst->m_player_info || !dst->m_player_info->m_ped || !dst->m_player_info->m_ped->m_net_object)
	{
		if (crash.process(src, "Crash C9")) // invalid destination
			return;
	}

	if (!dst || !dst->m_player_info || !dst->m_player_info->m_ped || !dst->m_player_info->m_ped->m_net_object || dst->m_player_info->m_ped->m_net_object->m_object_id == object_id)
	{
		if (crash.process(src, "Crash C6")) // player ped removal
			return;
	}

	if (const auto personal_vehicle = vehicle::get_personal_cvehicle())
	{
		if (auto net_obj = personal_vehicle->m_net_object)
		{
			if (object_id == net_obj->m_object_id && util::natives::network_has_control_of_entity(net_obj))
			{
				/*LOG(VERBOSE) << "DELETION ORDER FOR PERSONAL VEHICLE: " << net_obj->m_object_id << " m_is_remote: " << net_obj->m_is_remote
				             << " m_bubble: " << net_obj->m_bubble << " m_owner_id: " << net_obj->m_owner_id
				             << " m_wants_to_be_owner: " << net_obj->m_wants_to_be_owner;*/
				if (plyr != nullptr)
				{
					//.reactions.delete_vehicle.process(plyr);
				}
				return;
			}
		}
	}

	if (self::vehicle != 0)
	{
		if (const auto current_vehicle = self::local_ped->m_vehicle)
		{
			auto net_obj = current_vehicle->m_net_object;
			if (object_id == net_obj->m_object_id && util::natives::network_has_control_of_entity(net_obj))
			{
				/*LOG(VERBOSE) << "DELETION ORDER FOR CURRENT VEHICLE: " << net_obj->m_object_id
				             << " m_is_remote: " << net_obj->m_is_remote << " m_bubble: " << net_obj->m_bubble
				             << " m_owner_id: " << net_obj->m_owner_id << " m_wants_to_be_owner: " << net_obj->m_wants_to_be_owner;*/
				if (plyr != nullptr)
				{
					//.reactions.delete_vehicle.process(plyr);
				}
				return;
			}
		}
	}

	if (auto object = g_pointers->g_get_net_object(*g_pointers->g_network_object_mgr, object_id, true))
	{
		if (object->m_object_type == (int)eNetObjType::NET_OBJ_TYPE_PLAYER && object->m_owner_id != src->m_player_id)
		{
			std::string target = "<UNKNOWN>";

			if (auto tgt = g_player_manager->get_by_id(object->m_owner_id))
				target = tgt->get_name();

			//LOGF(stream::net_sync, WARNING, "Rejecting clone remove from {}, who is trying to delete {}'s player ped", src->get_name(), target);
			return;
		}
	}

	g_hooking->get_original<received_clone_remove>()(mgr, src, dst, object_id, ownership_token);
}
