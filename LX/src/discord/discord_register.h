#pragma once
#define DISCORD_EXPORT

extern "C" {
DISCORD_EXPORT void Discord_Register(const char* applicationId, const char* command);
DISCORD_EXPORT void Discord_RegisterSteamGame(const char* applicationId, const char* steamId);
}
