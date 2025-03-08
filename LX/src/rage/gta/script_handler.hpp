#pragma once

#pragma pack(push, 1)
namespace rage
{
	class scriptResourceEntry
	{
	public:
		scriptResource* m_data;     // 0x00
		uint32_t m_unk;        // 0x04
		char m_padding[0x0C];       // 0x0C
		scriptResourceEntry* m_next;// 0x18
	};
}

class CGameScriptHandlerNetwork : public CGameScriptHandler
{
public:
	uint8_t m_0xA0;       // 0xA0
	uint8_t m_0xA1;       // 0xA1
	uint8_t m_0xA2;       // 0xA2
	uint8_t m_0xA3;       // 0xA3
	uint8_t m_num_players;// 0xA4
	uint8_t m_0xA5;       // 0xA5
	uint8_t m_0xA6;       // 0xA6
	uint8_t m_0xA7;       // 0xA7
	uint8_t m_0xA8;       // 0xA8
	uint8_t m_0xA9;       // 0xA9
	uint8_t m_0xAA;       // 0xAA
	uint8_t m_0xAB;       // 0xAB
	uint8_t m_0xAC;       // 0xAC
	uint8_t m_0xAD;       // 0xAD
	uint8_t m_0xAE;       // 0xAE
	uint8_t m_0xAF;       // 0xAF
};

class CRemoteScriptInfo : public CGameScriptId
{
public:
	uint32_t m_participants;     //0x0040
	char pad_0044[4];            //0x0044
	class CNetGamePlayer* m_host;//0x0048
	uint32_t m_timestamp_2;      //0x0050
	uint16_t m_host_token;       //0x0054
	uint8_t m_reserved_peds;     //0x0056
	uint8_t m_reserved_vehicles; //0x0057
	uint8_t m_reserved_objects;  //0x0058
	uint8_t m_reserved_unk1;     //0x0059
	uint8_t m_reserved_unk2;     //0x005A
	uint8_t m_reserved_unk3;     //0x005B
	uint8_t m_reserved_unk4;     //0x005C
	char pad_005D[3];            //0x005D
};                               //Size: 0x0060
static_assert(sizeof(CRemoteScriptInfo) == 0x60);

static_assert(sizeof(rage::scriptHandler) == 0x60);
static_assert(sizeof(CGameScriptHandler) == 0xA0);
static_assert(sizeof(CGameScriptHandlerNetwork) == 0xB0);
#pragma pack(pop)
