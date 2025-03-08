#ifdef _DEBUG
#include "debug_sub.h"

#include "commands/settings.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "helpers/inputs.h"

namespace ui::submenus
{
	void debug_submenu::on_init()
	{
		this->add(break_option("Events"));
		this->add(toggle_option("Log Events", {}, &commands::settings::debug::log_events));
		this->add(toggle_option("Log Script Events", {}, &commands::settings::debug::log_script_events));

		this->add(break_option("Protection Related"));
		this->add(toggle_option("Log Nodes", {}, &commands::settings::debug::log_nodes));
		this->add(toggle_option("Log Clone Create", {}, &commands::settings::debug::log_clone_create));
		this->add(toggle_option("Log Clone Remove", {}, &commands::settings::debug::log_clone_remove));
		this->add(toggle_option("Log Clone Sync", {}, &commands::settings::debug::log_clone_sync));

		this->add(break_option("Info"));
		this->add(toggle_option("Log Thread Start", {}, &commands::settings::debug::log_thread_start));
		this->add(toggle_option("Log Thread Stop", {}, &commands::settings::debug::log_thread_stop));
		this->add(toggle_option("Log Metrics", {}, &commands::settings::debug::log_metrics));
		this->add(toggle_option("Log Packets", {}, &commands::settings::debug::log_packets));

		this->add(break_option("Other"));
		this->add(function_option("Exception", "Throw a exception", []
		{
			int* a = nullptr;
			*a = 69;
		}));
		this->add(break_option("Input"));
		static std::string test_string = "Test";
		this->add(function_option("Text Input", "Test the get_input function", []
		{
			get_input("Text Test", "Placeholder", text_input::InputFlags_All_Completion, test_string, [](const std::string& input)
			{
				test_string = input;
				notify(Info, 3s, "Input: {}", input);
			});
		}));
		static color test_color = { 255, 255, 255, 255 };
		this->add(function_option("Color Input", "Test the get_color function", []
		{
			color_input::get_color(std::string("Color Test"), true, test_color, [](const color& input)
			{
				test_color = input;
				notify(Info, 3s, "Color: R: {}, G: {}, B: {}, A: {}", input.r, input.g, input.b, input.a);
			});
		}));
		static ImGuiKey primary_key = ImGuiKey_None;
		static ImGuiKey secondary_key = ImGuiKey_None;
		this->add(function_option("Keybind Input (Keyboard)", "Test the get_keybind function", []
		{
			keybind_input::get_keybind("Keybind Test (Keyboard)", true, primary_key, secondary_key, [](ImGuiKey new_primary, ImGuiKey new_secondary)
			{
				primary_key = new_primary;
				secondary_key = new_secondary;
				notify(Info, 3s, "Primary: {}, Secondary: {}", ImGui::GetKeyName(new_primary), ImGui::GetKeyName(new_secondary));
			});
		}));
		static ImGuiKey primary_controller_key = ImGuiKey_None;
		static ImGuiKey secondary_controller_key = ImGuiKey_None;
		this->add(function_option("Keybind Input (Gamepad)", "Test the get_keybind function", []
		{
			keybind_input::get_keybind("Keybind Test (Gamepad)", false, primary_controller_key, secondary_controller_key, [](ImGuiKey new_primary, ImGuiKey new_secondary)
			{
				primary_controller_key = new_primary;
				secondary_controller_key = new_secondary;
				notify(Info, 3s, "Primary: {}, Secondary: {}", ImGui::GetKeyName(new_primary), ImGui::GetKeyName(new_secondary));
			});
		}));
	}

	debug_submenu g_debug_submenu("Debug", "Debug Options");
}
#endif