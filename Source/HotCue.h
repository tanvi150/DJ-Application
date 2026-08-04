/*
  ==============================================================================

    HotCue.h
    Created: 28 Feb 2026 11:09:15am
    Author:  ASUS

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// A hot cue stores a specific playback position within a track and whether it has been assigned by the user. 
struct Hotcue
{
    // Playback position in seconds
    double position = 0.0;

    // Boolean to check if the hot cue has been set by the user
    bool assigned = false;
};