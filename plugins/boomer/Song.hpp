#ifndef SONG_HPP_DEFINED
#define SONG_HPP_DEFINED

#include "MidiLooper.hpp"
#include "DistrhoPlugin.hpp"
#include "../../dpf/distrho/src/DistrhoDefines.h"
#include <unordered_map>

START_NAMESPACE_DISTRHO

class Song
{
public:
    Song();
    void setSamplerate(double sr) { sampleRate = sr; };
    void addPattern(std::string);
    void nextPattern();
    void setCallback(MidiLooper::Callback *cb);
    // void stop();
    // void seek();
    // void rewind(); // seek(0)

    // current midi patern to play
    MidiLooper *currentPattern;

private:
    double sampleRate;
    int songTPQ;

    struct Pattern
    {
        std::string id; // use path for id
        MidiLooper ml;
    };
    std::vector<Pattern> patterns;

    struct Clip
    {
        std::string path;
        int clipOffset; // in ticks
    };
    std::vector<Clip> song;
    size_t song_index;
    int accumOffset;
    int songTotalTicks;

    MidiLooper::Callback *callBack;

    //std::unordered_map<ghc::filesystem::path, MidiLooper*> patterns;
};

END_NAMESPACE_DISTRHO

#endif