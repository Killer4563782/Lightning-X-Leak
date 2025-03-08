#pragma once

#include <cstdint>
#include "fwArchetype.hpp"

enum class eModelType : std::uint8_t
{
    Invalid = 0,
    Object = 1,
    MLO = 2,
    Time = 3,
    Weapon = 4,
    Vehicle = 5,
    Ped = 6,
    Destructable = 7,
    WorldObject = 33,
	Sprinkler = 35,
	Unk65 = 65,
	EmissiveLOD = 67,
	Plant = 129,
	LOD = 131,
	Unk132 = 132,
	Unk133 = 133,
	OnlineOnlyPed = 134,
	Building = 161,
	Unk193 = 193
};

#pragma pack(push, 8)
class CBaseModelInfo : public rage::fwArchetype
{
public:
    char pad_0070[8]; //0x0070
    uint64_t unk_0078; //0x0078
    uint64_t unk_0080; //0x0080
    char pad_0088[8]; //0x0088
    uint64_t unk_0090; //0x0090
    char pad_0098[5]; //0x0098
    eModelType  m_model_type; //0x009D
    char pad_009E[6]; //0x009E
    uint64_t unk_00A8; //0x00A8

    uint8_t get_type()
	{
		return static_cast<int>(m_model_type) & 0xFF;
	}

	bool is_type(eModelType type)
	{
		return get_type() == static_cast<uint8_t>(type);
	}

	bool of_any_type(const std::vector<eModelType>& types)
	{
		for (const eModelType& type : types)
		{
			if (is_type(type))
			{
				return true;
			}
		}
		return false;
	}
}; //Size: 0x00B0
static_assert(sizeof(CBaseModelInfo) == 0xB0);
#pragma pack(pop)
