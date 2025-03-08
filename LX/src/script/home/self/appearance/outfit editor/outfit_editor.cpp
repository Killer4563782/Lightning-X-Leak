#include "outfit_editor.h"
#include "Outfits.hpp"
#include "commands/manager.hpp"
#include "commands/settings.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"
#include <gui/types/icon_option.hpp>
#include <fonts/icons.h>
#include "helpers/inihelper.h"

namespace ui::submenus
{
	static components_t components;
	static props_t props;

	void outfit_editor_submenu::on_init()
	{
		this->add(outfit_editor_save_and_load_outfits("Save & Load Outfits", "Save & Load Outfits"));
		this->add(break_option("Appearance And Accessories"));
		this->add(outfit_editor_accessories("Accessories", "Accessories"));
		this->add(break_option("Components"));
		this->add(outfit_editor_head("Head"));
		this->add(outfit_editor_beard_mask("Beard/Mask"));
		this->add(outfit_editor_hair("Hair"));
		this->add(outfit_editor_torso("Torso"));
		this->add(outfit_editor_legs("Legs"));
		this->add(outfit_editor_shoes("Shoes"));
		this->add(outfit_editor_hands_back("Hands/Back"));
		this->add(outfit_editor_teef_scars_necklaces_bracelets("Teef/Scars/Necklaces/Bracelets"));
		this->add(outfit_editor_body_accessories("Body Accessories"));
		this->add(outfit_editor_task_armour("Task/Armour"));
		this->add(outfit_editor_decals("Decals"));
		this->add(outfit_editor_tops_outer("Tops (Outer)"));

		this->add(function_option("Default Outfit", {}, [&]
		{
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(self::ped);
		}));
		this->add(function_option("Random Outfit", {}, [&]
		{
			set_ped_random_component_variation(self::ped);
		}));
		this->add(toggle_option("outfit_editor_camera"_LC));
		this->add(number_option("Type", {}, &commands::settings::outfit_editor::current_body_pos, 3, 0, 1));
	}

	void outfit_editor_submenu::on_tick()
	{
		for (auto& [id, label, drawable_id, drawable_id_max, texture_id, texture_id_max] : components.items)
		{
			drawable_id = PED::GET_PED_DRAWABLE_VARIATION(self::ped, id);
			drawable_id_max = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(self::ped, id) - 1;

			texture_id = PED::GET_PED_TEXTURE_VARIATION(self::ped, id);
			texture_id_max = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(self::ped, id, drawable_id) - 1;
		}

		for (auto& [id, label, drawable_id, drawable_id_max, texture_id, texture_id_max] : props.items)
		{
			drawable_id = PED::GET_PED_PROP_INDEX(self::ped, id, 0);
			drawable_id_max = PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(self::ped, id) - 1;

			texture_id = PED::GET_PED_PROP_TEXTURE_INDEX(self::ped, id);
			texture_id_max = PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(self::ped, id, drawable_id) - 1;
		}
	}

	void outfit_editor_save_and_load_outfits::on_enter()
	{
		m_options.clear();
		this->add(icon_option("Enter a save name for your outfit", "Input a outfit name", ICON_FA_KEYBOARD, []
		{
			get_input("Enter a save name for your outfit", "Enter a save name for your outfit", text_input::InputFlags_None,
				[] (const std::string& input)
			{
				if (input.empty())
				{
					notify(Warning, "Save name cannot be empty");
					return;
				}
				try
				{
					g_outfit_ini.save(input); // Save the current outfit configuration
					notify(Success, "Outfit saved successfully as: " + input);
				}
				catch (const std::exception& e)
				{
					notify(Error, std::string("Failed to save Outfit: ") + e.what());
				}
			});
		}));
		if (const auto custom_outfit = fs::path(g_lx_path / "CustomOutfit"); exists(custom_outfit) && is_directory(custom_outfit))
		{
			for (const auto& dir_entry : fs::directory_iterator(custom_outfit))
			{
				if (dir_entry.is_regular_file())
				{
					if (const auto& path = dir_entry.path(); path.extension() == ".ini")
					{
						auto stem = path.stem().string();
						this->add(function_option(stem, {}, [stem]
						{
							try
							{
								g_outfit_ini.load(stem);
								notify(Success, "Outfit loaded: " + stem);
							}
							catch (const std::exception& e)
							{
								notify(Error, std::string("Failed to load Outfit: ") + e.what());
							}
						}));
					}
				}
			}
		}
		else
		{
			notify(Warning, "No custom outfit files found in the directory");
		}
	}

	void outfit_editor_accessories::on_init()
	{
		this->add(submenu("Hats"));
		this->add(submenu("Glasses"));
		this->add(submenu("Ears"));
		this->add(submenu("Watches"));
		this->add(submenu("Wrist"));
		this->add(submenu("UNK1"));
		this->add(submenu("UNK2"));
		this->add(submenu("UNK3"));
	}

	void outfit_editor_head::on_init()
	{
		this->add(number_option("Variation", "Head Variation", &components.items[0].drawable_id, components.items[0].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[0], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[0].id, components.items[0].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[0].id));
			});
		}));

		this->add(number_option("Texture", "Head Texture", &components.items[0].texture_id, components.items[0].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[0], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[0].id, components.items[0].drawable_id, components.items[0].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[0].id));
			});
		}));
	}

	void outfit_editor_beard_mask::on_init()
	{
		this->add(number_option("Variation", "Beard/Mask Variation", &components.items[1].drawable_id, components.items[1].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[1], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[1].id, components.items[1].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[1].id));
			});
		}));

		this->add(number_option("Texture", "Beard/Mask Texture", &components.items[1].texture_id, components.items[1].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[1], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[1].id, components.items[1].drawable_id, components.items[1].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[1].id));
			});
		}));
	}

	void outfit_editor_hair::on_init()
	{
		this->add(number_option("Variation", "Hair Variation", &components.items[2].drawable_id, components.items[2].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[2], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[2].id, components.items[2].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[2].id));
			});
		}));

		this->add(number_option("Texture", "Hair Texture", &components.items[2].texture_id, components.items[2].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[2], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[2].id, components.items[2].drawable_id, components.items[2].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[2].id));
			});
		}));
	}

	void outfit_editor_torso::on_init()
	{
		this->add(number_option("Variation", "Torso Variation", &components.items[3].drawable_id, components.items[3].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[3], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[3].id, components.items[3].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[3].id));
			});
		}));

		this->add(number_option("Texture", "Torso Texture", &components.items[3].texture_id, components.items[3].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[3], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[3].id, components.items[3].drawable_id, components.items[3].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[3].id));
			});
		}));
	}

	void outfit_editor_legs::on_init()
	{
		this->add(number_option("Variation", "Legs Variation", &components.items[4].drawable_id, components.items[4].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[4], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[4].id, components.items[4].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[4].id));
			});
		}));

		this->add(number_option("Texture", "Legs Texture", &components.items[4].texture_id, components.items[4].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[4], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[4].id, components.items[4].drawable_id, components.items[4].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[4].id));
			});
		}));
	}

	void outfit_editor_shoes::on_init()
	{
		this->add(number_option("Variation", "Shoes Variation", &components.items[6].drawable_id, components.items[6].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[6], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[6].id, components.items[6].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[6].id));
			});
		}));

		this->add(number_option("Texture", "Shoes Texture", &components.items[6].texture_id, components.items[6].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[6], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[6].id, components.items[6].drawable_id, components.items[6].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[6].id));
			});
		}));
	}

	void outfit_editor_hands_back::on_init()
	{
		this->add(number_option("Variation", "Hands/Back Variation", &components.items[5].drawable_id, components.items[5].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[5], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[5].id, components.items[5].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[5].id));
			});
		}));

		this->add(number_option("Texture", "Hands/Back Texture", &components.items[5].texture_id, components.items[5].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[5], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[5].id, components.items[5].drawable_id, components.items[5].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[5].id));
			});
		}));
	}

	void outfit_editor_teef_scars_necklaces_bracelets::on_init()
	{
		this->add(number_option("Variation", "Teef/Scars/Necklaces/Bracelets Variation", &components.items[7].drawable_id, components.items[7].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[7], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[7].id, components.items[7].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[7].id));
			});
		}));

		this->add(number_option("Texture", "Teef/Scars/Necklaces/Bracelets Texture", &components.items[7].texture_id, components.items[7].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[7], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[7].id, components.items[7].drawable_id, components.items[7].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[7].id));
			});
		}));
	}

	void outfit_editor_body_accessories::on_init()
	{
		this->add(number_option("Variation", "Body Accessories", &components.items[8].drawable_id, components.items[8].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[8], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[8].id, components.items[8].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[8].id));
			});
		}));

		this->add(number_option("Texture", "Teef/Scars/Necklaces/Bracelets Texture", &components.items[8].texture_id, components.items[8].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[8], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[8].id, components.items[8].drawable_id, components.items[8].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[8].id));
			});
		}));
	}

	void outfit_editor_task_armour::on_init()
	{
		this->add(number_option("Variation", "Task/Armour Variation", &components.items[9].drawable_id, components.items[9].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[9], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[9].id, components.items[9].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[9].id));
			});
		}));

		this->add(number_option("Texture", "Task/Armour Texture", &components.items[9].texture_id, components.items[9].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[9], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[9].id, components.items[9].drawable_id, components.items[9].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[9].id));
			});
		}));
	}

	void outfit_editor_decals::on_init()
	{
		this->add(number_option("Variation", "Decals Variation", &components.items[10].drawable_id, components.items[10].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[10], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[10].id, components.items[10].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[10].id));
			});
		}));

		this->add(number_option("Texture", "Decals Texture", &components.items[10].texture_id, components.items[10].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[10], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[10].id, components.items[10].drawable_id, components.items[10].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[10].id));
			});
		}));
	}

	void outfit_editor_tops_outer::on_init()
	{
		this->add(number_option("Variation", "Tops (Outer) Variation", &components.items[11].drawable_id, components.items[11].drawable_id_max, 0, 1, []
		{
			check_bounds_drawable(&components.items[11], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[11].id, components.items[11].drawable_id, 0, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[11].id));
			});
		}));

		this->add(number_option("Texture", "Tops (Outer) Texture", &components.items[11].texture_id, components.items[11].texture_id_max, 0, 1, []
		{
			check_bounds_texture(&components.items[11], -1); // The game does this on it's own but seems to crash if we call OOB values to fast.
			g_fiber_pool->queue_job([&] {
				PED::SET_PED_COMPONENT_VARIATION(self::ped, components.items[11].id, components.items[11].drawable_id, components.items[11].texture_id, PED::GET_PED_PALETTE_VARIATION(self::ped, components.items[11].id));
			});
		}));
	}
}
