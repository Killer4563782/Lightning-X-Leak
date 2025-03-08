#include "input.h"

#include "commands/manager.hpp"
#include "memory/pointers.h"

static bool is_key_down(ImGuiKey primary_key, ImGuiKey secondary_key, bool both_needed)
{
	if (both_needed && primary_key == ImGuiKey_None && secondary_key == ImGuiKey_None)
		return false;

	return both_needed ?
		(primary_key == ImGuiKey_None || ImGui::IsKeyDown(primary_key)) && (secondary_key == ImGuiKey_None || ImGui::IsKeyDown(secondary_key)) :
		ImGui::IsKeyDown(primary_key) || ImGui::IsKeyDown(secondary_key);
}

Input::Input(ImGuiKey primary_key, ImGuiKey secondary_key, ImGuiKey primary_controller_key, ImGuiKey secondary_controller_key, bool enabled, bool both_needed, bool both_needed_controller, bool dont_repeat, float min_repeat_delay, float max_repeat_delay) :
	m_enabled(enabled),
	m_dont_repeat(dont_repeat),
	m_min_repeat_delay(min_repeat_delay),
	m_max_repeat_delay(max_repeat_delay),

	m_primary_key(primary_key),
	m_secondary_key(secondary_key),
	m_both_needed(both_needed),

	m_primary_controller_key(primary_controller_key),
	m_secondary_controller_key(secondary_controller_key),
	m_both_needed_controller(both_needed_controller)
{}

Input::Input(ImGuiKey primary_key, ImGuiKey secondary_key, ImGuiKey primary_controller_key, ImGuiKey secondary_controller_key, bool both_needed, bool both_needed_controller) :
	m_primary_key(primary_key),
	m_secondary_key(secondary_key),
	m_both_needed(both_needed),

	m_primary_controller_key(primary_controller_key),
	m_secondary_controller_key(secondary_controller_key),
	m_both_needed_controller(both_needed_controller)
{}

Input::Input(ImGuiKey primary_key, ImGuiKey secondary_key, ImGuiKey primary_controller_key, ImGuiKey secondary_controller_key, float max_repeat_delay, float min_repeat_delay, bool both_needed, bool both_needed_controller) :
	m_min_repeat_delay(min_repeat_delay),
	m_max_repeat_delay(max_repeat_delay),

	m_primary_key(primary_key),
	m_secondary_key(secondary_key),
	m_both_needed(both_needed),

	m_primary_controller_key(primary_controller_key),
	m_secondary_controller_key(secondary_controller_key),
	m_both_needed_controller(both_needed_controller)
{}


Input::Input(ImGuiKey key, ImGuiKey controller_key) : Input(key, ImGuiKey_None, controller_key, ImGuiKey_None)
{}


Input::operator bool()
{
	auto tick_count = GetTickCount64();

	if (!m_enabled || GetForegroundWindow() != g_pointers->g_hwnd || HUD::IS_MP_TEXT_CHAT_TYPING() || "disable_input"_LC->m_toggle)
		return false;

	if (is_key_down(m_primary_key, m_secondary_key, m_both_needed) || is_key_down(m_primary_controller_key, m_secondary_controller_key, m_both_needed_controller))
	{
		if (m_dont_repeat)
		{
			if (!m_last_down)
			{
				m_last_down = 1;
				return true;
			}

			return false;
		}

		if (!m_first_down)
			m_first_down = tick_count;

		auto down_time = static_cast<float>(tick_count - m_first_down);
		float next_down = m_max_repeat_delay;
		if (down_time > time_until_min_repeat_delay)
			next_down = m_min_repeat_delay;

		else if (down_time > time_until_min_repeat_delay / 2.5f)
			next_down = m_max_repeat_delay / 2.f;


		if (!m_last_down || m_last_down + static_cast<ULONGLONG>(next_down) <= tick_count)
		{
			m_last_down = tick_count;
			return true;
		}
	}
	else
	{
		m_first_down = 0;
		m_last_down = 0;
	}

	return false;
}

std::string Input::get_keyboard_key_string() const
{
	std::string keyboard_text;

	if (m_primary_key != ImGuiKey_None)
	{
		keyboard_text += std::string(ImGui::GetKeyName(m_primary_key));

		if (m_secondary_key != ImGuiKey_None)
			keyboard_text += (m_both_needed ? " and " : " or ") + std::string(ImGui::GetKeyName(m_secondary_key));
	}

	return keyboard_text;
}

std::string Input::get_gamepad_key_string() const
{
	std::string controller_text;

	if (m_primary_controller_key != ImGuiKey_None)
	{
		controller_text += std::string(ImGui::GetKeyName(m_primary_controller_key));

		if (m_secondary_controller_key != ImGuiKey_None)
			controller_text += (m_both_needed_controller ? " and " : " or ") + std::string(ImGui::GetKeyName(m_secondary_controller_key));
	}

	return controller_text;
}

std::string Input::get_key_string() const
{
	std::string keyboard_text = get_keyboard_key_string();
	std::string controller_text = get_gamepad_key_string();

	if (keyboard_text.empty() && controller_text.empty())
		return "Not Set";

	return keyboard_text + (!keyboard_text.empty() ? " | " : "") + controller_text;
}
