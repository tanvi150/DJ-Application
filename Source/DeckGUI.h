/*
  ==============================================================================

    DeckGUI.h
    Created: 24 Feb 2026 3:24:57pm
    Author:  ASUS

  ==============================================================================
*/
/*
    DeckGUI handles the user interface for a single DJ deck.
*/
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DiskComponent.h"
#include "PlaylistComponent.h"
#include "HotCue.h"
#include <vector>
#include <array>

// Forward declaration of Track Structure
struct Track;

class PlaylistComponent;

class DeckGUI : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::Timer
{
public:
    DeckGUI(DJAudioPlayer& _player, juce::AudioFormatManager & formatManagerToUse, juce::AudioThumbnailCache & cacheToUse);
    ~DeckGUI();

    // Pointer to the playlist component
    PlaylistComponent* playlistComponent = nullptr;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Implement button and slider listeners
    void buttonClicked(juce::Button*) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void setDeckColour(juce::Colour colour) { disk.setWaveformColour(colour); }

    // For updating UI elements (e.g. disk rotation, BPM updation)
    void timerCallback() override;

    // Loads a track and starts playback
    void loadAndPlayTrack(Track& track);
    Track* currentTrack = nullptr;

private:
    // Static disk
    DiskComponent disk;

    juce::FileChooser fChooser{ "Select a file..." };

    // Controls for the disk
    juce::TextButton play{ "PLAY" };
    juce::TextButton pause{ "PAUSE" };
    juce::TextButton load{ "LOAD" };
    juce::Slider vol;
    juce::Slider speed;
    juce::Slider pos;

    // Labels for the deck
    juce::Label volLabel, speedLabel, posLabel;

    // Reference to DJAudioPlayer controlling the deck
    DJAudioPlayer& player;

    // Buttons to assign hot cues
    std::vector<std::unique_ptr<juce::TextButton>> hotCueButtons;
    std::array<Hotcue, 8> hotcues;

    // Toggling to indicate edit mode for hot cues
    bool edit = false;
    juce::ToggleButton editToggle{ "Edit Hotcues" };

    // Clear button for hot cues
    juce::TextButton clearButton{ "Clear Hotcues" };
    bool cleared = false;

    // Index of the current track in a playlist
    int currentTrackIndex = -1;

    // EQ Knobs
    juce::Slider highEQ;
    juce::Slider midEQ;
    juce::Slider lowEQ;

    juce::Label highLabel, midLabel, lowLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};