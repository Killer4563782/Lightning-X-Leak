#include "custom_cars.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "helpers/inihelper.h"
#include <gui/types/icon_option.hpp>
#include <fonts/icons.h>
#include <helpers/inputs.h>
#include <helpers/addonhelper.h>

namespace ui::submenus
{
	void custom_cars_submenu::on_enter()
	{
		m_options.clear();
		if (self::vehicle)
		{
			this->add(icon_option("Enter a save name for your car", "Input a vehicle name", ICON_FA_KEYBOARD, []
			{
				get_input("Enter car save name", "Enter a name for the car save", text_input::InputFlags_None, [](const std::string& input)
				{
					if (input.empty())
					{
						notify(Warning, "Save name cannot be empty");
						return;
					}
					try
					{
						g_vehicle_ini.save(input); // Save the current vehicle configuration
						notify(Success, "Vehicle saved successfully as: " + input);
					}
					catch (const std::exception& e)
					{
						notify(Error, std::string("Failed to save vehicle: ") + e.what());
					}
				});
			}));
		}

		if (const auto custom_cars = fs::path(g_lx_path / "CustomCars"); exists(custom_cars) && is_directory(custom_cars))
		{
			for (const auto& dir_entry : fs::recursive_directory_iterator(custom_cars))
			{
				if (dir_entry.is_regular_file())
				{
					if (const auto& path = dir_entry.path(); path.extension() == ".ini" or path.extension() == ".rpf")
					{
						auto stem = path.stem().string();

						this->add(function_option(stem, {}, [stem]
						{
							try
							{
								notify(Success, "Vehicle loaded: " + stem);
							}
							catch (const std::exception& e)
							{
								notify(Error, std::string("Failed to load vehicle: ") + e.what());
							}
						}));
					}
				}
			}
		}
		else
		{
			notify(Warning, "No custom car files found in the directory");
		}
	}

	bool populateDlcContent()
	{
		const auto models = addoncar::getDlcModels();
		if (models.empty())
		{
			return false;
		}
		for (const auto& model : models)
		{

		}
		return true;
	}
	custom_cars_submenu g_custom_cars_submenu("Custom Vehicle", {});
}
