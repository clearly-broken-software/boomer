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
    // Parameters
    double fSampleRate;

    // UI stuff
    FontId fFont;
    float fScale;

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
