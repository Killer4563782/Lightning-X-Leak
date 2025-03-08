#pragma once

float constexpr time_until_min_repeat_delay = 3000.f;

class Input
{
public:
	explicit Input(ImGuiKey primary_key, ImGuiKey secondary_key, ImGuiKey primary_controller_key, ImGuiKey secondary_controller_key, bool enabled, bool both_needed, bool both_needed_controller, bool dont_repeat = false, float min_repeat_delay = 50.f, float max_repeat_delay = 300.f);
	explicit Input(ImGuiKey primary_key, ImGuiKey secondary_key, ImGuiKey primary_controller_key, ImGuiKey secondary_controller_key, bool both_needed = false, bool both_needed_controller = false);
	explicit Input(ImGuiKey primary_key, ImGuiKey secondary_key, ImGuiKey primary_controller_key, ImGuiKey secondary_controller_key, float max_repeat_delay, float min_repeat_delay = 50.f, bool both_needed = false, bool both_needed_controller = false);
	explicit Input(ImGuiKey key, ImGuiKey controller_key);

	explicit operator bool();

	std::string get_keyboard_key_string() const;
	std::string get_gamepad_key_string() const;
	std::string get_key_string() const;

	bool m_enabled = true;
	bool m_dont_repeat = false;
	float m_min_repeat_delay = 50.f;
	float m_max_repeat_delay = 300.f;

	ImGuiKey m_primary_key;
	ImGuiKey m_secondary_key;
	bool m_both_needed;

	ImGuiKey m_primary_controller_key;
	ImGuiKey m_secondary_controller_key;
	bool m_both_needed_controller;

	ULONGLONG m_first_down = 0;
	ULONGLONG m_last_down = 0;
};
