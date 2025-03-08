#pragma once
#include "util/globals.h"

struct outfit_t
{
	int id;
	std::string label;
	int drawable_id = 0;
	int drawable_id_max = 0;
	int texture_id = 0;
	int texture_id_max = 0;
};

struct components_t
{
	std::vector<outfit_t> items = {
		{0, "Head"},
		{1, "Beard"},
		{2, "Hair"},
		{3, "Top"},
		{4, "Pants"},
		{5, "Hands"},
		{6, "Feet"},
		{7, "Teef"},
		{8, "Body Accessories"},
		{9, "Parachute"},
		{10, "Decals"},
		{11, "Tops (Outer)"}
	};
};

struct props_t
{
	std::vector<outfit_t> items = {
		{0, "Hats"},
		{1, "Glasses"},
		{2, "Ears"},
		{3, "UNK1"},
		{4, "UNK2"},
		{5, "UNK3"},
		{6, "Watch"},
		{7, "Wrist"},
		{8, "UNK4"}
	};
};

inline void check_bounds_drawable(outfit_t* item, const int lower)
{
	if (item->drawable_id > item->drawable_id_max)
		item->drawable_id = item->drawable_id_max;
	if (item->drawable_id < lower)
		item->drawable_id = lower;
}

inline void check_bounds_texture(outfit_t* item, const int lower)
{
	if (item->texture_id > item->texture_id_max)
		item->texture_id = item->texture_id_max;
	if (item->texture_id < lower)
		item->texture_id = lower;
}

// usually each update increases 1//
inline char* get_slot_name_address(int slot)
{
	return scr_globals::stats.at(0, 5571).at(681).at(2463).at(slot, 8).as<char*>();
}

inline int* get_component_drawable_id_address(int slot, int id)
{
	return scr_globals::stats.at(0, 5571).at(681).at(1339).at(slot, 13).at(id, 1).as<int*>();
}

inline int* get_component_texture_id_address(int slot, int id)
{
	return scr_globals::stats.at(0, 5571).at(681).at(1613).at(slot, 13).at(id, 1).as<int*>();
}

inline int* get_prop_drawable_id_address(int slot, int id)
{
	return scr_globals::stats.at(0, 5571).at(681).at(1887).at(slot, 10).at(id, 1).as<int*>();
}

inline int* get_prop_texture_id_address(int slot, int id)
{
	return scr_globals::stats.at(0, 5571).at(681).at(2098).at(slot, 10).at(id, 1).as<int*>();
}

inline void set_ped_random_component_variation(Ped ped)
{
	auto [items] = components_t();

    auto range = [](int lower_bound, int upper_bound) -> int
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(lower_bound, upper_bound);
        return dis(gen);
    };

    for (const auto& item : items)
    {
        int drawable_id_max = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(ped, item.id) - 1;
        if (drawable_id_max < 0)
            continue;

        int drawable_id = range(0, drawable_id_max);

        int texture_id_max = PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(ped, item.id, drawable_id) - 1;
        if (texture_id_max < 0)
            continue;

        int texture_id = range(0, texture_id_max);

        PED::SET_PED_COMPONENT_VARIATION(ped, item.id, drawable_id, texture_id, PED::GET_PED_PALETTE_VARIATION(ped, item.id));
    }
}