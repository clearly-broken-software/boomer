/* 
TODO: Add license
 */
#include "BoomerPlugin.hpp"

START_NAMESPACE_DISTRHO

BoomerPlugin::BoomerPlugin() : Plugin(kParameterCount, 0, 0)
{
  tempo = 120.;
  // init the synth
  sampleRate = getSampleRate();
  synth.setSampleRate(sampleRate);
  synth.loadSfzFile("/home/rob/git/boomer/plugins/boomer/AVL_Drumkits_1.1-fix/Black_Pearl_5pc.sfz");
  midifile.read("/home/rob/git/boomer/plugins/boomer/test.mid");

  int tracks = midifile.getTrackCount();
  std::cout << "TPQ: " << midifile.getTicksPerQuarterNote() << std::endl;

  for (int track = 0; track < tracks; track++)
  {
    std::cout << "Tick\tMessage" << std::endl;
    for (int event = 0; event < midifile[track].size(); event++)
    {
      std::cout << std::dec << midifile[track][event].tick;
      std::cout << '\t' << std::hex;
      for (int i = 0; i < midifile[track][event].size(); i++)
        std::cout << (int)midifile[track][event][i] << ' ';
      std::cout << std::endl;
    }
  }
}

void BoomerPlugin::initParameter(uint32_t index, Parameter &parameter)
{
  parameter.hints = kParameterIsAutomable | kParameterIsOutput;
  parameter.ranges.def = 0.0f;
  parameter.ranges.min = 0.0f;
  parameter.ranges.max = 16777216.0f;

  switch (index)
  {
  case kParameterBufferSize:
    parameter.name = "BufferSize";
    parameter.symbol = "buffer_size";
    break;
  case kParameterTimePlaying:
    parameter.hints |= kParameterIsBoolean;
    parameter.name = "TimePlaying";
    parameter.symbol = "time_playing";
    parameter.ranges.min = 0.0f;
    parameter.ranges.max = 1.0f;
    break;
  case kParameterTimeFrame:
    parameter.name = "TimeFrame";
    parameter.symbol = "time_frame";
    break;
  case kParameterTimeValidBBT:
    parameter.hints |= kParameterIsBoolean;
    parameter.name = "TimeValidBBT";
    parameter.symbol = "time_validbbt";
    parameter.ranges.min = 0.0f;
    parameter.ranges.max = 1.0f;
    break;
  case kParameterTimeBar:
    parameter.name = "TimeBar";
    parameter.symbol = "time_bar";
    break;
  case kParameterTimeBeat:
    parameter.name = "TimeBeat";
    parameter.symbol = "time_beat";
    break;
  case kParameterTimeTick:
    parameter.name = "TimeTick";
    parameter.symbol = "time_tick";
    break;
  case kParameterTimeBarStartTick:
    parameter.name = "TimeBarStartTick";
    parameter.symbol = "time_barstarttick";
    break;
  case kParameterTimeBeatsPerBar:
    parameter.name = "TimeBeatsPerBar";
    parameter.symbol = "time_beatsperbar";
    break;
  case kParameterTimeBeatType:
    parameter.name = "TimeBeatType";
    parameter.symbol = "time_beattype";
    break;
  case kParameterTimeTicksPerBeat:
    parameter.name = "TimeTicksPerBeat";
    parameter.symbol = "time_ticksperbeat";
    break;
  case kParameterTimeBeatsPerMinute:
    parameter.name = "TimeBeatsPerMinute";
    parameter.symbol = "time_beatsperminute";
    break;
  }
}

float BoomerPlugin::getParameterValue(uint32_t index) const
{
  return fParameters[index];
}

void BoomerPlugin::setParameterValue(uint32_t index, float value)
{
  // do stuff
}

void BoomerPlugin::run(const float **, float **outputs, uint32_t frames,
                       const MidiEvent *midiEvents, uint32_t midiEventCount)
{
  float *outLeft = outputs[0];
  float *outRight = outputs[1];
  for (uint32_t i = 0; i < frames; ++i)
  {
    outLeft[i] = 0;
    outRight[i] = 0;
  }
  // get time position
  const TimePosition &timePos(getTimePosition());

  // set basic values
  fParameters[kParameterTimePlaying] = timePos.playing ? 1.0f : 0.0f;
  fParameters[kParameterTimeFrame] = timePos.frame;
  fParameters[kParameterTimeValidBBT] = timePos.bbt.valid ? 1.0f : 0.0f;

  // set bbt
  if (timePos.bbt.valid)
  {
    if (!looper.ready)
    {
      looper = MidiLooper(midifile,
                          sampleRate,
                          timePos);
      looper.setCallback(this);
    }
    else
    {
      if (timePos.playing)
      {
        looper.setBBT(timePos);
        for (int delay = 0; delay < frames; ++delay)
        {
          looper.tick(delay);
        }
      }
    }

    fParameters[kParameterTimeBar] = timePos.bbt.bar;
    fParameters[kParameterTimeBeat] = timePos.bbt.beat;
    fParameters[kParameterTimeTick] = timePos.bbt.tick;
    fParameters[kParameterTimeBarStartTick] = timePos.bbt.barStartTick;
    fParameters[kParameterTimeBeatsPerBar] = timePos.bbt.beatsPerBar;
    fParameters[kParameterTimeBeatType] = timePos.bbt.beatType;
    fParameters[kParameterTimeTicksPerBeat] = timePos.bbt.ticksPerBeat;
    fParameters[kParameterTimeBeatsPerMinute] = timePos.bbt.beatsPerMinute;
  }
  else
  {
    fParameters[kParameterTimeBar] = 0.0f;
    fParameters[kParameterTimeBeat] = 0.0f;
    fParameters[kParameterTimeTick] = 0.0f;
    fParameters[kParameterTimeBarStartTick] = 0.0f;
    fParameters[kParameterTimeBeatsPerBar] = 0.0f;
    fParameters[kParameterTimeBeatType] = 0.0f;
    fParameters[kParameterTimeTicksPerBeat] = 0.0f;
    fParameters[kParameterTimeBeatsPerMinute] = 0.0f;
  }
  synth.renderBlock(outputs, frames, 2);
}
void BoomerPlugin::bufferSizeChanged(uint32_t newBufferSize)
{
  fParameters[kParameterBufferSize] = newBufferSize;
}

void BoomerPlugin::newEvent(MidiLooper *lp, smf::MidiMessage message, int delay)
{
  if (message.isNoteOn())
  {
    const int nn = message.getKeyNumber();
    const int vv = message.getVelocity();
    synth.noteOn(delay, nn, vv);
    return;
  }
  if (message.isNoteOff())
  {
    const int nn = message.getKeyNumber();
    const int vv = message.getVelocity();
    synth.noteOff(delay, nn, vv);
    return;
  }
}

END_NAMESPACE_DISTRHO
