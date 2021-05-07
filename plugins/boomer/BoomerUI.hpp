#ifndef BOOMER_UI_HPP_DEFINED
#define BOOMER_UI_HPP_DEFINED

#include "DistrhoPluginInfo.h"
#include "DistrhoUI.hpp"
#include "filesystem.hpp"
#include "ListView.hpp"
#include "fonts.hpp"
#include "CB_Colors.hpp"
#include <iostream>

namespace fs = ghc::filesystem;

START_NAMESPACE_DISTRHO

class BoomerUI : public UI,
                 ListView::Callback
{
public:
    BoomerUI();

protected:
    void parameterChanged(uint32_t index, float value) override;
    void sampleRateChanged(double newSampleRate) override;
    void onNanoDisplay() override;
    void onListViewClicked(ListView *listview, ghc::filesystem::path p) override;
    void stateChanged(const char *key, const char *value);

private:
    void makeTree(fs::path p, std::vector<fs::path> &tree);
    fs::path midiDir;
    std::unique_ptr<ListView> fListView;
    //ScopedPointer<ListView> fListView;

    // Parameters
    float fParameters[kParameterCount];
    double fSampleRate;

    // UI stuff
    FontId fFont;
    float fScale;

    // temp buf for text
    char fStrBuf[0xff + 1];

    // helpers for putting text into fStrBuf and returning it
    const char *getTextBufInt(const int value);

    const char *getTextBufFloat(const float value);

    const char *getTextBufTime(const uint64_t frame);

    // helpers for drawing text
    void drawLeft(const float x, const float y, const char *const text);

    void drawRight(const float x, const float y, const char *const text);

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoomerUI)
};

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI *createUI()
{
    return new BoomerUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif
