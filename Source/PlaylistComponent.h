/*
  ==============================================================================

    PlaylistComponent.h
    Created: 25 Feb 2026 6:03:24pm
    Author:  ASUS

  ==============================================================================
*/
/*
    PlaylistComponent handles the track storage, display and persistence.
*/
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <array>
#include "DeckGUI.h"
#include "HotCue.h"

//==============================================================================

// Forward declaration
class DeckGUI;

// Structure to hold track data
struct Track 
{
    juce::String title;
    juce::String duration;
    juce::URL url;

    // Store track's hotcues
    std::array<Hotcue, 8> hotcues;

    float lowGain = 0.0f;
    float midGain = 0.0f;
    float highGain = 0.0f;
};

// Custom JUCE TextButtons styled with orange borders and rounded corners for playlist buttons (PLAY & REMOVE)
class OrangeButton : public juce::TextButton
{
public:
    using juce::TextButton::TextButton;

    void paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = getLocalBounds().toFloat();
        float cornerSize = 6.0f; // rectangle radius

        g.setColour(findColour(juce::TextButton::buttonColourId));
        g.fillRoundedRectangle(bounds, cornerSize);

        g.setColour(juce::Colours::orange);
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

        g.setColour(findColour(juce::TextButton::textColourOffId));
        g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
    }
};

class PlaylistComponent  : public juce::Component,
                           public juce::TableListBoxModel
{
public:
    // Table displaying track list
    juce::TableListBox tableComponent;

    PlaylistComponent();
    ~PlaylistComponent() override;

    // Pointers to the left and right deck GUIs for loading tracks
    DeckGUI* leftDeck = nullptr;
    DeckGUI* rightDeck = nullptr;

    void paint (juce::Graphics&) override;
    void resized() override;

    // To manage rows and cells
    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

    // Adds a track from file to the playlist and updates table display
    void addTrack(const juce::File& file);

    // Saves and loads playlist to and from the disk for persistence
    void savePlaylistToDisk();
    void loadPlaylistFromDisk();

    // Saves and loads hot cues and EQ settings to and from the disk for persistence
    void saveHotcuesAndEQToDisk();
    void loadHotcuesAndEQFromDisk();

    // Vector holding all tracks in the playlist
    std::vector<Track> tracks;

private:
    // Row index currently playing on the left and right decks
    int leftPlayingRow = -1;
    int rightPlayingRow = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
