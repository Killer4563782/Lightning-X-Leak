#pragma once
#include "imgui/imgui.h"
#include "renderer/renderer.h"

using namespace std::chrono;
inline std::mutex g_notification_mutex; // Sometimes exception occurs

#define NOTIFY_PADDING_X              20.f
#define NOTIFY_PADDING_Y              20.f
#define NOTIFY_PADDING_MESSAGE_Y      10.f
#define NOTIFY_FADE_IN_OUT_TIME       200.f
#define NOTIFY_FLASH_TIME             NOTIFY_FADE_IN_OUT_TIME + 50.f
#define NOTIFY_DEFAULT_DISMISS        4000.f
#define NOTIFY_OPACITY                0.8f

constexpr ImGuiWindowFlags notify_default_toast_flags = ImGuiWindowFlags_AlwaysAutoResize |
ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus |
ImGuiWindowFlags_NoFocusOnAppearing;

enum ImGuiToastType
{
	Success,
	Warning,
	Error,
	Info,
};

enum class NotifyPhase : uint8_t
{
	FadeIn,
	Wait,
	Flash,
	FadeOut,
	Expired
};

class ImGuiToast
{
public:
	ImGuiToastType type;
	std::string content;
	float dismiss_time;
	system_clock::time_point creation_time = system_clock::now();
	uint16_t repeats = 0;

	template <typename... T>
	explicit ImGuiToast(ImGuiToastType type, float dismiss_time, std::string format, T... args)
	{
		this->type = type;
		this->dismiss_time = dismiss_time;
		this->content = std::vformat(format, std::make_format_args(args...));
	}

	ImVec4 get_color() const;
	nanoseconds get_elapsed_time() const;
	NotifyPhase get_phase() const;
	float get_fade_percent() const;
};

namespace ImGui
{
	inline std::vector<ImGuiToast> notifications;

	void insert_notification(const ImGuiToast& toast);
	void remove_notification(uint64_t index);
	void render_notifications();
}

template <typename... T>
void notify(ImGuiToastType type, float duration, const std::string& format, T... args)
{
	std::string content = std::vformat(format, std::make_format_args(args...));
	ImGui::insert_notification(ImGuiToast(type, duration, content));
}

template <typename... T>
void notify(ImGuiToastType type, high_resolution_clock::duration duration, const std::string& format, T... args)
{
	auto time = std::chrono::duration_cast<milliseconds>(duration);
	std::string content = std::vformat(format, std::make_format_args(args...));
	ImGui::insert_notification(ImGuiToast(type, static_cast<float>(time.count()), content));
}

template <typename... T>
void notify(ImGuiToastType type, const std::string& format, T... args)
{
	std::string content = std::vformat(format, std::make_format_args(args...));
	ImGui::insert_notification(ImGuiToast(type, NOTIFY_DEFAULT_DISMISS, content));
}