/*
  ==============================================================================

    DiskComponent.h
    Created: 16 Feb 2026 1:44:12pm
    Author:  ASUS

  ==============================================================================
*/
/*
    DiskComponent displays track waveform and a rotating vinyl disk animation for the deck.
*/
#pragma once
#include <JuceHeader.h>

class DiskComponent : public juce::Component,
                      public juce::ChangeListener,
                      public juce::Timer
{
    public: 
    DiskComponent(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse);
    ~DiskComponent() override;

    // Receives change event callbacks
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    void paint(juce::Graphics&) override;
    void resized() override;

    // Loads an audio track from URL and generates waveform
    void loadURL(juce::URL audioURL);

    void setWaveformColour(juce::Colour newColour);

    // To animate the disk
    void timerCallback() override;
    // Sets the rotation speed of the disk
    void setRotationSpeed(double speed);

    // Sets whether a track is currently playing
    void setPlaying(bool shouldPlay);

    // Sets the relative position of the playhead
    void setPositionRelative(double pos);

    // Updates the displayed BPM of the track
    void setBPM(double bpm) { currentBPM = bpm; repaint(); };

    private:
        // For drawing waveform
        juce::AudioThumbnail audioThumb;
        // Boolean to check if a track is loaded
        bool fileLoaded;
        // Relative playhead position
        double position;

        juce::Colour waveformColour{ juce::Colours::cyan };

        // Current rotation angle of the vinyl disk
        double rotationAngle = 0.0;
        // Current speed of rotation of the vinyl disk 
        double currentRotationSpeed = 1.0;

        // Check if the track is currently playing
        bool isPlaying = false;

        // Current BPM for a loaded track
        double currentBPM = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiskComponent)
};