#include "protections.h"

#include "model_info.hpp"

namespace protections
{
	bool is_crash_object(rage::joaat_t model)
	{
		if (!model_info::get_model(model))
			return false;

		if (!model_info::is_model_of_type(model, eModelType::Object, eModelType::Time, eModelType::Weapon, eModelType::Destructable, eModelType::WorldObject, eModelType::Sprinkler, eModelType::Unk65, eModelType::Plant, eModelType::LOD, eModelType::Unk132, eModelType::Building))
			return true;

		return std::ranges::any_of(crash_objects, [model](uint32_t crash_object) { return crash_object == model; });
	}

	bool is_crash_ped(rage::joaat_t model)
	{
		if (std::ranges::any_of(crash_peds, [model](uint32_t crash_ped) { return crash_ped == model; }))
			return true;
		if (!model_info::is_model_of_type(model, eModelType::Ped, eModelType::OnlineOnlyPed))
			return true;
		return false;
	}

	bool is_crash_vehicle(rage::joaat_t model)
	{
		if (std::ranges::any_of(crash_vehicles, [model](uint32_t crash_vehicle) { return crash_vehicle == model; }))
			return true;

		if (!model_info::is_model_of_type(model, eModelType::Vehicle, eModelType::Unk133))
			return true;

		return false;
	}

	bool is_valid_player_model(rage::joaat_t model)
	{
		return std::ranges::any_of(valid_player_models, [model](rage::joaat_t valid_player_model) { return valid_player_model == model; });
	}

	bool is_cage_object(rage::joaat_t model)
	{
		return std::ranges::any_of(cage_objects, [model](rage::joaat_t cage_object) { return cage_object == model; });
	}
}
