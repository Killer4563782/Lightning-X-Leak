#pragma once

#include "rage/rl.h"
#include "rage/rlGamerInfo.hpp"

#include "rage/rlGamerHandle.hpp"
#include <rage/rlPeerInfo.h>
#include <network/snSession.hpp>

namespace rage
{


	struct rlSessionDetailResponse
	{
		uint32_t m_Response;
		uint32_t m_ResponseParam;
		uint32_t m_UniqueID;
		netPeerAddress m_peer_id;
		rlSessionInfo m_SessionInfo;
		rlSessionConfig m_Config;
		rlGamerHandle m_HostHandle;
		char m_HostName[RL_MAX_NAME_BUF_SIZE];
		uint32_t m_NumFilledPublicSlots;
		uint32_t m_NumFilledPrivateSlots;
	};
}
