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
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void newEvent(MidiLooper *lp, smf::MidiMessage message, int delay) = 0;
        virtual void midifileEnd() = 0;
    };
    //constructors
    MidiLooper();
    MidiLooper(smf::MidiFile midifile, const double &samplerate, Callback *cb);
    // getters
    void setTotalTicks();
    void setSongTotalTicks(int ticks);
    void getTimeSig();
    int getTotalTicks();
    int getCurrentEventTick();
    int getEventIndex(int tick);
    double getTPQ(); // TODO: check if needed
    // setters
    void setCallback(Callback *cb); // TODO: check if needed
    void setBBT(const TimePosition &timePos, double ratio);
    void setTPQ(int tpq);
    void setEventIndex(uint index);
    void setOffsetTicks(int tick);
    
    // process
    void tick(int delay, double ratio); // forward the sequence;

    // flags TODO: check if needed
    bool ready;
    bool playing;

private:
    Callback *callBack;
    double samplerate;
    void sendEvent(smf::MidiMessage m, int delay);
    void endOfFile();
    smf::MidiFile mf;
    size_t eventCount;
    double midifileTPQ;
    double increment;
    double globalTick;
    int totalTicks;
    int songTotalTicks;
    int totalBars;
    bool firstLoop;

    double timeSigLower;
    double timeSigUpper;
    double ticks_per_bar;
    double transportTPB;
    int offSetTicks;
};

END_NAMESPACE_DISTRHO

#endif