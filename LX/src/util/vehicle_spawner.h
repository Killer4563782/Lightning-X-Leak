#pragma once
#include "protections.h"
#include "helpers/gxtLabelHelpers.h"

namespace util::vehicle
{
    class vehicle_data
    {
    public:
        vehicle_data(CVehicleModelInfo* model_info, std::string manufacturer_label, std::string name_label) :
            m_model_info(model_info),
            m_manufacturer_label(std::move(manufacturer_label)),
            m_name_label(std::move(name_label))
        {}

        CVehicleModelInfo* m_model_info{};
        std::string m_manufacturer_label{};
        std::string m_name_label{};
    };

    class models
    {
    public:
        std::vector<vehicle_data>& get(eVehicleClass vehicle_class)
        {
            if (const auto it = m_vehicleClassMap.find(vehicle_class); it != m_vehicleClassMap.end())
            {
                return it->second;
            }
            return m_dummy;
        }

    private:
        std::map<eVehicleClass, std::vector<vehicle_data>> m_vehicleClassMap{
            {eVehicleClass::VC_COMPACT, {}},
            {eVehicleClass::VC_SEDAN, {}},
            {eVehicleClass::VC_SUV, {}},
            {eVehicleClass::VC_COUPE, {}},
            {eVehicleClass::VC_MUSCLE, {}},
            {eVehicleClass::VC_SPORT_CLASSIC, {}},
            {eVehicleClass::VC_SPORT, {}},
            {eVehicleClass::VC_SUPER, {}},
            {eVehicleClass::VC_MOTORCYCLE, {}},
            {eVehicleClass::VC_OFF_ROAD, {}},
            {eVehicleClass::VC_INDUSTRIAL, {}},
            {eVehicleClass::VC_UTILITY, {}},
            {eVehicleClass::VC_VAN, {}},
            {eVehicleClass::VC_CYCLE, {}},
            {eVehicleClass::VC_BOAT, {}},
            {eVehicleClass::VC_HELICOPTER, {}},
            {eVehicleClass::VC_PLANE, {}},
            {eVehicleClass::VC_SERVICE, {}},
            {eVehicleClass::VC_EMERGENCY, {}},
            {eVehicleClass::VC_MILITARY, {}},
            {eVehicleClass::VC_COMMERCIAL, {}},
            {eVehicleClass::VC_RAIL, {}},
            {eVehicleClass::VC_OPEN_WHEEL, {}}
        };
        std::vector<vehicle_data> m_dummy{};
    };

    inline size_t g_selected_vehicle_from_class;
    inline uint8_t g_selected_vehicle_class;
    inline models g_models{};
    inline std::vector<std::string> g_model_classes{};

    inline HashTable<CVehicleModelInfo*>& get_hash_table()
    {
        return *reinterpret_cast<HashTable<CVehicleModelInfo*>*>(g_pointers->g_hash_table);
    }

    inline std::string get_vehicle_class_name(const uint8_t veh_class)
    {
        const std::string class_id = std::to_string(veh_class);
        const std::string label = "VEH_CLASS_" + class_id;
        const auto value = gxt::get_gxt_label(label);

        if (!value || value->empty())
        {
            return "Unknown Class (" + class_id + ")";
        }

        return *value;
    }

    inline void cache_model_table()
    {
        constexpr uint8_t num_vehicle_classes = 23;
        g_model_classes.reserve(num_vehicle_classes);

        for (uint8_t i = 0; i < num_vehicle_classes; ++i)
        {
            g_model_classes.push_back(get_vehicle_class_name(i));
        }

        const auto& [m_data, m_size, pad_000A, m_item_size, pad_0020, m_lookup_table, m_lookup_key] = get_hash_table();
        for (int i = 0; i < m_size; ++i)
        {
            for (const HashNode* node = m_lookup_table[i]; node; node = node->m_next)
            {
                if (const uint16_t table_idx = node->m_idx; table_idx != m_size)
                {
                    if (CVehicleModelInfo* model = m_data[table_idx]; model && model->is_type(eModelType::Vehicle))
                    {
                        if (protections::is_crash_vehicle(model->m_hash))
                        {
                            continue; // skip unwanted / next gen unavailable vehicles
                        }

                        auto manufacturer_label = gxt::get_gxt_label(model->m_manufacturer);
                        auto name_label = gxt::get_gxt_label(model->m_name, true);
                        g_models.get(static_cast<eVehicleClass>(model->get_class())).emplace_back(model, manufacturer_label.value_or(""), name_label.value());
                    }
                }
            }
        }
    }
}