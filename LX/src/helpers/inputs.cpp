#include "inputs.h"

#include "gui/drawing.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "commands/manager.hpp"
#include "imgui/imgui_internal.h"
#include "renderer/renderer.h"

template <typename... Args>
using cb = std::function<void(Args...)>;

namespace text_input
{
	bool is_valid_char(const std::vector<char>& char_list, const char c)
	{
		return std::ranges::any_of(char_list, [c](const char& valid_char)
		{
			return std::tolower(valid_char) == std::tolower(c);
		});
	}

	int callback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter)
		{
			if (data->BufTextLen >= max_input_length)
				return 1;

			const char event_char = static_cast<char>(data->EventChar);
			bool allow = false;

			// check allowed text
			if (input_flags & InputFlags_Allow_Text && is_valid_char(chars, event_char))
				allow = true;

			// check allowed number
			if (input_flags & InputFlags_Allow_Number && is_valid_char(numbers, event_char))
				allow = true;

			// check allowed special
			if (input_flags & InputFlags_Allow_Special && is_valid_char(special, event_char))
				allow = true;

			if (!allow && (input_flags & InputFlags_Allow_Text || input_flags & InputFlags_Allow_Number || input_flags & InputFlags_Allow_Special))
				return 1;

			// check if it is a not allowed char (not in chars, numbers and special)
			if (!is_valid_char(chars, event_char) && !is_valid_char(numbers, event_char) && !is_valid_char(special, event_char))
				return 1;
		}

		if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
		{
			if (data->EventKey == ImGuiKey_UpArrow && history_pos < history.size() - 1)
			{
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history[++history_pos].c_str());
				return 1;
			}
			if (data->EventKey == ImGuiKey_DownArrow && history_pos > 0)
			{
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history[--history_pos].c_str());
				return 1;
			}
		}

		// Handle completion
		// TODO: Either add a completion list or when pressing tab multiple times it goes to next completion. Completion list is easier so fuck pressing multiple times
		if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion && data->BufTextLen > 0)
		{
			std::string input(data->Buf);
			auto split = ::split(input, ' ');
			if (std::string& last_word = split.back(); !last_word.empty())
			{
				// Command Completion
				if (input_flags & InputFlags_Completion_Command)
				{
					for (const auto& key : commands::g_commands | std::views::keys)
					{
						if (key.starts_with(last_word))
						{
							int last_word_start = static_cast<int>(input.find_last_of(' ') + 1);
							data->DeleteChars(last_word_start, data->BufTextLen - last_word_start);
							data->InsertChars(last_word_start, key.c_str());
							return 1;
						}
					}
				}

				// Player Name Completion
				if (auto mgr = *g_pointers->g_network_player_mgr; input_flags & InputFlags_Completion_Player && mgr)
				{
					for (const auto player : mgr->m_player_list)
					{
						if (player && to_lower(player->get_name()).starts_with(to_lower(last_word)))
						{
							int last_word_start = static_cast<int>(input.find_last_of(' ') + 1);
							data->DeleteChars(last_word_start, data->BufTextLen - last_word_start);
							data->InsertChars(last_word_start, player->get_name());
							return 1;
						}
					}
				}
			}
		}

		return 0;
	}

	void get_input(const std::string& title, const std::string& placeholder, int flags, const std::string& value, const cb<const std::string&>& input_cb)
	{
		input_flags = flags;
		history_pos = 0;
		"disable_input"_LC->enable();
		ImGui::GetIO().MouseDrawCursor = true;
		g_open = false;
		input = value;

		g_renderer->m_render_queue["text_input"] = [title, placeholder, input_cb]
		{
			ImGui::SetNextWindowPos(drawing::cords_to_screen({ 0.5f - 0.35f / 2.f, 0.5f - 0.3f }));
			if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::SetWindowFontScale(0.5f);

				constexpr ImGuiInputFlags gui_input_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere();

				ImGui::SetNextItemWidth(drawing::cords_to_screen({ 0.35f, 0.1f }).x);
				bool enter = placeholder.empty() ?
					ImGui::InputText("##TextInput", &input, gui_input_flags, callback) :
					ImGui::InputTextWithHint("##TextInput", placeholder.c_str(), &input, gui_input_flags, callback);


				bool confirm = ImGui::Button("Confirm");
				ImGui::SameLine();
				bool cancel = ImGui::Button("Cancel") || ImGui::IsKeyReleased(ImGuiKey_Escape);


				if (input_flags & InputFlags_Completion_Command && !input.empty())
				{
					auto first_word = split(input, ' ')[0];
					if (commands::g_commands.contains(first_word))
					{
						auto cmd = commands::g_commands[first_word];
						ImGui::Separator();
						ImGui::SetWindowFontScale(0.30f);
						ImGui::Text(cmd->m_name.c_str());

						if (!cmd->m_description.empty())
						{
							ImGui::SetWindowFontScale(0.25f);
							ImGui::Text(cmd->m_description.c_str());
						}
						ImGui::SetWindowFontScale(0.5f);
						ImGui::Separator();
					}
				}

				if (enter || confirm || cancel)
				{
					if (enter || confirm)
					{
						if (!(input_flags & InputFlags_Dont_Add_History) && !input.empty())
						{
							if (history.size() <= 1 || history[1] != input)
								history.insert(history.begin() + 1, input);
						}
						input_cb(input);
					}

					g_renderer->m_render_queue.erase("text_input");
					"disable_input"_LC->disable();
					ImGui::GetIO().MouseDrawCursor = false;
					g_open = true;
					return ImGui::End();
				}
			}
			ImGui::End();
		};
	}

	void get_input(const std::string& title, const std::string& placeholder, int flags, const cb<const std::string&>& input_cb)
	{
		get_input(title, placeholder, flags, "", input_cb);
	}
}

namespace color_input
{
	void get_color(const std::string& title, bool alpha, const ImVec4& value, const cb<const ImVec4&>& input_cb)
	{
		"disable_input"_LC->enable();
		ImGui::GetIO().MouseDrawCursor = true;
		g_open = false;
		input = value;

		g_renderer->m_render_queue["color_input"] = [title, alpha, input_cb]
		{
			ImGui::SetNextWindowPos(drawing::cords_to_screen({ 0.5f - 0.10f, 0.5f - 0.40f }));
			if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
			{
				ImGui::SetWindowFontScale(0.5f);

				int flags = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview;
				if (alpha) flags |= ImGuiColorEditFlags_AlphaBar;
				else flags |= ImGuiColorEditFlags_NoAlpha;

				ImGui::SetNextItemWidth(drawing::cords_to_screen({ alpha ? 0.20f : 0.205f, 0.1f }).x);
				ImGui::ColorPicker4("##InputColor", reinterpret_cast<float*>(&input), flags);
				const bool confirm = ImGui::Button("Confirm");
				ImGui::SameLine();

				if (const bool cancel = ImGui::Button("Cancel") || ImGui::IsKeyReleased(ImGuiKey_Escape); confirm || cancel)
				{
					if (confirm)
						input_cb(input);

					g_renderer->m_render_queue.erase("color_input");
					"disable_input"_LC->disable();
					ImGui::GetIO().MouseDrawCursor = false;
					g_open = true;
					ImGui::End();
					return;
				}
			}
			ImGui::End();
		};
	}

	inline void get_color(const std::string& title, bool alpha, const cb<const ImVec4&>& input_cb)
	{
		get_color(title, alpha, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), input_cb);
	}
}

namespace keybind_input
{
	void get_keybind(const std::string& title, bool is_keyboard, ImGuiKey primary, ImGuiKey secondary, const cb<ImGuiKey, ImGuiKey>& input_cb)
	{
		"disable_input"_LC->enable();
		ImGui::GetIO().MouseDrawCursor = true;
		g_open = false;
		primary_key = primary;
		secondary_key = secondary;

		g_renderer->m_render_queue["keybind_input"] = [title, is_keyboard, input_cb]
		{
			ImGui::SetNextWindowPos(drawing::cords_to_screen({ 0.5f - 0.35f / 2.f, 0.5f - 0.3f }));
			if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
			{
				ImGui::SetWindowFontScale(0.5f);

				for (int i = is_keyboard ? ImGuiKey_NamedKey_BEGIN : 631; i < (is_keyboard ? 631 : 655); i++)
				{
					if (auto key = static_cast<ImGuiKey>(i); ImGui::IsKeyDown(key) && key != ImGuiKey_End && key != ImGuiKey_Escape)
					{
						if (ImGui::IsKeyPressed(secondary_key, false) || !ImGui::IsKeyDown(primary_key) && key != secondary_key)
						{
							primary_key = ImGuiKey_None;
							secondary_key = ImGuiKey_None;
						}
						else if (ImGui::IsKeyPressed(primary_key, false) || !ImGui::IsKeyDown(secondary_key) && key != primary_key)
						{
							secondary_key = ImGuiKey_None;
						}

						if (primary_key == ImGuiKey_None)
						{
							secondary_key = ImGuiKey_None;
							primary_key = key;
						}
						else if (key != primary_key)
						{
							secondary_key = key;
						}
					}
				}

				if (primary_key != ImGuiKey_None)
				{
					input = ImGui::GetKeyName(primary_key);

					if (secondary_key != ImGuiKey_None)
						input += " + " + std::string(ImGui::GetKeyName(secondary_key));
				}
				else input = "Not Set";

				ImGui::SetNextItemWidth(drawing::cords_to_screen({ 0.35f, 0.1f }).x);
				ImGui::InputText("##KeybindInput", &input, ImGuiInputTextFlags_ReadOnly);


				bool confirm = ImGui::Button("Confirm");
				ImGui::SameLine();
				bool remove = ImGui::Button("Remove");
				ImGui::SameLine();
				bool cancel = ImGui::Button("Cancel") || ImGui::IsKeyReleased(ImGuiKey_Escape);


				if (confirm || remove || cancel)
				{
					if (confirm)
						input_cb(primary_key, secondary_key);

					if (remove)
						input_cb(ImGuiKey_None, ImGuiKey_None);

					g_renderer->m_render_queue.erase("keybind_input");
					"disable_input"_LC->disable();
					ImGui::GetIO().MouseDrawCursor = false;
					g_open = true;
					return ImGui::End();
				}
			}
			ImGui::End();
		};
	}

	void get_keybind(const std::string& title, bool is_keyboard, const cb<ImGuiKey, ImGuiKey>& input_cb)
	{
		get_keybind(title, is_keyboard, ImGuiKey_None, ImGuiKey_None, input_cb);
	}
}