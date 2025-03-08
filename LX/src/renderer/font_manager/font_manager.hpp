#pragma once
#include "imgui/backends/imgui_impl_dx11.h"
#include "pch/types.h"

extern std::filesystem::path g_lx_path;

class font
{
public:
    font(std::string_view name, const f32& size, std::span<const u8> memory = {}, ImFontConfig cfg = {})
        : m_config(cfg), m_name(name), m_type(memory.empty() ? 2 : 1), m_size(size)
    {
        if (!memory.empty())
        {
            m_mem.assign(memory.begin(), memory.end());
        }
    }

    font(const std::filesystem::path& path, const f32& size, ImFontConfig cfg = {})
        : m_config(cfg), m_path(path), m_size(size)
    {
        m_name = path.filename().string();
    }

    ImFont* register_in_atlas(ImFontAtlas* atlas)
    {
        switch (m_type)
        {
        case 0:
            strcpy_s(m_config.Name, m_name.data());
            m_font = atlas->AddFontFromFileTTF(m_path.string().c_str(), m_size, &m_config, m_config.GlyphRanges);
            break;
        case 1:
            strcpy_s(m_config.Name, m_name.data());
            if (!m_mem.empty())
            {
                m_font = atlas->AddFontFromMemoryTTF(m_mem.data(), static_cast<int>(m_mem.size()), m_size, &m_config);
            }
            else
            {
                LOG(error, "Failed to load font!");
            }
            break;
        case 2:
            m_font = atlas->AddFontDefault(&m_config);
            break;
        default:
            break;
        }
        return m_font;
    }

    ImFont* imfont() const { return m_font; }
    ImFontConfig& imfontconfig() { return m_config; }
    const std::filesystem::path& path() const { return m_path; }
    const std::string& name() const { return m_name; }
    f32 size() const { return m_size; }

private:
    ImFontConfig m_config{};
    ImFont* m_font{};
    std::vector<u8> m_mem{};
    std::filesystem::path m_path{};
    std::string m_name{};
    s32 m_type{};
    f32 m_size{};
};

class font_mgr
{
public:
    font_mgr() = default;
    ~font_mgr() = default;

    ImFont* add_font_to_atlas(ImFontAtlas* atlas, f32 size = 19.f)
    {
		m_fonts.emplace_back(std::string_view{"default"}, size);
        return m_fonts.back().register_in_atlas(atlas);
    }

    ImFont* add_font_to_atlas(ImFontAtlas* atlas, const std::filesystem::path& path, f32 size = 19.f)
    {
        m_fonts.emplace_back(path, size);
        return m_fonts.back().register_in_atlas(atlas);
    }

    ImFont* add_font_to_atlas(ImFontAtlas* atlas, std::string_view name, std::span<const u8> memory, f32 size = 19.f)
    {
        m_fonts.emplace_back(name, size, memory);
        return m_fonts.back().register_in_atlas(atlas);
    }

    void append_to_previous_font(ImFontAtlas* atlas, const std::filesystem::path& path, const ImWchar* ranges = nullptr, f32 size = 19.f)
    {
        ImFontConfig cfg{};
        cfg.MergeMode = true;
        cfg.PixelSnapH = true;
        cfg.GlyphMinAdvanceX = size;
        cfg.GlyphRanges = ranges;
        m_fonts.emplace_back(path, size, cfg);
        m_fonts.back().register_in_atlas(atlas);
    }

    void update(ImFontAtlas* atlas)
    {
        if (!should_update())
        {
            return;
        }
        ImGui_ImplDX11_InvalidateDeviceObjects();
        atlas->Clear();
        for (auto& f : m_fonts)
        {
            m_fonts_backup.push_back(f);
            f.register_in_atlas(atlas);
        }
        ImGui_ImplDX11_CreateDeviceObjects();
    }

    bool should_update() const
    {
        return m_fonts_backup.empty() || m_fonts.size() != m_fonts_backup.size();
    }

    std::optional<font*> get_font(std::string_view name)
    {
        for (auto& f : m_fonts)
        {
            if (f.name() == name)
            {
                return &f;
            }
        }
        return std::nullopt;
    }

private:
    std::vector<font> m_fonts{};
    std::vector<font> m_fonts_backup{};
};

inline std::unique_ptr<font_mgr> g_font_mgr{};