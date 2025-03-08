#pragma once
#include "vehicleHelpers.h"
#include "rage/util.h"
#include "util/util.h"
#include <script/home/self/appearance/outfit editor/Outfits.hpp>

class ini
{
	std::string ini_file;
public:
	explicit ini(const std::string& file)
	{
		this->ini_file = file;
	}

	void write_string(const std::string& string, const std::string& app, const std::string& key) const
	{
		WritePrivateProfileStringA(app.c_str(), key.c_str(), string.c_str(), this->ini_file.c_str());
	}

	std::string get_string(const std::string& app, const std::string& key) const
	{
		try
		{
			char buf[100];
			GetPrivateProfileStringA(app.c_str(), key.c_str(), "NULL", buf, 100, this->ini_file.c_str());
			return buf;
		}
		catch (...)
		{
			
		}

		return {};
	}

	void write_int(int value, const std::string& app, const std::string& key) const
	{
		write_string(std::to_string(value), app, key);
	}

	void write_hash(Hash value, const std::string& app, const std::string& key) const
	{
		write_string(std::to_string(value), app, key);
	}

	int get_int(const std::string& app, const std::string& key) const
	{
		return std::stoi(get_string(app, key));
	}

	void write_float(float value, const std::string& app, const std::string& key) const
	{
		write_string(std::to_string(value), app, key);
	}

	float get_float(const std::string& app, const std::string& key) const
	{
		return std::stof(get_string(app, key));
	}

	void write_bool(bool value, const std::string& app, const std::string& key) const
	{
		write_string(std::to_string(value), app, key);
	}

	bool get_bool(const std::string& app, const std::string& key) const
	{
		const std::string fetched = get_string(app, key);
		if (fetched == "1")
		{
			return true;
		}
		if (fetched == "0")
		{
			return false;
		}
		return false;
	}
};

class vehicle_ini
{
public:
	bool does_ini_exists(const char* path)
	{
		struct stat buffer;
		return stat(path, &buffer) == 0;
	}

	void save(const std::string& name)
	{
		g_fiber_pool->queue_job([&]
		{
			// Get the player's current vehicle
			if (!self::vehicle)
			{
				notify(Error, "You must be in a vehicle to save it.");
				return;
			}

			// Define the custom cars directory and file path
			const auto custom_cars_path = g_lx_path / "CustomCars";
			const auto file_path = custom_cars_path / (name + ".ini");

			// Ensure the directory exists
			if (!exists(custom_cars_path))
			{
				create_directories(custom_cars_path);
			}

			// Create the INI file
			const ini color_ini(file_path.string());

			// Retrieve vehicle color data
			int r, g, b;
			VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(self::vehicle, &r, &g, &b);
			int r2, g2, b2;
			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(self::vehicle, &r2, &g2, &b2);
			int primary_color, secondary_color;
			VEHICLE::GET_VEHICLE_COLOURS(self::vehicle, &primary_color, &secondary_color);
			int pearl, wheel;
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(self::vehicle, &pearl, &wheel);

			// Write vehicle data to the INI file
			color_ini.write_string(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(ENTITY::GET_ENTITY_MODEL(self::vehicle)), "Info", "Name");
			color_ini.write_int(r, "Color", "R");
			color_ini.write_int(g, "Color", "G");
			color_ini.write_int(b, "Color", "B");
			color_ini.write_int(r2, "Color", "R2");
			color_ini.write_int(g2, "Color", "G2");
			color_ini.write_int(b2, "Color", "B2");
			color_ini.write_int(VEHICLE::GET_VEHICLE_WHEEL_TYPE(self::vehicle), "Wheel", "Type");
			color_ini.write_int(VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(self::vehicle), "Plate", "Index");
			color_ini.write_string(VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(self::vehicle), "Plate", "Text");
			color_ini.write_int(primary_color, "Color1", "Index");
			color_ini.write_int(secondary_color, "Color2", "Index");
			color_ini.write_int(wheel, "Wheel", "Index");
			color_ini.write_int(pearl, "Pearl", "Index");
			color_ini.write_string(MENU_VERSION, "Other", "Version");

			// Save vehicle upgrades
			for (int i = 0; i < 50; i++)
			{
				char mod_index[64];
				sprintf(mod_index, "index_%i", i);
				color_ini.write_int(VEHICLE::GET_VEHICLE_MOD(self::vehicle, i), "upgrades", mod_index);
			}

			// Notify the user of the save operation
			notify(Info, "Vehicle saved successfully to: " + file_path.string());
		});
	}


	void load(const std::string& name)
	{
		g_fiber_pool->queue_job([&]
		{
			const auto custom_cars_path = g_lx_path / "CustomCars";
			const auto file_path = custom_cars_path / (name + ".ini");
			const auto color_ini = new ini(file_path.string());

			// Load vehicle data
			std::string version = color_ini->get_string("Other", "Version");
			Hash hash = rage::joaat(color_ini->get_string("Info", "Name"));

			// Create vehicle
			Vehicle new_spawned_vehicle{};
			new_spawned_vehicle = vehicle::spawn_vehicle(hash, self::position, ENTITY::GET_ENTITY_HEADING(self::ped), true, false);

			PED::SET_PED_INTO_VEHICLE(self::ped, new_spawned_vehicle, -1);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(new_spawned_vehicle, color_ini->get_int("Color", "R"), color_ini->get_int("Color", "G"), color_ini->get_int("Color", "B"));
			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(new_spawned_vehicle, color_ini->get_int("Color", "R2"), color_ini->get_int("Color", "G2"), color_ini->get_int("Color", "B2"));
			VEHICLE::SET_VEHICLE_MOD_KIT(new_spawned_vehicle, 0);
			VEHICLE::SET_VEHICLE_WHEEL_TYPE(new_spawned_vehicle, color_ini->get_int("Wheel", "Type"));
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(new_spawned_vehicle, color_ini->get_int("Plate", "Index"));
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(new_spawned_vehicle, color_ini->get_string("Plate", "Text").c_str());

			for (int i = 0; i < 50; i++)
			{
				char input2[64];
				sprintf(input2, "index_%i", i);
				VEHICLE::SET_VEHICLE_MOD(new_spawned_vehicle, i, color_ini->get_int("upgrades", input2), false);
			}
		});
	}
};

inline vehicle_ini g_vehicle_ini;

class outfit_ini {
public:
	bool does_ini_exists(const char* path)
	{
		struct stat buffer;
		return stat(path, &buffer) == 0;
	}

	void save(const std::string& name)
	{
		g_fiber_pool->queue_job([&]
		{
			std::vector<std::tuple<int, std::string, int, int, int, int>> components = {
				{0, "Head", 0, 0, 0, 0},
				{1, "Beard", 0, 0, 0, 0},
			    {2, "Hair", 0, 0, 0, 0},
				{3, "Torso", 0, 0, 0, 0},
				{4, "Legs", 0, 0, 0, 0},
				{5, "Hands", 0, 0, 0, 0},
				{6, "Feet", 0, 0, 0, 0},
				{7, "Chains", 0, 0, 0, 0},
				{8, "Accessories", 0, 0, 0, 0},
				{9, "Vests", 0, 0, 0, 0},
				{10, "Decals", 0, 0, 0, 0},
				{11, "Tops", 0, 0, 0, 0}
			};

			std::vector<std::tuple<int, std::string, int, int, int, int>> props = {
				{0, "Hats", 0, 0, 0, 0},
				{1, "Glasses", 0, 0, 0, 0},
				{2, "Ears", 0, 0, 0, 0},
				{6, "Watches", 0, 0, 0, 0},
				{7, "Bracelets", 0, 0, 0, 0}
			};

		
			for (auto& [id, label, drawable_id, drawable_id_max, texture_id, texture_id_max] : components)
			{
				drawable_id = PED::GET_PED_DRAWABLE_VARIATION(self::ped, id);
				drawable_id_max = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(self::ped, id) - 1;

				texture_id = PED::GET_PED_TEXTURE_VARIATION(self::ped, id);
				texture_id_max = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(self::ped, id, drawable_id) - 1;
			}

			
			for (auto& [id, label, drawable_id, drawable_id_max, texture_id, texture_id_max] : props)
			{
				drawable_id = PED::GET_PED_PROP_INDEX(self::ped, id, NETWORK::NETWORK_IS_GAME_IN_PROGRESS());
				drawable_id_max = PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(self::ped, id) - 1;

				texture_id = PED::GET_PED_PROP_TEXTURE_INDEX(self::ped, id);
				texture_id_max = PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(self::ped, id, drawable_id) - 1;
			}

			const auto custom_outfit_path = g_lx_path / "CustomOutfit";
			const auto file_path = custom_outfit_path / (name + ".ini");

		
			if (!exists(custom_outfit_path))
			{
				create_directories(custom_outfit_path);
			}

		
			const ini save_ini(file_path.string());

			
			for (const auto& [id, label, drawable_id, drawable_id_max, texture_id, texture_id_max] : components)
			{
				save_ini.write_int(drawable_id, label, "index");
				save_ini.write_int(texture_id, label, "texture");
			}

			for (const auto& [id, label, drawable_id, drawable_id_max, texture_id, texture_id_max] : props)
			{
				save_ini.write_int(drawable_id, label, "index");
				save_ini.write_int(texture_id, label, "texture");
			}

			notify(Info, "Outfit saved successfully to: " + file_path.string());
		});
	}


	void load(const std::string& name)
	{
		g_fiber_pool->queue_job([&]
		{
	
			const auto custom_outfit_path = g_lx_path / "CustomOutfit";
			const auto file_path = custom_outfit_path / (name + ".ini");


			if (!exists(file_path))
			{
				notify(Error, "Outfit file not found: " + file_path.string());
				return;
			}


			const auto load_ini = new ini(file_path.string());

			std::string version = load_ini->get_string("Other", "Version");

			PED::SET_PED_COMPONENT_VARIATION(self::ped, 0, load_ini->get_int("Head", "index"), load_ini->get_int("Head", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 1, load_ini->get_int("Beard", "index"), load_ini->get_int("Beard", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 2, load_ini->get_int("Hair", "index"), load_ini->get_int("Hair", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 3, load_ini->get_int("Torso", "index"), load_ini->get_int("Torso", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 11, load_ini->get_int("Tops", "index"), load_ini->get_int("Tops", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 4, load_ini->get_int("Legs", "index"), load_ini->get_int("Legs", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 5, load_ini->get_int("Hands", "index"), load_ini->get_int("Hands", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 6, load_ini->get_int("Feet", "index"), load_ini->get_int("Feet", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 7, load_ini->get_int("Chains", "index"), load_ini->get_int("Chains", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 8, load_ini->get_int("Accessories", "index"), load_ini->get_int("Accessories", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 9, load_ini->get_int("Vests", "index"), load_ini->get_int("Vests", "texture"), 0);
			PED::SET_PED_COMPONENT_VARIATION(self::ped, 10, load_ini->get_int("Decals", "index"), load_ini->get_int("Decals", "texture"), 0);

		
			PED::SET_PED_PROP_INDEX(self::ped, 0, load_ini->get_int("Hats", "index"), load_ini->get_int("Hats", "texture"), true, NETWORK::NETWORK_IS_GAME_IN_PROGRESS());
			PED::SET_PED_PROP_INDEX(self::ped, 1, load_ini->get_int("Glasses", "index"), load_ini->get_int("Glasses", "texture"), true, NETWORK::NETWORK_IS_GAME_IN_PROGRESS());
			PED::SET_PED_PROP_INDEX(self::ped, 2, load_ini->get_int("Ears", "index"), load_ini->get_int("Ears", "texture"), true, NETWORK::NETWORK_IS_GAME_IN_PROGRESS());
			PED::SET_PED_PROP_INDEX(self::ped, 6, load_ini->get_int("Watches", "index"), load_ini->get_int("Watches", "texture"), true, NETWORK::NETWORK_IS_GAME_IN_PROGRESS());
			PED::SET_PED_PROP_INDEX(self::ped, 7, load_ini->get_int("Bracelets", "index"), load_ini->get_int("Bracelets", "texture"), true, NETWORK::NETWORK_IS_GAME_IN_PROGRESS());

		
			notify(Info, "Outfit successfully loaded: " + name);
		});
	}
};

inline outfit_ini g_outfit_ini;
