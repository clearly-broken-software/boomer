/*
TODO: Add License
*/

/*
see https://distrho.github.io/DPF/group__PluginMacros.html
*/
#pragma once


#define DISTRHO_PLUGIN_BRAND "Clearly Broken Software"
#define DISTRHO_PLUGIN_NAME "Boomer"
#define DISTRHO_PLUGIN_URI "http://github.com/clearly-broken-software/boomer"

#define DISTRHO_PLUGIN_HAS_UI 1
#define DISTRHO_PLUGIN_IS_RT_SAFE 1
#define DISTRHO_PLUGIN_NUM_INPUTS 0
#define DISTRHO_PLUGIN_NUM_OUTPUTS 2
#define DISTRHO_PLUGIN_IS_SYNTH 1
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 0
#define DISTRHO_PLUGIN_WANT_LATENCY 0
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0
#define DISTRHO_PLUGIN_WANT_PROGRAMS 0
#define DISTRHO_PLUGIN_WANT_STATE 1
#define DISTRHO_PLUGIN_WANT_FULL_STATE 1
#define DISTRHO_PLUGIN_WANT_TIMEPOS 1
#define DISTRHO_UI_USE_NANOVG 1
#define DISTRHO_UI_USER_RESIZABLE 0
// #define DISTRHO_UI_URI DISTRHO_PLUGIN_URI "#UI"

enum Parameters
{
    kParameterBufferSize = 0,
    kParameterTimePlaying,
    kParameterTimeFrame,
    kParameterTimeValidBBT,
    kParameterTimeBar,
    kParameterTimeBeat,
    kParameterTimeTick,
    kParameterTimeBarStartTick,
    kParameterTimeBeatsPerBar,
    kParameterTimeBeatType,
    kParameterTimeTicksPerBeat,
    kParameterTimeBeatsPerMinute,
    kParameterCount
};

