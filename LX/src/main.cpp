#include "commands/patches/byte_patch_manager.hpp"
#include "commands/manager.hpp"
#include "core/util.hpp"
#include "core/exception_handler/handler.h"
#include "core/security/security.h"
#include "discord/discord.h"
#include "fiber/fiber_pool.hpp"
#include "fiber/script_mgr.hpp"
#include "hooking/hooking.hpp"
#include "hooking/nativeHooking/native_hooks.hpp"
#include "memory/pointers.h"
#include "renderer/renderer.h"
#include "services/custom_text/custom_text_service.hpp"
#include "services/entity_preview/entity_preview.h"
#include "services/script_patcher/script_patcher_service.hpp"
#include "thread/threadpool.h"
#include "util/vehicle_spawner.h"
#include "services/api/api_service.hpp"
#include "services/friends/friends_service.hpp"
#include "helpers/onScreenText.h"
#include "services/online/battleye.h"
#include "util/util.h"
using namespace std::chrono_literals;

void initialize_services()
{
	g_font_mgr = std::make_unique<font_mgr>();
	g_byte_patch_manager = std::make_unique<byte_patch_manager>();
	g_renderer = std::make_unique<renderer>();
	g_fiber_pool = std::make_unique<fiber_pool>(11);
	g_script_patcher_service = std::make_unique<script_patcher_service>();
	g_custom_text_service = std::make_unique<custom_text_service>();
	g_api_service = std::make_unique<api_service>();
	g_friends_service = std::make_unique<friends_service>();
	g_player_manager = std::make_unique<player_manager>();
	g_hooking = std::make_unique<hooking>();
	g_hooking->enable();
	g_native_hooks = std::make_unique<native_hooks>();
	util::vehicle::cache_model_table();
}

void register_scripts()
{
	g_script_mgr.add_script(std::make_unique<script>(&commands::manager::init, "commands", false));
#ifndef _DEBUG
	g_script_mgr.add_script(std::make_unique<script>([]
	{
		on_screen_text::welcome();
		g_open = true;
	}, "Welcome", false));
#else
    g_open = true; // Skip welcome screen in debug mode
#endif
}

void cleanup()
{
	discord::shutdown();
	g_thread_pool->destroy();
	g_thread_pool.reset();
	g_pointers.reset();
	exceptions::uninit_exception_handler();
	g_logger.reset();
	CloseHandle(g_thread);
	FreeLibraryAndExitThread(g_module, 0);
	commands::g_battleye_service.stop();
}


void init()
{
	if (security::check_for_debugger())
	{
		// Mfs caught with a debugger ON!
		TerminateProcess(GetCurrentProcess(), NULL);
		return;
	}

	exceptions::init_exception_handler();
	core::util::initialize_lx_path_folder();
	g_logger = std::make_unique<logger>(BRAND " " MENU_VERSION, g_lx_path);
	g_thread_pool = std::make_unique<thread_pool>();
	g_pointers = std::make_unique<pointers>();

	if (!core::util::supported_game_version())
	{
		cleanup();
		return;
	}

	util::wait_for_splash_screen_end();
	security::defeat_guardit();
	security::attempt_to_yeet_rage_sec();
	util::wait_for_game_load();
	initialize_services();
	register_scripts();
	discord::start();
	commands::g_battleye_service.start(); 

}

void loop()
{
	while (g_running)
	{
		discord::loop();
		if (GetAsyncKeyState(VK_END))
		{
			// Reset all player modifications
			if (g_script_mgr.can_tick())
			{
				g_fiber_pool->reset();
				g_fiber_pool->queue_job([]
				{
					commands::manager::un_init();
					entity_preview::ped::clean_up();
					entity_preview::vehicle::clean_up();
					GRAPHICS::TRIGGER_SCREENBLUR_FADE_OUT(0); // Remove blur from welcome screen
					g_running = false;
				});
			}
			else
			{
				g_running = false;
			}
		}
	}
}

void un_init()
{
	LOG(info, "Unloading LX...");
	discord::shutdown();
	g_script_mgr.remove_all_scripts();
	g_hooking->disable();
	g_native_hooks.reset();
	g_thread_pool->destroy();
	g_scripting->uninitialize();
	g_scripting.reset();
	g_script_patcher_service.reset();
	g_custom_text_service.reset();
	g_api_service.reset();
	g_friends_service.reset();
	g_player_manager.reset();
	g_hooking.reset();
	g_fiber_pool.reset();
	g_renderer.reset();
	g_font_mgr.reset();
	g_byte_patch_manager.reset();
	g_pointers.reset();
	g_thread_pool.reset();
	security::uninit();
	exceptions::uninit_exception_handler();
	g_logger.reset();
}

DWORD entry_point(LPVOID)
{
	init();
	loop();
	un_init();
	CloseHandle(g_thread);
	FreeLibraryAndExitThread(g_module, 0);
}

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason_for_call, LPVOID)
{
	g_module = module;
	security::init();

	if (reason_for_call == DLL_PROCESS_ATTACH)
	{
		g_thread = CreateThread(nullptr, NULL, &entry_point, nullptr, NULL, nullptr);
	}

	return TRUE;
}