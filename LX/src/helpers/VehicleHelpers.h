#pragma once

namespace vehicle
{
	void set_mp_bitset(Vehicle veh);
	bool into_vehicle(Vehicle veh);

	void max_vehicle(Vehicle veh, bool request_control = true);
	void max_vehicle_performance(Vehicle veh);
	void downgrade(Vehicle vehicle);
	bool delete_vehicle(Vehicle vehicle);
	bool repair_vehicle(Vehicle vehicle);

	Vehicle spawn_vehicle(Hash hash, const Vector3& location, float heading, bool is_networked, bool script_veh);
	Vehicle spawn_vehicle_for_teleporting(Hash hash, const Vector3& location, float heading, bool is_networked, bool script_veh);
	void spawn_vehicle_by_name();

	void drive_to_waypoint();
	void cancel_driving();

	Vehicle get_personal_vehicle();
	CVehicle* get_personal_cvehicle();

	// Constants
	inline bool seamless_re_enter_vehicle = true;
	inline bool set_ped_in_vehicle_when_spawned = true;
	inline bool set_vehicle_maxed_when_spawned = true;
	inline const char* property_name_vehicle_spawn_in_online = "MPBitset";
}
