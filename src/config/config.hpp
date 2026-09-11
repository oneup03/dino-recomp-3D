#pragma once

#include <filesystem>
#include <string_view>

#include "json/json.hpp"

#include "input/input.hpp"

namespace dino::config {
    constexpr std::u8string_view program_id = u8"DinoPlanetRecompiled";
    constexpr std::string_view program_name = "Dinosaur Planet: Recompiled";

    void load_config();
    void save_config();
    
    void reset_input_bindings();
    void reset_cont_input_bindings();
    void reset_kb_input_bindings();
    void reset_single_input_binding(dino::input::InputDevice device, dino::input::GameInput input);

    std::filesystem::path get_app_folder_path();
    
    enum class AutosaveMode {
        On,
        Off,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::AutosaveMode, {
        {dino::config::AutosaveMode::On, "On"},
        {dino::config::AutosaveMode::Off, "Off"}
    });

    enum class TargetingMode {
        Switch,
        Hold,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::TargetingMode, {
        {dino::config::TargetingMode::Switch, "Switch"},
        {dino::config::TargetingMode::Hold, "Hold"}
    });

    TargetingMode get_targeting_mode();
    void set_targeting_mode(TargetingMode mode);

    enum class CameraInvertMode {
        InvertNone,
        InvertX,
        InvertY,
        InvertBoth,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::CameraInvertMode, {
        {dino::config::CameraInvertMode::InvertNone, "InvertNone"},
        {dino::config::CameraInvertMode::InvertX, "InvertX"},
        {dino::config::CameraInvertMode::InvertY, "InvertY"},
        {dino::config::CameraInvertMode::InvertBoth, "InvertBoth"}
    });

    CameraInvertMode get_camera_invert_mode();
    void set_camera_invert_mode(CameraInvertMode mode);

    CameraInvertMode get_analog_camera_invert_mode();
    void set_analog_camera_invert_mode(CameraInvertMode mode);

    enum class AnalogCamMode {
        On,
        Off,
		OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::AnalogCamMode, {
        {dino::config::AnalogCamMode::On, "On"},
        {dino::config::AnalogCamMode::Off, "Off"}
    });

    enum class HUDMode {
        Default,
        AlwaysVisible,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::HUDMode, {
        {dino::config::HUDMode::Default, "Default"},
        {dino::config::HUDMode::AlwaysVisible, "AlwaysVisible"}
    });

    enum class MinimapMode {
        Default,
        Hold,
        Hidden,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::MinimapMode, {
        {dino::config::MinimapMode::Default, "Default"},
        {dino::config::MinimapMode::Hold, "Hold"},
        {dino::config::MinimapMode::Hidden, "Hidden"}
    });

    // Stereoscopic 3D. Kept in its own struct rather than added to
    // ultramodern::renderer::GraphicsConfig, because that type lives in the
    // N64ModernRuntime submodule and extending it would mean forking a second
    // submodule for a Dino-only feature.
    enum class StereoMode {
        Off,
        SideBySide,
        TopAndBottom,
        RowInterlaced,
        ColumnInterlaced,
        Checkerboard,
        Anaglyph,
        LeiaSR,
        OptionCount
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(dino::config::StereoMode, {
        {dino::config::StereoMode::Off, "Off"},
        {dino::config::StereoMode::SideBySide, "SideBySide"},
        {dino::config::StereoMode::TopAndBottom, "TopAndBottom"},
        {dino::config::StereoMode::RowInterlaced, "RowInterlaced"},
        {dino::config::StereoMode::ColumnInterlaced, "ColumnInterlaced"},
        {dino::config::StereoMode::Checkerboard, "Checkerboard"},
        {dino::config::StereoMode::Anaglyph, "Anaglyph"},
        {dino::config::StereoMode::LeiaSR, "LeiaSR"}
    });

    struct StereoSettings {
        // The values below are the ones this port was actually tuned on, rather
        // than the conservative placeholders it started with.
        //
        // Off, unlike everything below it. The rest of these are tuning that
        // holds whatever the display is; the mode names specific hardware, and
        // defaulting to a panel the user may not own would greet them with a
        // side-by-side double image on first run.
        StereoMode mode = StereoMode::Off;
        // 0..100, mapping linearly onto 0..0.10 of screen width. Under the
        // clip-space parameterization this IS the projection shear, so the value
        // is the background disparity the viewer sees — see kSeparationPerSlider
        // in rt64_projection_processor.cpp. 30 is 3% of screen width at infinity,
        // comfortably inside the ~10.5% divergence ceiling on a 27-inch panel and
        // lower again in proportion on anything larger.
        int separation = 30;
        // Convergence distance in TENTHS of its slider, 10..1000 (= 1.0..100.0),
        // which is also game units 1:1. Carried in tenths because the
        // depth-driven loop solves for a continuous value and would otherwise
        // quantise into visible 10-unit steps.
        //
        // This is the screen plane, so it also sets where depth stops reading:
        // disparity saturates hyperbolically, and past roughly 2-3x this distance
        // everything is nearly at the background plane whatever its real depth.
        int convergence_tenths = 160;
        // 0..100, 50 = screen plane. The resulting shift scales with separation,
        // so the HUD goes flat along with the world at separation 0.
        int hud_depth = 50;
        // Depth-driven auto-convergence: pulls convergence in when something
        // gets close, never pushes it out past the slider. On by default -- it
        // only ever protects, and the convergence above is a ceiling it cannot
        // exceed, so leaving it off gives up comfort for nothing.
        bool auto_convergence = true;
        // Permitted pop-out for that loop, in thousandths of screen width.
        // Signed: 0 puts the screen plane exactly on the nearest object, and
        // negative values put the whole scene behind the glass.
        int comfort_target = 12;
        // Ghost reduction (anti-crosstalk) range compression, applied in the
        // compose shader. 100 / 0 are exact no-ops.
        int ghost_contrast = 100;
        int ghost_black_floor = 0;

        auto operator<=>(const StereoSettings&) const = default;
    };

    StereoSettings get_stereo_settings();
    void set_stereo_settings(const StereoSettings& settings);

    AutosaveMode get_autosave_mode();
    void set_autosave_mode(AutosaveMode mode);

    AnalogCamMode get_analog_cam_mode();
    void set_analog_cam_mode(AnalogCamMode mode);

    bool get_dinomod_check();
    void set_dinomod_check(bool enabled);

    HUDMode get_hud_mode();
    void set_hud_mode(HUDMode mode);
    MinimapMode get_minimap_mode();
    void set_minimap_mode(MinimapMode mode);

    void reset_sound_settings();
    void set_main_volume(int volume);
    int get_main_volume();
    void set_bgm_volume(int volume);
    int get_bgm_volume();
    void set_sfx_volume(int volume);
    int get_sfx_volume();
    void set_dialog_volume(int volume);
    int get_dialog_volume();
    bool get_subtitles_enabled();
    void set_subtitles_enabled(bool enabled);

    void open_quit_game_prompt();

    bool get_debug_dll_logging_enabled();
    void set_debug_dll_logging_enabled(bool enabled);

    bool get_debug_diprintf_enabled();
    void set_debug_diprintf_enabled(bool enabled);

    int get_debug_reasset_loglevel();
    void set_debug_reasset_loglevel(int level);

    bool get_debug_recompsave_enabled();
    void set_debug_recompsave_enabled(bool enabled);
}