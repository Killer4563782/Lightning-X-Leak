#include "hooking/hooking.hpp"

int hooks::get_battle_eye_init_state() {

	return false;
}

uint32_t hooks::can_access_network_features(const eNetworkAccessArea accessArea, eNetworkAccessCode* nAccessCode) {

	if (nAccessCode)
	{
		*nAccessCode = Access_Granted;
	}

	return g_hooking->get_original<can_access_network_features>()(accessArea, nAccessCode);
}