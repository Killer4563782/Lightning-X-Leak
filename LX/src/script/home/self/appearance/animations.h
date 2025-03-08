#pragma once
#include "gui/types/submenu.hpp"
using namespace tables;

namespace ui::submenus
{
	class animations_options_submenu final : public submenu
	{
	public:
		bool m_loop = false;
		bool m_not_interruptible = false;
		bool m_allow_movement = false;
		bool m_abort_on_movement = false;
		bool m_ignore_gravity = false;
		bool m_hide_weapon = false;
		bool m_force = false;

		explicit animations_options_submenu(std::string name, std::string description);

		[[nodiscard]] int get_flags() const;
		void on_init() override;
	};

	extern animations_options_submenu g_animations_options_submenu;


	class dictionary_submenu final : public submenu
	{
	public:
		const animation* m_animations;
		size_t m_size;

		explicit dictionary_submenu(std::string name, const animation animations[], size_t size);

		void on_enter() override;
		void on_init() override;
	};

	class scenarios_submenu final : public submenu
	{
	public:
		const scenario* m_scenarios;
		size_t m_size;

		explicit scenarios_submenu(std::string name, const scenario scenarios[], size_t size);

		void on_enter() override;
		void on_init() override;
	};

	class animations_submenu final : public submenu
	{
		std::string m_custom_dictionary;
		std::string m_custom_animation_name;

		using submenu::submenu;

		void on_init() override;
	};
}
