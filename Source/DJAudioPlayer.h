/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 20 Feb 2026 8:56:09pm
    Author:  ASUS

  ==============================================================================
*/
/*
    DJAudioPlayer handles audio playback, speed control, EQ and BPM detection for a single deck.
*/
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public juce::AudioSource {
public:
    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    // Prepares to play the audio source for playback
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    // Fills the buffer with the next block of audio samples
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    // Releases audio resources when playback stops or when the app is closed
    void releaseResources() override;

    // Loads an audio file from the JUCE URL
    void loadURL(juce::URL audioURL);

    // Sets the overall playback volume
    void setGain(double gain);
    // Sets frequency gains for the three-band EQs
    void setLowGain(float gainDB);
    void setMidGain(float gainDB);
    void setHighGain(float gainDB);
    // Sets playback speed 
    void setSpeed(double ratio);
    // Sets absolute playback position
    void setPosition(double posInSecs);
    // Sets relative playback position
    void setPositionRelative(double pos);

    // Starts and stops the playback of a loaded track
    void start();
    void stop();

    // Gets the relative position of the playhead
    double getPositionRelative();

    // Returns the current track's URL
    juce::URL getCurrentURL();

    // Returns the BPM estimate for the loaded track
    double getCurrentBPM() const { return currentBPM; };

private:
    // For loading audio files
    juce::AudioFormatManager& formatManager;
    // Reader source for currently loaded audio file
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    // Transport source for audio playback
    juce::AudioTransportSource transportSource;
    // Resampling source for speed control
    juce::ResamplingAudioSource resampleSource{ &transportSource, false };
    // URL of the currently loaded track
    juce::URL currentURL;

    // DSP Processor Chain from EQs
    juce::dsp::ProcessorChain < juce::dsp::IIR::Filter<float>,
                                juce::dsp::IIR::Filter<float>,
                                juce::dsp::IIR::Filter<float>
                              >eqChain;

    // Gain values for each EQ band
    float lowGain = 0.0f;
    float midGain = 0.0f;
    float highGain = 0.0f;

    // Sets BPM
    void setBPM(const juce::AudioSourceChannelInfo& bufferToFill);

    // Sample rate currently used for processing
    double currentSampleRate = 44100.0;

    // For BPM Calculations
    std::atomic<double> currentBPM{ 0.0 };
    std::atomic<double>lastBeatTime{ 0.0 };
    double envelope = 0.0;
    float threshold = 0.02f;
    bool isPeak = false;
};