#pragma once

template <typename... Args>
using cb = std::function<void(Args...)>;

namespace text_input
{
	const std::vector chars = {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
		'm', 'n','o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' '
	};

	const std::vector numbers = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '.', ',', ' '
	};

	const std::vector special = {
		'_', '-', '!', '?', '.', ',', ' '
	};

	// TODO: Maybe add vehicle model name completion for stuff like vehicle spawner and similar stuff or an arg with own completion list
	enum InputFlags_ : int
	{
		InputFlags_None = 0,
		InputFlags_Completion_Player = 1 << 0,
		InputFlags_Completion_Command = 1 << 1,
		InputFlags_All_Completion = InputFlags_Completion_Player | InputFlags_Completion_Command,
		InputFlags_Allow_Text = 1 << 2,
		InputFlags_Allow_Number = 1 << 3,
		InputFlags_Allow_Special = 1 << 4,
		InputFlags_Dont_Add_History = 1 << 5
	};


	bool is_valid_char(const std::vector<char>& char_list, char c);

	inline int input_flags = InputFlags_None;
	static size_t history_pos = 0;
	static std::vector<std::string> history{ "" };
	constexpr int max_input_length = 200;
	int callback(ImGuiInputTextCallbackData* data);

	inline std::string input;
	void get_input(const std::string& title, const std::string& placeholder, int flags, const std::string& value, const cb<const std::string&>& input_cb);
	void get_input(const std::string& title, const std::string& placeholder, int flags, const cb<const std::string&>& input_cb);
}

namespace color_input
{
	inline ImVec4 input;
	void get_color(const std::string& title, bool alpha, const ImVec4& value, const cb<const ImVec4&>& input_cb);
	void get_color(const std::string& title, bool alpha, const cb<const ImVec4&>& input_cb);
}

namespace keybind_input
{
	inline std::string input;
	inline ImGuiKey primary_key;
	inline ImGuiKey secondary_key;
	void get_keybind(const std::string& title, bool is_keyboard, ImGuiKey primary, ImGuiKey secondary, const cb<ImGuiKey, ImGuiKey>& input_cb);
	void get_keybind(const std::string& title, bool is_keyboard, const cb<ImGuiKey, ImGuiKey>& input_cb);
}