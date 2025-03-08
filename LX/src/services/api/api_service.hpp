#pragma once
#include <json/json.h>
#include "memory/pointers.h"

inline std::string get_ticket()
{
	return g_pointers->g_sc_info->m_ticket;
}

inline std::string get_authorization_ticket()
{
	return std::format("SCAUTH val=\"{}\"", get_ticket());
}

namespace rage
{
	class rlPresenceEventInviteAccepted {
	public:
		rlPresenceEventInviteAccepted(const rlGamerInfo& gamer_info, const rlSessionInfo& session_info, const rlGamerHandle& gamer_handle) : m_gamer_info(gamer_info), m_session_info(session_info), m_gamer_handle(gamer_handle)
		{
			m_self = this;
		}

		void* m_vft; //0x0000
		rlPresenceEventInviteAccepted* m_self; //0x0008
		rlGamerInfo m_gamer_info; //0x0010
		rlSessionInfo m_session_info; //0x0108
		rlGamerHandle m_gamer_handle; //0x01D8
	}; //Size: 0x01E8 // new == 0x1E0
	static_assert(sizeof(rlPresenceEventInviteAccepted) == 0x1E0);
}

class api_service {
public:
	api_service() = default;
	~api_service() = default;

	// Returns true if an valid profile matching his username has been found
	static uint64_t get_rid_from_username(std::string username);

	// Returns true if an valid profile matching his rid has been found
	static std::string get_username_from_rid(uint64_t rid);

	// Returns true if the message has been successfully sended to the target
	bool send_socialclub_message(uint64_t rid, std::string_view message);

	static void join_session(const rage::rlSessionInfo& info);

	static bool is_rid_online(uint64_t rid);

	static void rid_join(uint64_t rid);

	static void rid_invite(uint64_t rid);


	int rid_joiner_idx{};
	std::vector<std::string> rid_joiner_type{
		"Via Host",
		"Via Presence Invite"
	};

	bool join_queued = false;
	rage::rlSessionInfo queued_info;
private:
	const std::vector<std::string> languages = {"unset", "en", "fr", "de", "it", "es", "pt", "pl", "ru", "es-mx"};
};

inline std::unique_ptr<api_service> g_api_service{};