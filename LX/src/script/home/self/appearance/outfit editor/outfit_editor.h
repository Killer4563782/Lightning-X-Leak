#pragma once
#include "gui/types/submenu.hpp"

namespace ui::submenus
{
	class outfit_editor_submenu final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
		void on_tick() override;
	};

	class outfit_editor_save_and_load_outfits final : public submenu
	{
		using submenu::submenu;

		void on_enter() override;
	};

	class outfit_editor_accessories final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	// Components

	class outfit_editor_hair final : public submenu
	{
		using submenu::submenu;


		void on_init() override;
	};

	class outfit_editor_head final : public submenu
	{
		using submenu::submenu;


		void on_init() override;
	};

	class outfit_editor_beard_mask final : public submenu
	{
		using submenu::submenu;


		void on_init() override;
	};

	class outfit_editor_torso final : public submenu
	{
		using submenu::submenu;


		void on_init() override;
	};

	class outfit_editor_legs final : public submenu
	{
		using submenu::submenu;


		void on_init() override;
	};

	class outfit_editor_shoes final : public submenu
	{
		using submenu::submenu;


		void on_init() override;
	};

	class outfit_editor_hands_back final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	class outfit_editor_teef_scars_necklaces_bracelets final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	class outfit_editor_body_accessories final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	class outfit_editor_task_armour final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	class outfit_editor_decals final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};

	class outfit_editor_tops_outer final : public submenu
	{
		using submenu::submenu;

		void on_init() override;
	};
}
