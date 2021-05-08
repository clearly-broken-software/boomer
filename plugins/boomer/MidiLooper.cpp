// MidiLooper.cpp
#include "MidiLooper.hpp"

START_NAMESPACE_DISTRHO
// default ctor
MidiLooper::MidiLooper() : eventCount(0),
                           globalTick(0.0),
                           increment(0.0),
                           ready(false),
                           totalTicks(0),
                           playing(false),
                           firstLoop(true),
                           offSetTicks(0),
                           callBack(nullptr),
                           samplerate(0.0),
                           midifileTPQ(0),
                           songTotalTicks(0),
                           totalBars(0),
                           timeSigLower(0.0),
                           timeSigUpper(0.0),
                           ticks_per_bar(0.0),
                           transportTPB(0.0)
{
}

MidiLooper::MidiLooper(smf::MidiFile midifile,
                       const double &sr, Callback *cb) : callBack(cb),
                                                         samplerate(sr),
                                                         eventCount(0),
                                                         globalTick(0.0),
                                                         increment(0.0),
                                                         ready(false),
                                                         totalTicks(0),
                                                         playing(false),
                                                         firstLoop(true),
                                                         offSetTicks(0),

                                                         midifileTPQ(0),
                                                         songTotalTicks(0),
                                                         totalBars(0),
                                                         timeSigLower(0.0),
                                                         timeSigUpper(0.0),
                                                         ticks_per_bar(0.0),
                                                         transportTPB(0.0)
{
    mf = midifile;
    mf.joinTracks();
    getTimeSig();
}

void MidiLooper::setCallback(Callback *cb)
{
    callBack = cb;
}

void MidiLooper::sendEvent(smf::MidiMessage m, int delay)
{
    callBack->newEvent(this, m, delay);
}

void MidiLooper::endOfFile()
{
    callBack->midifileEnd();
}

void MidiLooper::setBBT(const TimePosition &timePos, double ratio)
{
    // get increment
    const double beat_in_seconds = 60. / timePos.bbt.beatsPerMinute;
    const double beat_in_samples = beat_in_seconds * samplerate;
    const double quarter_in_samples = 4. / timeSigLower * beat_in_samples;
    // ticksPerBeat to ticks per Quarternote
    const double beatLowerMultiplier = timePos.bbt.beatType / 4.;
    const double ticksPerQuarternote = timePos.bbt.ticksPerBeat * beatLowerMultiplier;
    const double tick_in_samples = quarter_in_samples / ticksPerQuarternote;
    increment = 1.0f / tick_in_samples;

    // set global tick
    const double barStartTick = timePos.bbt.barStartTick;
    const double ticksPerBeat = timePos.bbt.ticksPerBeat;
    const int beat = timePos.bbt.beat;
    const int tick = timePos.bbt.tick;
    const int currentTick = barStartTick +
                            (beat - 1) * ticksPerBeat +
                            tick;
    const int tt = static_cast<int>(static_cast<double>(songTotalTicks) * ratio);
    globalTick = static_cast<float>(currentTick % tt) - offSetTicks;
    // globalTick = currentTick;
    //printf("globalTick %f\n", globalTick);
}

double MidiLooper::getTPQ()
{
    return mf.getTPQ();
}

void MidiLooper::tick(int delay, double ratio)
{
    const int t = static_cast<double>(mf[0][eventCount].tick) * ratio;
    const int maxEvents = mf[0].getEventCount();
    const int gt = static_cast<int>(globalTick);
    if (gt == t)
    {
        while (eventCount < maxEvents &&
               static_cast<int>(static_cast<double>(mf[0][eventCount].tick) * ratio) == t)
        {
            sendEvent(mf[0][eventCount], delay);
            eventCount++;
        }
    }
    globalTick += increment;
    if (globalTick > (totalTicks * ratio))
    {
        globalTick -= totalTicks * ratio;
    }

    if (eventCount >= maxEvents)
    {
        eventCount = 0;
        firstLoop = true;
        endOfFile();
    }
}

int MidiLooper::getCurrentEventTick()
{
    return mf[0][eventCount].tick;
}

int MidiLooper::getEventIndex(int tick)
{
    for (int i = 0; i < mf.getEventCount(0); ++i)
    {
        if (mf[0][i].tick >= tick)
            return i;
    }
    return 0;
}

void MidiLooper::setEventIndex(uint index)
{
    if (index < mf[0].getEventCount())
        eventCount = index;
}

void MidiLooper::setTPQ(int songTPQ)
{
    const double mfTPQ = mf.getTPQ();
    if (static_cast<int>(mfTPQ) != songTPQ)
    {
        const auto ratio = static_cast<double>(songTPQ) / mfTPQ;
        for (int i = 0; i < mf[0].getEventCount(); i++)
        {
            const double t = mf[0][i].tick;
            mf[0][i].tick = t * ratio;
        }
        mf.setTicksPerQuarterNote(songTPQ);
    }
}

void MidiLooper::setOffsetTicks(int ticks)
{
    offSetTicks = ticks;
}

void MidiLooper::setSongTotalTicks(int ticks)
{
    songTotalTicks = ticks;
}

void MidiLooper::setTotalTicks()
{
    // get totalTicks
    // quarter notes per bar
    float beat_in_quarternotes = timeSigLower / 4.0f;
    float quarternotes_per_bar = timeSigUpper / beat_in_quarternotes;
    // ticks per bar
    ticks_per_bar = quarternotes_per_bar * mf.getTicksPerQuarterNote();

    /* 
        smf::getFildeDurationInTics() returns the tick of the last event,
        tick 0 still means a length of 1 
    */
    float durInTicks = mf.getFileDurationInTicks() + 1;
    float bars = durInTicks / ticks_per_bar;
    // round up the number of bars in the file
    int totalBars = static_cast<int>(ceilf(bars));
    totalTicks = totalBars * static_cast<int>(ticks_per_bar);
}

int MidiLooper::getTotalTicks()
{
    return totalTicks;
}

void MidiLooper::getTimeSig()
{
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
}

END_NAMESPACE_DISTRHO
