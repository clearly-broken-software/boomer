// MidiLooper.cpp
#include "MidiLooper.hpp"

START_NAMESPACE_DISTRHO

MidiLooper::MidiLooper() : eventCount(0),
                           globalTick(0.0f),
                           increment(0.0f),
                           ready(false),
                           totalTicks(0)
{
}

MidiLooper::MidiLooper(smf::MidiFile &f,
                       const double &samplerate,
                       const TimePosition &timePos)
{
    mf = f;
    eventCount = 0;
    globalTick = 0.0f;
    //set midifile tpq to transport tpq
    float transportTimeSigLower = timePos.bbt.beatType;
    //  float transportTImeSigUpper = timePos.bbt.beatsPerBar;
    float transportTicksPerBeat = timePos.bbt.ticksPerBeat;
    float transportBPM = timePos.bbt.beatsPerMinute;
    float transportTPQ = transportTimeSigLower / 4.0f * transportTicksPerBeat;
    mf.joinTracks();

    float mfTPQ = mf.getTicksPerQuarterNote();
    float ratio = transportTPQ / mfTPQ;
    for (int i = 0; i < mf[0].size(); ++i)
    {
        float event_tick = mf[0][i].tick;
        mf[0][i].tick = static_cast<int>(std::round(event_tick * ratio));
    }

    // get increment
    timeSigLower = 4;
    timeSigUpper = 4;
    for (int i = 0; i < mf[0].size(); ++i)
    {
        if (mf[0][i].isTimeSignature())
        {
            timeSigLower = std::pow(2, mf[0][i][4]);
            timeSigUpper = mf[0][i][3];
        }
    }

    float beat_in_seconds = 60.f / transportBPM;
    float beat_in_samples = beat_in_seconds * samplerate;
    float quarter_in_samples = 4.0f / static_cast<float>(timeSigLower) * beat_in_samples;
    float tick_in_samples = quarter_in_samples / transportTPQ;
    increment = 1.0f / tick_in_samples;

    // calc totalTicks
    // quarter notes per bar
    float beat_in_quarternotes = timeSigLower / 4.0f;
    float quarternotes_per_bar = timeSigUpper / beat_in_quarternotes;
    // ticks per bar
    ticks_per_bar = quarternotes_per_bar * transportTPQ;

    // round up the number of bars in the file
    float durInTicks = mf.getFileDurationInTicks();
    float bars = durInTicks / ticks_per_bar;
    float p = bars / ticks_per_bar;
    int totalBars = static_cast<int>(ceilf(bars));

    totalTicks = totalBars * static_cast<int>(ticks_per_bar);

    ready = true;
}

void MidiLooper::setCallback(Callback *cb)
{
    callBack = cb;
}

void MidiLooper::sendEvent(smf::MidiMessage m, int delay)
{
    callBack->newEvent(this, m, delay);
}

void MidiLooper::setBBT(const TimePosition &timePos)
{
    int barStartTick = timePos.bbt.barStartTick;
    int ticksPerBeat = timePos.bbt.ticksPerBeat;
    // float bar = timePos.bbt.bar;
    int beat = timePos.bbt.beat;
    int tick = timePos.bbt.tick;
    int currentTick = barStartTick + (beat - 1) * ticksPerBeat + tick;
    globalTick = static_cast<float>(currentTick % totalTicks);
    // printf ("globalTick %i\n",static_cast<int>(globalTick));
}

bool MidiLooper::tick(int delay)
{
    int t = mf[0][eventCount].tick;
    int maxEvents = mf[0].getEventCount();
    int gt = globalTick;
    //std::cout << std::dec << gt << '\t' << t << ', ';

    if (static_cast<int>(globalTick) == t)
    {
        while (eventCount < maxEvents && mf[0][eventCount].tick == t)
        {
            std::cout << std::dec << mf[0][eventCount].tick;
            std::cout << '\t' << std::hex;
            for (int i = 0; i < mf[0][eventCount].size(); i++)
                std::cout << (int)mf[0][eventCount][i] << ' ';
            std::cout << std::endl;

            sendEvent(mf[0][eventCount], delay);
            eventCount++;
        }
    }
    globalTick += increment;
    if (globalTick > totalTicks)
        globalTick -= totalTicks;

    if (eventCount >= maxEvents)
        eventCount = 0;

    return false;
}

END_NAMESPACE_DISTRHO
