#include "discord.h"
#include "core/security/security.h"

namespace discord
{
	void on_ready(const DiscordUser* init_user)
	{
		if (g_user.userId != init_user->userId)
		{
			g_user.avatar = init_user->avatar;
			g_user.discriminator = init_user->discriminator;
			g_user.userId = init_user->userId;
			g_user.username = init_user->username;

			LOG_DEBUG("Initialized discord with user: {} | ID: {}", g_user.username, g_user.userId);
		}
	}

	bool update_presence()
	{
	
	}

	bool start()
	{
		DiscordEventHandlers handlers{};
		handlers.ready = on_ready;

		return update_presence();
	}

	void loop()
	{
		Discord_RunCallbacks();
	}

	void shutdown()
	{
		Discord_Shutdown();
	}
}
