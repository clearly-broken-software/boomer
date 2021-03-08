/*
* 
*/
#include "DistrhoPluginInfo.h"
#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

class BoomerUI : public UI
{
public:
    BoomerUI()
        : UI(400, 400), fScale(1.0f)
    {
        std::memset(fParameters, 0, sizeof(float) * kParameterCount);
        std::memset(fStrBuf, 0, sizeof(char) * (0xff + 1));

        fSampleRate = getSampleRate();
        fFont = createFontFromFile("sans", "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");
        std::memset(fParameters, 0, sizeof(float) * kParameterCount);
        std::memset(fStrBuf, 0, sizeof(char) * (0xff + 1));

        fSampleRate = getSampleRate();
        fFont = createFontFromFile("sans", "/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf");
    }

protected:
    void parameterChanged(uint32_t index, float value) override
    {
        fParameters[index] = value;
        repaint();
    }

    void sampleRateChanged(double newSampleRate) override
    {
        fSampleRate = newSampleRate;
        repaint();
    }

    void onNanoDisplay() override
    {
        const float lineHeight = 20 * fScale;

        fontSize(15.0f * fScale);
        textLineHeight(lineHeight);

        float x = 0.0f * fScale;
        float y = 15.0f * fScale;

        // buffer size
        drawLeft(x, y, "Buffer Size:");
        drawRight(x, y, getTextBufInt(fParameters[kParameterBufferSize]));
        y += lineHeight;

        // sample rate
        drawLeft(x, y, "Sample Rate:");
        drawRight(x, y, getTextBufFloat(fSampleRate));
        y += lineHeight;

        // nothing
        y += lineHeight;

        // time stuff
        drawLeft(x, y, "Playing:");
        drawRight(x, y, (fParameters[kParameterTimePlaying] > 0.5f) ? "Yes" : "No");
        y += lineHeight;

        drawLeft(x, y, "Frame:");
        drawRight(x, y, getTextBufInt(fParameters[kParameterTimeFrame]));
        y += lineHeight;

        drawLeft(x, y, "Time:");
        drawRight(x, y, getTextBufTime(fParameters[kParameterTimeFrame]));
        y += lineHeight;

        // BBT
        x = 200.0f * fScale;
        y = 15.0f * fScale;

        const bool validBBT(fParameters[kParameterTimeValidBBT] > 0.5f);
        drawLeft(x, y, "BBT Valid:");
        drawRight(x, y, validBBT ? "Yes" : "No");
        y += lineHeight;

        if (!validBBT)
            return;

        drawLeft(x, y, "Bar:");
        drawRight(x, y, getTextBufInt(fParameters[kParameterTimeBar]));
        y += lineHeight;

        drawLeft(x, y, "Beat:");
        drawRight(x, y, getTextBufInt(fParameters[kParameterTimeBeat]));
        y += lineHeight;

        drawLeft(x, y, "Tick:");
        drawRight(x, y, getTextBufInt(fParameters[kParameterTimeTick]));
        y += lineHeight;

        drawLeft(x, y, "Bar Start Tick:");
        drawRight(x, y, getTextBufFloat(fParameters[kParameterTimeBarStartTick]));
        y += lineHeight;

        drawLeft(x, y, "Beats Per Bar:");
        drawRight(x, y, getTextBufFloat(fParameters[kParameterTimeBeatsPerBar]));
        y += lineHeight;

        drawLeft(x, y, "Beat Type:");
        drawRight(x, y, getTextBufFloat(fParameters[kParameterTimeBeatType]));
        y += lineHeight;

        drawLeft(x, y, "Ticks Per Beat:");
        drawRight(x, y, getTextBufFloat(fParameters[kParameterTimeTicksPerBeat]));
        y += lineHeight;

        drawLeft(x, y, "BPM:");
        drawRight(x, y, getTextBufFloat(fParameters[kParameterTimeBeatsPerMinute]));
        y += lineHeight;
    }

    // -------------------------------------------------------------------------------------------------------

private:
    // Parameters
    float fParameters[kParameterCount];
    double fSampleRate;

    // UI stuff
    FontId fFont;
    float fScale;

    // temp buf for text
    char fStrBuf[0xff + 1];

    // helpers for putting text into fStrBuf and returning it
    const char *getTextBufInt(const int value)
    {
        std::snprintf(fStrBuf, 0xff, "%i", value);
        return fStrBuf;
    }

    const char *getTextBufFloat(const float value)
    {
        std::snprintf(fStrBuf, 0xff, "%.1f", value);
        return fStrBuf;
    }

    const char *getTextBufTime(const uint64_t frame)
    {
        const uint32_t time = frame / uint64_t(fSampleRate);
        const uint32_t secs = time % 60;
        const uint32_t mins = (time / 60) % 60;
        const uint32_t hrs = (time / 3600) % 60;
        std::snprintf(fStrBuf, 0xff, "%02i:%02i:%02i", hrs, mins, secs);
        return fStrBuf;
    }

    // helpers for drawing text
    void drawLeft(const float x, const float y, const char *const text)
    {
        beginPath();
        fillColor(200, 200, 200);
        textAlign(ALIGN_RIGHT | ALIGN_TOP);
        textBox(x, y, 100 * fScale, text);
        closePath();
    }

    void drawRight(const float x, const float y, const char *const text)
    {
        beginPath();
        fillColor(255, 255, 255);
        textAlign(ALIGN_LEFT | ALIGN_TOP);
        textBox(x + (105 * fScale), y, 100 * fScale, text);
        closePath();
    }

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
