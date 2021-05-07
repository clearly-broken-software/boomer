// Song.cpp
#include "Song.hpp"

START_NAMESPACE_DISTRHO

Song::Song() : song_index(0),
               sampleRate(48000.),
               currentPattern(nullptr),
               accumOffset(0),
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
    const int tt = patterns.back().ml.getTotalTicks();
    songTotalTicks += tt;
    song.push_back(Clip{path, accumOffset});
    accumOffset += tt;
}

void Song::nextPattern()
{
    // get path
    if (song_index >= 0 && song_index < song.size())
    {
        std::string path = song[song_index].path;
        int clipOffset = song[song_index].clipOffset;
        // find pattern
        printf("getting %s\n", path.c_str());
        auto it = std::find_if(patterns.begin(), patterns.end(), [path](Pattern p) { return p.id == path; });
        if (it != patterns.end())
        {
            // set pointer to midifile to be played
            currentPattern = &it->ml;
            currentPattern->setOffsetTicks(clipOffset);
            currentPattern->setSongTotalTicks(songTotalTicks);
            currentPattern->setEventIndex(0);

            // advance index for when this function is called again
            song_index++;
            if (song_index >= patterns.size())
            {
                song_index = 0;
            }
        }
    }
}

END_NAMESPACE_DISTRHO
