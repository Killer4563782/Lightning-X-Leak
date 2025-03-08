#pragma once
#include "rate_limiter.hpp"
#include "rage/classes/network/CNetGamePlayer.hpp"
#include "rage/classes/network/netConnection.hpp"
#include "rage/classes/network/snSession.hpp"
#include "rage/classes/vehicle/CVehicle.hpp"

namespace services
{
	class player final
	{
	public:
		CNetGamePlayer* m_net_game_player = nullptr;
		bool m_is_friend;
		bool m_is_modder;

		explicit player(CNetGamePlayer* net_game_player);
		~player() = default;

		player(const player&) = default;
		player(player&&) noexcept = default;
		player& operator=(const player&) = default;
		player& operator=(player&&) noexcept = default;

		[[nodiscard]] CVehicle* get_current_vehicle() const;
		[[nodiscard]] std::string get_vehicle_name() const;
		[[nodiscard]] std::string get_vehicle_manufacture_name() const;
		[[nodiscard]] rage::fvector3* get_pos() const;
		[[nodiscard]] const char* get_name() const;
		[[nodiscard]] rage::rlGamerInfo* get_net_data() const;
		[[nodiscard]] int64_t get_rockstar_id() const;
		[[nodiscard]] CNetGamePlayer* get_net_game_player() const;
		[[nodiscard]] CPed* get_ped() const;
		[[nodiscard]] CPlayerInfo* get_player_info() const;
		[[nodiscard]] rage::snPlayer* get_session_player() const;
		[[nodiscard]] rage::snPeer* get_session_peer() const;
		[[nodiscard]] rage::netConnectionPeer* get_connection_peer() const;
		[[nodiscard]] std::optional<netAddress> get_ip_address() const;
		[[nodiscard]] uint16_t get_port() const;

		[[nodiscard]] uint8_t id() const;
		[[nodiscard]] int rank() const;

		[[nodiscard]] bool is_friend() const;
		[[nodiscard]] bool is_host() const;
		[[nodiscard]] bool is_valid() const;
		[[nodiscard]] bool is_script_host() const;

		rate_limiter m_host_migration_rate_limit{2s, 15};
		rate_limiter m_ptfx_rate_limit{1s, 10};
		rate_limiter m_play_sound_rate_limit{1s, 10};
		rate_limiter m_explosion_rate_limit{2s, 5};
		rate_limiter m_invites_rate_limit{10s, 2};
		rate_limiter m_radio_request_rate_limit{2s, 2};

		bool trigger_desync_kick = false;
		bool trigger_end_session_kick = false;
		bool tampered_with_be = false;

		bool equals(const CNetGamePlayer* net_game_player) const;

		[[nodiscard]] std::string to_lowercase_identifier() const;
	};
}
