#include "commands/manager.hpp"
#include "hooking/hooking.hpp"
#include <commands/settings.h>

bool hooks::rage_rlScMatchmaking_Advertise(int localGamerIndex, uint32_t numSlots, uint32_t availableSlots, const rage::rlMatchingAttributes* attrs, uint64_t sessionId, const rage::rlSessionInfo* sessionInfo, rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status)
{
	if (commands::settings::session::player_magnet)
	{
		status->m_Status = rage::netStatus::PENDING;

		g_fiber_pool->queue_job([=]
		{
			// Publishing a legitimate advertisement allows us to use rage_rlScMatchmaking_Unadvertise to attach multiplexed advertisement cleanup to legitimate advertisement cleanup.
			rage::netStatus legitimate;
			if (g_hooking->get_original<rage_rlScMatchmaking_Advertise>()(localGamerIndex, numSlots, availableSlots, attrs, sessionId, sessionInfo, matchId, &legitimate))
			{
				status->m_Status = rage::netStatus::FAILED;
				return;
			}

			legitimate.waitUntilDone();
			if (legitimate.m_StatusCode != rage::netStatus::SUCCEEDED)
			{
				status->m_Status = rage::netStatus::FAILED;
				return;
			}

			for (uint8_t i = 0; i != 10; ++i)
			{
				rage::netStatus multiplex_status{};
				rage::rlScMatchmakingMatchId id{};
				if (g_hooking->get_original<rage_rlScMatchmaking_Advertise>()(localGamerIndex, numSlots, availableSlots, attrs, sessionId, sessionInfo, &id, &multiplex_status))
				{
					multiplex_status.waitUntilDone();
					if (multiplex_status.m_StatusCode == rage::netStatus::SUCCEEDED)
					{
						session_advertisements.emplace_back(std::move(id));
					}
				}
			}

			status->m_StatusCode = rage::netStatus::SUCCEEDED;
		});

		return true;
	}

	return g_hooking->get_original<rage_rlScMatchmaking_Advertise>()(localGamerIndex, numSlots, availableSlots, attrs, sessionId, sessionInfo, matchId, status);
}
bool hooks::rage_rlScMatchmaking_Unadvertise(  int localGamerIndex, const rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status)
{
	if (commands::settings::session::player_magnet)
	{
		bool found = false;

		for (const auto& id : session_advertisements)
		{
			if (std::equal(std::begin(id.m_Guid), std::end(id.m_Guid), std::begin(matchId->m_Guid), std::end(matchId->m_Guid)))
			{
				found = true;
				break;
			}
		}

		if (!found) // It's a legitimate advertisement, we're leaving the session.
		{
			status->m_StatusCode = rage::netStatus::PENDING;
			g_fiber_pool->queue_job([=]
			{
				rage::netStatus legitimate_status{};
				g_hooking->get_original<rage_rlScMatchmaking_Unadvertise>()(localGamerIndex, matchId, &legitimate_status);

				legitimate_status.waitUntilDone();
				for (const auto& id : session_advertisements)
				{
					rage::netStatus status{};

					if (g_hooking->get_original<hooks::rage_rlScMatchmaking_Unadvertise>()(localGamerIndex, &id, &status))
					{
						status.waitUntilDone();
					}
				}

				session_advertisements.clear();

				status->m_StatusCode = rage::netStatus::SUCCEEDED;
			});

			return true;
		}

		// They don't take no for an answer (it throws an error if we say this operation failed).
		status->m_StatusCode = rage::netStatus::SUCCEEDED;
		return true;
	}

	return g_hooking->get_original<rage_rlScMatchmaking_Unadvertise>()(localGamerIndex, matchId, status);
}


bool hooks::rage_netTransactor_SendResponse_rage_rlSessionDetailResponse(void* /* rage::netTransactor */ _this, const void* /* rage::netTransactionInfo */ txInfo, rage::rlSessionDetailResponse* msg)
{
	if (commands::settings::session::player_magnet && msg->m_Response == 5) /* RESPONSE_HAVE_DETAILS */
	{
		msg->m_NumFilledPublicSlots = std::max(25u, msg->m_NumFilledPublicSlots);
	}

	return g_hooking->get_original<rage_netTransactor_SendResponse_rage_rlSessionDetailResponse>()(_this, txInfo, msg);
}