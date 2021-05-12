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
    int getClipIndex() { return clip_index; };
    size_t getSongSize() { return song.size(); };
    MidiLooper* getPatternPtr(size_t index);
    void addPattern(std::string);
    void nextPattern();
    void setCallback(MidiLooper::Callback *cb);
    // void stop();
    // void seek();
    // void rewind(); // seek(0)

    // current midi pattern to play
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
    size_t clip_index;
    int accumClipOffset;
    int songTotalTicks;

    MidiLooper::Callback *callBack;

    //std::unordered_map<ghc::filesystem::path, MidiLooper*> patterns;
};

END_NAMESPACE_DISTRHO

#endif