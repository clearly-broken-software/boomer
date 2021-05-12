/* 
TODO: Add license
 */
#include "BoomerPlugin.hpp"

START_NAMESPACE_DISTRHO

BoomerPlugin::BoomerPlugin() : Plugin(kParameterCount, 0, 1)
{
  mf_path = ghc::filesystem::current_path();
  tempo = 120.;
  //
  // init the synth
  sampleRate = getSampleRate();
  synth.setSampleRate(sampleRate);
  //synth.loadSfzFile("/home/rob/Muziek/1234_sfz/1234.sfz");
  synth.loadSfzFile("/home/rob/git/boomer/plugins/boomer/AVL_Drumkits_1.1-fix/Black_Pearl_5pc.sfz");

  // init song
  song.setSamplerate(sampleRate);
  song.setCallback(this);
}

String BoomerPlugin::getState(const char *) const
{
  return String(mf_path.c_str());
}

void BoomerPlugin::initState(uint32_t index, String &stateKey, String &defaultStateValue)
{
  switch (index)
  {
  case 0:
    stateKey = "midifile";
    defaultStateValue = "";
    break;

  default:
    break;
  }
}

void BoomerPlugin::setState(const char *key, const char *value)
{
  const MutexLocker cml(fMutex);
  if (std::strcmp(key, "midifile") == 0)
    loadMidifile(value);
}

void BoomerPlugin::initParameter(uint32_t index, Parameter &)
{
  switch (index)
  {
  default:
    printf("BoomerPlugin::initParameter %i\n", index);
    break;
  }
}

float BoomerPlugin::getParameterValue(uint32_t) const
{
  return 0.0f;
  // do stuff
}

void BoomerPlugin::setParameterValue(uint32_t, float)
{
  // do stuff
}

void BoomerPlugin::run(const float **, float **outputs, uint32_t frames,
                       const MidiEvent *, uint32_t)
{
  float *outLeft = outputs[0];
  float *outRight = outputs[1];
  for (uint32_t i = 0; i < frames; ++i)
  {
    outLeft[i] = 0;
    outRight[i] = 0;
  }
  if (!fMutex.tryLock())
  {
    printf("LOCKED! \n");
    // midi data is locked by setState, so we can't access it; do nothing
    return;
  }

  // get time position
  const TimePosition &timePos(getTimePosition());

  // set bbt
  if (timePos.bbt.valid && timePos.playing && song.currentPattern)
  {                                                           // midifile tpq
    const double midifileTPQ = song.currentPattern->getTPQ(); // hard coded to 1920
    // tpBeat to tpQ
    const double beatLowerMultiplier = timePos.bbt.beatType / 4.;
    const double ticksPerQuarternote = timePos.bbt.ticksPerBeat * beatLowerMultiplier;
    const double ratio = ticksPerQuarternote / midifileTPQ;
    song.currentPattern->setBBT(timePos, ratio);
    for (uint32_t delay = 0; delay < frames; ++delay)
    {
      song.currentPattern->tick(delay, ratio);
    }
  }
  fMutex.unlock();
  synth.renderBlock(outputs, frames, 2);
}
void BoomerPlugin::bufferSizeChanged(uint32_t)
{
  // do stuff
}

void BoomerPlugin::newEvent(MidiLooper *, smf::MidiMessage message, int delay)
{
  if (message.isNoteOn())
  {
    const int nn = message.getKeyNumber();
    const int vv = message.getVelocity();
    printf("note %i \n", nn);
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

void BoomerPlugin::midifileEnd()
{
  song.nextPattern();
}

void BoomerPlugin::loadMidifile(const char *path)
{
  song.addPattern(path);
  if (song.currentPattern == nullptr)
    song.nextPattern();
  else
    // pointer gets invalidated after addPattern because of push_back
    song.currentPattern = song.getPatternPtr(song.getClipIndex());
}
END_NAMESPACE_DISTRHO
