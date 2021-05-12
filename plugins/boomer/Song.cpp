// Song.cpp
#include "Song.hpp"

START_NAMESPACE_DISTRHO

Song::Song() : clip_index(0),
               sampleRate(48000.),
               currentPattern(nullptr),
               accumClipOffset(0),
               songTPQ(1920)
{
}

void Song::setCallback(MidiLooper::Callback *cb)
{
    callBack = cb;
}

void Song::addPattern(std::string path)
{
    printf("adding %s\n", path.c_str());
    patterns.push_back(Pattern{path, MidiLooper(smf::MidiFile(path),
                                                sampleRate,
                                                callBack)});
    patterns.back().ml.setTPQ(songTPQ);
    patterns.back().ml.setTotalTicks();
    patterns.back().ml.setEventIndex(0);
    std::cout << &patterns.back().ml << std::endl;
    const int tt = patterns.back().ml.getTotalTicks();
    songTotalTicks += tt;
    song.push_back(Clip{path, accumClipOffset});
    accumClipOffset += tt;
}

void Song::nextPattern()
{
    // get path

    if (clip_index >= 0 && clip_index < song.size())
    {
        std::string path = song[clip_index].path;
        int clipOffset = song[clip_index].clipOffset;
        // find pattern
        printf("getting %s\n", path.c_str());
        auto it = std::find_if(patterns.begin(), patterns.end(), [path](Pattern p)
                               { return p.id == path; });
        if (it != patterns.end())
        {
            // set pointer to midifile to be played
            currentPattern = &it->ml;
            currentPattern->setOffsetTicks(clipOffset);
            currentPattern->setSongTotalTicks(songTotalTicks);
            currentPattern->setEventIndex(0);

            // advance index for when this function is called again
            clip_index++;
            if (clip_index >= patterns.size())
            {
                clip_index = 0;
            }
        }
    }
}

MidiLooper *Song::getPatternPtr(size_t index)
{
    if (clip_index >= 0 && clip_index < song.size())
    {
        std::string path = song[clip_index].path;
        auto it = std::find_if(patterns.begin(), patterns.end(), [path](Pattern p)
                               { return p.id == path; });
        if (it != patterns.end())
        {
            return &it->ml;
        }
    }
    return nullptr; 
}

END_NAMESPACE_DISTRHO
