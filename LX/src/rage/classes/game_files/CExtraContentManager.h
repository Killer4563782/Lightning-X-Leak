#pragma once
#include "rage/atArray.hpp"

namespace rage
{
    struct atString
    {
        char* data;
        uint16_t size;
        uint16_t capacity;
    };
    static_assert(sizeof(atString) == 0x10);

    struct SSetupData
    {
        /* 0x00 */ rage::atString m_deviceName;
        /* 0x10 */ rage::atString m_datFile;
        /* 0x20 */ rage::atString m_timeStamp;
        /* 0x30 */ int32_t m_nameHash;
        uint8_t padding[0x54];
    };
    static_assert(sizeof(SSetupData) == 0x88);

    class CMountableContent
    {
    public:
        uint8_t padding1[0x30];
        SSetupData m_setupData;
        /* 0xB8 */ rage::atString m_filename;
        uint8_t padding2[0x28];
    };
    static_assert(sizeof(CMountableContent) == 0xF0);

    class CExtraContentManager
    {
    public:
        uint8_t padding1[0x28];
        rage::atArray<CMountableContent> m_content;
        uint8_t padding2[0x182 - 0x28 - sizeof(rage::atArray<CMountableContent>)];
        bool m_enumerating : 1;
        bool m_enumerateOnUpdate : 1;
        bool m_everHadBadPackOrder : 1;
        bool m_specialTriggerTunablesTested : 1;
    };
}