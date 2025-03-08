#include "lx_customs.h"
#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/icon_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "commands/manager.hpp"
#include "gui/types/number_option.hpp"
#include "commands/settings.h"
#include "wheels.h"
using namespace commands::settings;

namespace ui::submenus
{
	void lx_customs_submenu::on_init()
	{
		this->add(number_option("Power", {}, &lx_customs::power, 99999.f, 0.f, 10.f, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
			VEHICLE::MODIFY_VEHICLE_TOP_SPEED(veh, lx_customs::power);
		}));
		this->add(number_option("Headlight", {}, &lx_customs::headlight, 100.f, 0.f, 1.f, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
			VEHICLE::SET_VEHICLE_LIGHT_MULTIPLIER(veh, lx_customs::headlight);
		}));
		this->add(number_option("Spoiler", {}, &lx_customs::spoiler, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_SPOILER, lx_customs::spoiler, 0);
		}));
		this->add(number_option("Frontbumper", {}, &lx_customs::frontbumper, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_FRONTBUMPER, lx_customs::frontbumper, 0);
		}));
		this->add(number_option("Rearbumper", {}, &lx_customs::rearbumper, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_REARBUMPER, lx_customs::rearbumper, 0);
		}));
		this->add(number_option("Sideskirt", {}, &lx_customs::sideskirt, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_SIDESKIRT, lx_customs::sideskirt, 0);
		}));
		this->add(number_option("Exhaust", {}, &lx_customs::exhaust, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_EXHAUST, lx_customs::exhaust, 0);
		}));
		this->add(number_option("Chassis", {}, &lx_customs::chassis, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_CHASSIS, lx_customs::chassis, 0);
		}));
		this->add(number_option("Grille", {}, &lx_customs::grille, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_GRILLE, lx_customs::grille, 0);
		}));
		this->add(number_option("Hood", {}, &lx_customs::hood, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_HOOD, lx_customs::hood, 0);
		}));
		this->add(number_option("Fender", {}, &lx_customs::fender, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_FENDER, lx_customs::fender, 0);
		}));
		this->add(number_option("Rightfender", {}, &lx_customs::rightfender, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_RIGHTFENDER, lx_customs::rightfender, 0);
		}));
		this->add(number_option("Roof", {}, &lx_customs::roof, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_ROOF, lx_customs::roof, 0);
		}));
		this->add(number_option("Engine", {}, &lx_customs::engine, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_ENGINE, lx_customs::engine, 0);
		}));
		this->add(number_option("Brakes", {}, &lx_customs::brakes, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_BRAKES, lx_customs::brakes, 0);
		}));
		this->add(number_option("Transmission", {}, &lx_customs::transmission, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_TRANSMISSION, lx_customs::transmission, 0);
		}));
		this->add(number_option("Horns", {}, &lx_customs::horns, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_HORNS, lx_customs::horns, 0);
		}));
		this->add(number_option("Supension", {}, &lx_customs::supension, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_SUSPENSION, lx_customs::supension, 0);
		}));
		this->add(number_option("Armor", {}, &lx_customs::armor, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_ARMOR, lx_customs::armor, 0);
		}));
		this->add(number_option("Turbo", {}, &lx_customs::turbo, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_TURBO, lx_customs::turbo, 0);
		}));
		this->add(number_option("Livery", {}, &lx_customs::livery, 30, 0, 1, [this] () {
			Vehicle veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false); VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0); for (int i = 0; i < 50; i++)
				(VEHICLE::GET_NUM_VEHICLE_MODS(veh, i) - 1, false);
			VEHICLE::SET_VEHICLE_MOD(veh, MOD_LIVERY, lx_customs::livery, 0);
		}));
		this->add(&g_wheel_submenu);

		

	}

	lx_customs_submenu g_lx_customs_submenu("Lx Customs", "Want to pimp out your ride you came to the right place");
}
