#pragma once

class CWeaponInfoManager
{
public:
	char field_0x0[16][4];
	rage::atArray<CWeaponInfo*> m_item_infos;
};