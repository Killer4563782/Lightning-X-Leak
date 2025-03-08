#pragma once
#include "player.hpp"

using player_ptr = std::shared_ptr<services::player>;
using player_entry = std::pair<std::string, player_ptr>;
using players = std::multimap<std::string, player_ptr>;

class player_manager final {
public:
	CNetGamePlayer** m_self;
	player_ptr m_self_ptr;
	players m_players;
	player_ptr m_dummy = std::make_shared<services::player>(nullptr);
	player_ptr m_selected_player;

	player_manager();
	~player_manager() = default;

	player_manager(const player_manager&) = delete;
	player_manager(player_manager&&) noexcept = delete;
	player_manager& operator=(const player_manager&) = delete;
	player_manager& operator=(player_manager&&) noexcept = delete;

	void do_cleanup();

	[[nodiscard]] player_ptr get_self();

	[[nodiscard]] player_ptr get_by_msg_id(uint32_t msg_id) const;
	[[nodiscard]] player_ptr get_by_id(uint32_t id) const;
	[[nodiscard]] player_ptr get_by_host_token(uint64_t token) const;
	[[nodiscard]] player_ptr get_selected() const;

	void player_join(CNetGamePlayer* net_game_player);
	void player_leave(CNetGamePlayer* net_game_player);

	players& players()
	{
		return m_players;
	}

	void iterate(const std::function<void(const player_entry& entry)>& func) const
	{
		for (const auto& iter : m_players)
			func(iter);
	}

	void set_selected(const player_ptr& plyr);
};

inline std::unique_ptr<player_manager> g_player_manager{};
