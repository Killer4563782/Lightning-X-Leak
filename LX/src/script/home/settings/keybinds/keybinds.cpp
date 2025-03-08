#include "keybinds.h"

#include "functions/input.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"

namespace ui::submenus
{
	keybind_submenu::keybind_submenu(std::string name, std::string description, Input* input, bool allow_disable_repeat) : submenu(std::move(name), std::move(description)),
		m_input(input),
		m_original_description(m_description),
		m_allow_disable_repeat(allow_disable_repeat)
	{
		m_description = get_description();
	}

	std::string keybind_submenu::get_description() const
	{
		std::string keyboard_text = m_input->get_keyboard_key_string();
		std::string controller_text = m_input->get_gamepad_key_string();

		if (keyboard_text.empty())
			keyboard_text = "Not Set";

		if (controller_text.empty())
			controller_text = "Not Set";

		return (m_original_description.empty() ? "" : m_original_description + "\n\n") + "Current Keybinds:\nKeyboard: " + keyboard_text + "\nController: " + controller_text;
	}


	void keybind_submenu::on_tick()
	{
		m_options.clear();
		m_description = get_description();

		if (m_allow_disable_repeat)
		{
			this->add(toggle_option("Dont Repeat", "If enabled the keybind wont be repeated and will only be counted as down once when holding down", &m_input->m_dont_repeat));
			if (!m_input->m_dont_repeat)
			{
				this->add(number_option("Min Repeat Delay", "The minimum delay to use when holding down this key for some time", &m_input->m_min_repeat_delay, 5000.f, 1.f, 50.f));
				this->add(number_option("Max Repeat Delay", "The maximum delay to use when holding down this key", &m_input->m_max_repeat_delay, 5000.f, 1.f, 50.f));
			}
		}
		this->add(break_option("Keyboard Key"));
		this->add(function_option("Change Keybind", {}, [this]
		{
			keybind_input::get_keybind("Change Keybind: " + m_name, true, m_input->m_primary_key, m_input->m_secondary_key, [this](ImGuiKey new_primary_key, ImGuiKey new_secondary_key)
			{
				m_input->m_primary_key = new_primary_key;
				m_input->m_secondary_key = new_secondary_key;
			});
		}));
		this->add(toggle_option("Both Needed", "Whether or not the primary and secondary key need to be down at the same time for this keybind to count as down", &m_input->m_both_needed));
		this->add(break_option("Controller Key"));
		this->add(function_option("Change Keybind", {}, [this]
		{
			keybind_input::get_keybind("Change Keybind: " + m_name, false, m_input->m_primary_controller_key, m_input->m_secondary_controller_key, [this](ImGuiKey new_primary_key, ImGuiKey new_secondary_key)
			{
				m_input->m_primary_controller_key = new_primary_key;
				m_input->m_secondary_controller_key = new_secondary_key;
			});
		}));
		this->add(toggle_option("Both Needed", "Whether or not the primary and secondary key need to be down at the same time for this keybind to count as down", &m_input->m_both_needed_controller));
	}

	void keybinds_submenu::on_init()
	{
		this->add(keybind_submenu("Open", "Open or close the menu", &handlers::key::open));
		this->add(keybind_submenu("Enter", "Run the action of the selected option", &handlers::key::enter));
		this->add(keybind_submenu("Back", "Go to the parent of the current submenu", &handlers::key::back));
		this->add(keybind_submenu("Up", "Move up in the menu", &handlers::key::up));
		this->add(keybind_submenu("Down", "Move down in the menu", &handlers::key::down));
		this->add(keybind_submenu("Left", "Change the value of the selected option", &handlers::key::left));
		this->add(keybind_submenu("Right", "Change the value of the selected option", &handlers::key::right));
		this->add(break_option("Freecam / No Clip"));
		this->add(keybind_submenu("Up", {}, &handlers::key::move_up, false));
		this->add(keybind_submenu("Down", {}, &handlers::key::move_down, false));
		this->add(keybind_submenu("Forward", {}, &handlers::key::move_forward, false));
		this->add(keybind_submenu("Backward", {}, &handlers::key::move_backward, false));
		this->add(keybind_submenu("Left", {}, &handlers::key::move_left, false));
		this->add(keybind_submenu("Right", {}, &handlers::key::move_right, false));
	}
}
