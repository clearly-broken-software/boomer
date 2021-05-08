/* 
TODO: Add license
 */

#ifndef BOOMER_PLUGIN_HPP_INCLUDED
#define BOOMER_PLUGIN_HPP_INCLUDED

#include "DistrhoPlugin.hpp"
#include "DistrhoPluginInfo.h"
#include "MidiFile.h"
#include "MidiHelpers.h"
#include "MidiLooper.hpp"
#include "filesystem.hpp"
#include "sfizz.hpp"
#include "Song.hpp"
#include <iostream>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Template plugin for audio in/out
 */
class BoomerPlugin : public Plugin,
                     public MidiLooper::Callback
{
public:
  BoomerPlugin();

protected:
  const char *getLabel() const override
  {
    return "Boomer";
  }

  /**
      Get an extensive comment/description about the plugin.
  **/
  const char *getDescription() const override
  {
    return "because drums go boom";
  }

  const char *getMaker() const override
  {
    return "Clearly Broken Software";
  }

  const char *getHomePage() const override
  {
    return "http://github.com/clearly-broken-software/boomer";
  }

  const char *getLicense() const override
  {
    return "GPLv3";
  }

  uint32_t getVersion() const override
  {
    return d_version(1, 0, 0);
  }

  int64_t getUniqueId() const override
  {
    return d_cconst('B', 'o', 'm', 'R');
  }

  void initState(uint32_t index, String &stateKey, String &defaultStateValue) override;
  void setState(const char *key, const char *value) override;
  String getState(const char *key) const;
  void initParameter(uint32_t index, Parameter &parameter) override;
  float getParameterValue(uint32_t index) const override;
  void setParameterValue(uint32_t index, float value) override;
  void run(const float **, float **outputs, uint32_t frames, const MidiEvent *midiEvents, uint32_t midiEventCount) override;
  void bufferSizeChanged(uint32_t newBufferSize) override;

  void newEvent(MidiLooper *lp, smf::MidiMessage ev, int delay) override;
  void midifileEnd() override;

private:
  void loadMidifile(const char *path);
  const float transportTPQ{1920.f};
  Song song;
  ghc::filesystem::path mf_path;
  double sampleRate;
  sfz::Sfizz synth;
  double tempo;
  
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoomerPlugin)
};

Plugin *createPlugin()
{
  return new BoomerPlugin();
}

END_NAMESPACE_DISTRHO

#endif // BOOMER_PLUGIN_HPP_INCLUDED
