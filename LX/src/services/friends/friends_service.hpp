#pragma once

class friends_service final {
public:
	friends_service() = default;
	~friends_service() = default;

	[[nodiscard]] static bool is_friend(int64_t rockstar_id);
	[[nodiscard]] static bool is_friend(CNetGamePlayer* net_player);
};

inline std::unique_ptr<friends_service> g_friends_service{};