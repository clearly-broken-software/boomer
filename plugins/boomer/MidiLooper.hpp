#ifndef MIDI_LOOPER_HPP_DEFINED
#define MIDI_LOOPER_HPP_DEFINED

#include "MidiFile.h"
#include "DistrhoPlugin.hpp"
#include "../../dpf/distrho/src/DistrhoDefines.h"
#include <cmath>
#include <iostream>

START_NAMESPACE_DISTRHO

class MidiLooper
{
public:
    MidiLooper();
    MidiLooper(smf::MidiFile &mf,
               const double &samplerate,
               const TimePosition &timePos
    );

    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void newEvent(MidiLooper *lp, smf::MidiMessage message, int delay) = 0;
    };
    bool tick(int delay); // forward the sequence;
    void setCallback(Callback *cb);
    void setBBT(const TimePosition &timePos);
    bool ready;

private:
    Callback *callBack;
    void sendEvent(smf::MidiMessage m, int delay);
    smf::MidiFile mf;
    size_t eventCount;
    float midifileTPQ;
    float increment;
    float globalTick;
    int totalTicks;
    int totalBars;

    int timeSigLower;
    int timeSigUpper;
    float ticks_per_bar;
    float transportTPB;
};

END_NAMESPACE_DISTRHO

#endif