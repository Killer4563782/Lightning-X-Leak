#pragma once
#include "discord_rpc.h"

namespace discord
{
	inline DiscordUser g_user{};

	static int64_t time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	bool update_presence();
	bool start();
	void loop();
	void shutdown();
}
