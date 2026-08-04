    /*
      ==============================================================================

        DJAudioPlayer.cpp
        Created: 20 Feb 2026 8:56:09pm
        Author:  ASUS

      ==============================================================================
    */

    #include "DJAudioPlayer.h"
    
    // Initialises the audio format manager reference for reading audio files
    DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager) : formatManager(_formatManager)
    {

    };

    DJAudioPlayer::~DJAudioPlayer()
    {

    };

    // Prepares the audio sources for playback
    void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
    {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
        resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

        // Store sample rate for EQ coefficient updates
        currentSampleRate = sampleRate;

        // DSP Specification for ProcessorChain
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlockExpected;
        spec.numChannels = 2;

        eqChain.prepare(spec);

        // Set default gains
        setLowGain(0.0f);
        setMidGain(0.0f);
        setHighGain(0.0f);
    };

    // Fills the next audio block for playback
    void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
    {
        resampleSource.getNextAudioBlock(bufferToFill);

        // Apply EQ using DSP Processor chain
        juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        eqChain.process(context);
        setBPM(bufferToFill);
    };

    void DJAudioPlayer::releaseResources()
    {
        transportSource.releaseResources();
        resampleSource.releaseResources();
    };

    // Loads an audio file from the URL
    void DJAudioPlayer::loadURL(juce::URL audioURL)
    {
        transportSource.setSource(nullptr);

        auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

        if (reader != nullptr)
        {
            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            currentURL = audioURL;
        }
    };

    // Sets playback gain
    void DJAudioPlayer::setGain(double gain)
    {
        if (gain < 0 || gain > 1.0)
        {
            std::cout << "DJAudioPlayer::setGain should be between 0 and 1" << std::endl;
        }
        else
        {
            transportSource.setGain(gain);
        }
    }

    // Sets playback speed using ratio (0.5x - 2.0x)
    void DJAudioPlayer::setSpeed(double ratio)
    {
        if (ratio < 0 || ratio > 2.0)
        {
            std::cout << "DJAudioPlayer::setSpeed should be between 0.5 and 2" << std::endl;
        }
        else
        {
            resampleSource.setResamplingRatio(ratio);
        }
    };

    // Sets playback position in seconds
    void DJAudioPlayer::setPosition(double posInSecs)
    {
        transportSource.setPosition(posInSecs);
    };

    // Sets relative playback position
    void DJAudioPlayer::setPositionRelative(double pos)
    {
        if (pos < 0 || pos > 1.0)
        {
            std::cout << "DJAudioPlayer::setPositionRelative should be between 0 and 1" << std::endl;
        }
        else
        {
            double posInSecs = transportSource.getLengthInSeconds() * pos;
            setPosition(posInSecs);
        }
    }

    // Sets low EQ gain in decibels
    void DJAudioPlayer::setLowGain(float gainDB)
    {
        lowGain = gainDB;

        auto coeff = juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, 400.0f, 1.5f, juce::Decibels::decibelsToGain(gainDB));
        *eqChain.get<0>().coefficients = *coeff;
    }

    // Sets mid EQ gain in decibels
    void DJAudioPlayer::setMidGain(float gainDB)
    {
        midGain = gainDB;

        auto coeff = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 1000.0f, 1.2f, juce::Decibels::decibelsToGain(gainDB));
        *eqChain.get<1>().coefficients = *coeff;
    }

    // Sets high EQ gain in decibels 
    void DJAudioPlayer::setHighGain(float gainDB)
    {
        highGain = gainDB;

        auto coeff = juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, 3000.0f, 1.5f, juce::Decibels::decibelsToGain(gainDB));
        *eqChain.get<2>().coefficients = *coeff;
    }

    // Start playback
    void DJAudioPlayer::start()
    {
        transportSource.start();
    }

    // Stop playback
    void DJAudioPlayer::stop()
    {
        transportSource.stop();
    }

    // Get current relative playback position
    double DJAudioPlayer::getPositionRelative()
    {
        return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
    }

    // Returns the current loaded audio URL
    juce::URL DJAudioPlayer::getCurrentURL()
    {
        return currentURL;
    }

    void DJAudioPlayer::setBPM(const juce::AudioSourceChannelInfo& bufferToFill)
    {
        // Simple peak detection for BPM
        juce::AudioBuffer<float>* buffer = bufferToFill.buffer;
        const int numSamples = bufferToFill.numSamples;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get absolute amplitude
            float input = std::abs(buffer->getSample(0, sample));

            // Envelope follower with decay
            envelope = (input > envelope) ? input : envelope * 0.999f;

            // Peak detection: if envelope crosses a threshold, it is a beat
            if (envelope > threshold && !isPeak)
            {
                auto currentTime = juce::Time::getMillisecondCounterHiRes();
                auto timeSinceLastBeat = currentTime - lastBeatTime;

                // Calculate BPM: 60000 ms / time between beats
                if (timeSinceLastBeat > 250.0)
                {
                    currentBPM = 60000 / timeSinceLastBeat;
                    lastBeatTime = currentTime;
                }
                isPeak = true;
            }
            else if (envelope < threshold * 0.8f)
            {
                isPeak = false;
            }
        }
    }