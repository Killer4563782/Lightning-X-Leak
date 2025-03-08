#include "notification.h"

#include "imgui/imgui_internal.h"

ImVec4 ImGuiToast::get_color() const
{
	switch (this->type)
	{
		case Success:	return { 0, 255, 0, 255 };
		case Warning:	return { 255, 255, 0, 255 };
		case Error:		return { 255, 0, 0, 255 };
		case Info:		return { 0, 157, 255, 255 };
	}

	return { 255, 255, 255, 255 };
}

nanoseconds ImGuiToast::get_elapsed_time() const
{
	return system_clock::now() - this->creation_time;
}

NotifyPhase ImGuiToast::get_phase() const
{
	auto elapsed = static_cast<float>(std::chrono::duration_cast<milliseconds>(get_elapsed_time()).count());

	if (elapsed > NOTIFY_FADE_IN_OUT_TIME + static_cast<float>(this->dismiss_time) + NOTIFY_FADE_IN_OUT_TIME)
		return NotifyPhase::Expired;

	if (elapsed > NOTIFY_FADE_IN_OUT_TIME + static_cast<float>(this->dismiss_time))
		return NotifyPhase::FadeOut;

	if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
		return NotifyPhase::Wait;

	if (repeats != 0 && elapsed < NOTIFY_FLASH_TIME)
		return NotifyPhase::Flash;

	return NotifyPhase::FadeIn;
}

float ImGuiToast::get_fade_percent() const
{
	NotifyPhase phase = get_phase();
	auto elapsed = static_cast<float>(std::chrono::duration_cast<milliseconds>(get_elapsed_time()).count());
	switch (phase)
	{
		case NotifyPhase::FadeIn:
			return elapsed / NOTIFY_FADE_IN_OUT_TIME * NOTIFY_OPACITY;

		case NotifyPhase::FadeOut:
			return (1.f - (elapsed - NOTIFY_FADE_IN_OUT_TIME - static_cast<float>(this->dismiss_time)) / NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
	}

	return NOTIFY_OPACITY;
}


namespace ImGui
{
	void insert_notification(const ImGuiToast& toast)
	{
		std::lock_guard lock(g_notification_mutex);

		for (size_t i = 0; i < std::min(15ULL, notifications.size()); i++)
		{
			if (auto& notification = notifications[i]; notification.content == toast.content)
			{
				notification.creation_time = system_clock::now();
				if (notification.repeats < 999)
					notification.repeats++;

				return;
			}
		}

		notifications.push_back(toast);
	}

	void remove_notification(uint64_t index)
	{
		notifications.erase(notifications.begin() + index);
	}

	void render_notifications()
	{
		std::lock_guard lock(g_notification_mutex);

		ImVec2 main_window_size = GetMainViewport()->Size;
		float height = 0.f;
		for (uint64_t i = 0; i < notifications.size(); i++)
		{
			if (GetMainViewport()->Pos.y + NOTIFY_PADDING_Y + height > GetMainViewport()->Size.y)
				return;

			ImGuiToast* current_toast = &notifications[i];
			ImVec4 color = current_toast->get_color();
			float opacity = current_toast->get_fade_percent();
			NotifyPhase phase = current_toast->get_phase();


			switch (phase)
			{
				case NotifyPhase::Expired:
					remove_notification(i--);
					continue;

				case NotifyPhase::Flash:
					PushStyleColor(ImGuiCol_WindowBg, { 0.3f, 0.3f, 0.3f, 1.f });
					break;

				default:
					SetNextWindowBgAlpha(opacity);
					break;
			}

			ImVec2 main_window_pos = GetMainViewport()->Pos;
			SetNextWindowSize(ImVec2(main_window_size.x / 6.f, 0));
			SetNextWindowPos(
				ImVec2(main_window_pos.x + main_window_size.x - NOTIFY_PADDING_X, main_window_pos.y + NOTIFY_PADDING_Y + height),
				ImGuiCond_Always,
				ImVec2(1.0f, 0.0f)
			);

			if (Begin(std::format("##TOAST{}", i).c_str(), nullptr, notify_default_toast_flags))
			{
				PushTextWrapPos(main_window_size.x / 6.f);
				PushFont(g_renderer->m_ubuntu->imfont());

				GImGui->FontSize = 20.f;

				if (!current_toast->content.empty())
				{
					std::string count = std::format("(x{})", current_toast->repeats);
					float count_width = GetWindowWidth() - CalcTextSize(count.c_str()).x - 10.f;

					PushTextWrapPos(count_width - 5.f);
					TextWrapped("%s", current_toast->content.c_str());
					PopTextWrapPos();

					if (current_toast->repeats > 0)
					{
						SameLine(count_width);
						Text("%s", count.c_str());
					}
				}

				PopFont();
				PopTextWrapPos();

				// Save height for next toasts
				height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

				auto timer_bar_pos = ImVec2(GetWindowPos().x, GetWindowPos().y + GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y - 10.f);
				float timer_bar_width = GetWindowWidth();
				switch (phase)
				{
					case NotifyPhase::Wait:
					{
						auto elapsed = static_cast<float>(std::chrono::duration_cast<milliseconds>(current_toast->get_elapsed_time()).count());
						if (float wait_time = current_toast->dismiss_time; elapsed >= NOTIFY_FADE_IN_OUT_TIME && elapsed <= NOTIFY_FADE_IN_OUT_TIME + wait_time)
						{
							timer_bar_width = GetWindowWidth() * (1.0f - (elapsed - NOTIFY_FADE_IN_OUT_TIME) / wait_time);
						}
						break;
					}
					case NotifyPhase::FadeOut:
					{
						auto elapsed = static_cast<float>(std::chrono::duration_cast<milliseconds>(current_toast->get_elapsed_time()).count());
						if (elapsed >= NOTIFY_FADE_IN_OUT_TIME + current_toast->dismiss_time)
						{
							timer_bar_width = 0.0f;
						}
						break;
					}
				}

				GetBackgroundDrawList()->AddRectFilled(
					timer_bar_pos,
					ImVec2(timer_bar_pos.x + timer_bar_width, timer_bar_pos.y + 5.0f),
					ImColor(color.x, color.y, color.z, color.w)
				);
			}

			End();

			if (phase == NotifyPhase::Flash)
				PopStyleColor();
		}
	}
}