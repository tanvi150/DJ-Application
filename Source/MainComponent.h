#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

//==============================================================================
/*
    MainComponent acts as a controller. It manages two decks, a playlist and a mixer to allow
    simultaneous playback and interaction between tracks.
*/
class MainComponent : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================

    // Prepares audio players and mixer to start playback.
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    // Fills the audio buffer with mixed audio from both decks.
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Releases audio resources when playback stops or the app closes.
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    // Manages audio formats
    juce::AudioFormatManager formatManager;

    // Caches audio thumbnails for waveform displays
    juce::AudioThumbnailCache thumbCache{100};

    // Left deck audio player
    DJAudioPlayer leftPlayer{formatManager};
    // Right deck audio player
    DJAudioPlayer rightPlayer {formatManager};
    // Playlist manager for tracks, hot cues and EQ settings
    PlaylistComponent playlistComponent;

    // UI for left and right decks
    DeckGUI leftDeck {leftPlayer, formatManager, thumbCache};
    DeckGUI rightDeck {rightPlayer, formatManager, thumbCache };

    // Mixes audio from both decks for output
    juce::MixerAudioSource mixerSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
