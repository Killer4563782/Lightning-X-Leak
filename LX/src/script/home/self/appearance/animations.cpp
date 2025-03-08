#include "animations.h"

#include "commands/manager.hpp"
#include "fiber/script.hpp"
#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/icon_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "helpers/inputs.h"
#include "imgui/imgui_internal.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

static bool load_animation_dict(const char* dict)
{
	if (!STREAMING::DOES_ANIM_DICT_EXIST(dict))
	{
		notify(Error, 5s, "{} could not be found. Make sure its a valid animation dictionary", dict);
		return false;
	}

	if (STREAMING::HAS_ANIM_DICT_LOADED(dict))
		return true;

	for (uint8_t i = 0; !STREAMING::HAS_ANIM_DICT_LOADED(dict) && i < 70; i++)
	{
		STREAMING::REQUEST_ANIM_DICT(dict);
		script::get_current()->yield();
	}

	return STREAMING::HAS_ANIM_DICT_LOADED(dict);
}

namespace ui::submenus
{
	animations_options_submenu::animations_options_submenu(std::string name, std::string description) : submenu(std::move(name), std::move(description))
	{}

	int animations_options_submenu::get_flags() const
	{
		int animation_flags = DEFAULT;

		if (m_loop)
			animation_flags |= LOOPING;

		if (m_not_interruptible)
			animation_flags |= NOT_INTERRUPTABLE;

		if (m_allow_movement)
			animation_flags |= UPPERBODY | SECONDARY;

		if (m_abort_on_movement)
			animation_flags |= ABORT_ON_PED_MOVEMENT;

		if (m_ignore_gravity)
			animation_flags |= IGNORE_GRAVITY;

		if (m_hide_weapon)
			animation_flags |= HIDE_WEAPON;

		if (m_force)
			animation_flags |= FORCE_START;

		return animation_flags;
	}

	void animations_options_submenu::on_init()
	{
		this->add(toggle_option("Loop", "Repeat the animation", &m_loop));
		this->add(toggle_option("Not Interruptible", {}, &m_not_interruptible));
		this->add(toggle_option("Allow movement", "Allow you to move while playing the animation", &m_allow_movement));
		this->add(toggle_option("Abort on Movement", "Stop the animation on movement", &m_abort_on_movement));
		this->add(toggle_option("Ignore Gravity", "Gravity wont be applied while the animation is playing", &m_ignore_gravity));
		this->add(toggle_option("Hide Weapon", {}, &m_hide_weapon));
		this->add(toggle_option("Force", {}, &m_force));
	}

	animations_options_submenu g_animations_options_submenu("Options", "These options dont count for Scenarios");


	dictionary_submenu::dictionary_submenu(std::string name, const animation animations[], size_t size) : submenu(std::move(name), std::format("{} Animations", size)),
		m_animations(animations),
		m_size(size)
	{}

	void dictionary_submenu::on_enter()
	{
		m_current = 0;
		m_scroll_position = 0;
	}

	void dictionary_submenu::on_init()
	{
		this->add(icon_option("Stop Animation", {}, ICON_FA_TRASH_CAN, []
		{
			g_fiber_pool->queue_job([]
			{
				TASK::CLEAR_PED_TASKS(self::ped);
			});
		}));


		for (size_t i = 0; i < m_size; ++i)
		{
			const auto& animation = m_animations[i];
			auto dict = animation.m_animation.first.data();
			auto anim = animation.m_animation.second.data();

			this->add(function_option(animation.label.data(), {}, [dict, anim]
			{
				g_fiber_pool->queue_job([dict, anim]
				{
					if (load_animation_dict(dict))
						TASK::TASK_PLAY_ANIM(self::ped, dict, anim, 4.f, -4.f, -1, g_animations_options_submenu.get_flags(), 0, false, false, false);
				});
			}));
		}
	}

	scenarios_submenu::scenarios_submenu(std::string name, const scenario scenarios[], size_t size) : submenu(std::move(name), std::format("{} Scenarios", size)),
		m_scenarios(scenarios),
		m_size(size)
	{}

	void scenarios_submenu::on_enter()
	{
		m_current = 0;
		m_scroll_position = 0;
	}

	void scenarios_submenu::on_init()
	{
		this->add(icon_option("Stop Animation", {}, ICON_FA_TRASH_CAN, []
		{
			g_fiber_pool->queue_job([]
			{
				TASK::CLEAR_PED_TASKS(self::ped);
			});
		}));

		for (size_t i = 0; i < m_size; ++i)
		{
			auto& scenario = m_scenarios[i];
			this->add(function_option(scenario.m_label.data(), {}, [scenario]
			{
				g_fiber_pool->queue_job([scenario]
				{
					TASK::TASK_START_SCENARIO_IN_PLACE(self::ped, scenario.m_key.data(), 0, true);
				});
			}));
		}
	}

	void animations_submenu::on_init()
	{
		this->add(icon_option("Instantly Stop Animation", {}, ICON_FA_TRASH_CAN, []
		{
			g_fiber_pool->queue_job([]
			{
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);
			});
		}));
		this->add(icon_option("Stop Animation", {}, ICON_FA_TRASH_CAN, []
		{
			g_fiber_pool->queue_job([]
			{
				TASK::CLEAR_PED_TASKS(self::ped);
			});
		}));

		this->add(&g_animations_options_submenu);
		this->add(function_option("Play Custom Animation", {}, [this]
		{
			m_custom_dictionary = "";
			m_custom_animation_name = "";
			"disable_input"_LC->enable();
			ImGui::GetIO().MouseDrawCursor = true;
			g_open = false;
			text_input::input_flags = text_input::InputFlags_None;

			g_renderer->m_render_queue["custom_animation_input"] = [this]
			{
				ImGui::SetNextWindowPos(drawing::cords_to_screen({ 0.5f - 0.35f / 2.f, 0.5f - 0.3f }));
				if (ImGui::Begin("Play Custom Animation", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::SetWindowFontScale(0.5f);
					constexpr ImGuiInputFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

					if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
						ImGui::SetKeyboardFocusHere(0);

					ImGui::SetNextItemWidth(drawing::cords_to_screen({ 0.35f, 0.1f }).x);
					const bool enter = ImGui::InputTextWithHint("##DictionaryInput", "Dictionary", &m_custom_dictionary, flags);
					ImGui::SetNextItemWidth(drawing::cords_to_screen({ 0.35f, 0.1f }).x);
					const bool enter2 = ImGui::InputTextWithHint("##AnimationNameInput", "Animation Name", &m_custom_animation_name, flags);


					const bool confirm = ImGui::Button("Confirm");
					ImGui::SameLine();
					const bool cancel = ImGui::Button("Cancel") || ImGui::IsKeyReleased(ImGuiKey_Escape);

					if (enter || enter2 || confirm || cancel)
					{
						if (enter || enter2 || confirm)
						{
							g_fiber_pool->queue_job([this]
							{
								if (load_animation_dict(m_custom_dictionary.c_str()))
								{
									TASK::TASK_PLAY_ANIM(self::ped, m_custom_dictionary.c_str(), m_custom_animation_name.c_str(), 4.f, -4.f, -1, g_animations_options_submenu.get_flags(), 0, false, false, false);
								}
							});
						}

						g_renderer->m_render_queue.erase("custom_animation_input");
						"disable_input"_LC->disable();
						ImGui::GetIO().MouseDrawCursor = false;
						g_open = true;
						return ImGui::End();
					}
				}
				ImGui::End();
			};
		}));
		this->add(break_option("Presets"));
		this->add(scenarios_submenu("Scenarios", g_scenarios, std::size(g_scenarios)));
		this->add(scenarios_submenu("Animal Scenarios", g_animal_scenarios, std::size(g_animal_scenarios)));
		this->add(dictionary_submenu("Dances", g_dance_animations, std::size(g_dance_animations)));
		this->add(dictionary_submenu("Sport", g_sport_animations, std::size(g_sport_animations)));
		this->add(dictionary_submenu("Super Hero", g_super_hero_animations, std::size(g_super_hero_animations)));
		this->add(dictionary_submenu("Sus", g_sex_animations, std::size(g_sex_animations)));
		this->add(dictionary_submenu("Misc", g_miscellaneous_animations, std::size(g_miscellaneous_animations)));
	}
}
