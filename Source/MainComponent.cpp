#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after you add any child components
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Open audio channels: 0 inputs, 2 outputs
        setAudioChannels (0, 2);
    }

    // Add decks and playlist to the GUI
    addAndMakeVisible(leftDeck);
    addAndMakeVisible(rightDeck);
    addAndMakeVisible(playlistComponent);

    // Link playlist component with decks
    playlistComponent.leftDeck = &leftDeck;
    playlistComponent.rightDeck = &rightDeck;

    leftDeck.playlistComponent = &playlistComponent;
    rightDeck.playlistComponent = &playlistComponent;

    // Set deck waveform colours
    leftDeck.setDeckColour(juce::Colours::yellow);
    rightDeck.setDeckColour(juce::Colours::cyan);

    // Register audio formats
    formatManager.registerBasicFormats();

    // Load saved playlist, hot cues and EQs from the disk
    playlistComponent.loadPlaylistFromDisk();
    playlistComponent.loadHotcuesAndEQFromDisk();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source
    shutdownAudio();

    // Saves playlist, hot cues and EQ to disk for persistence
    playlistComponent.savePlaylistToDisk();
    playlistComponent.saveHotcuesAndEQToDisk();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Prepare each DJ deck
    leftPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    rightPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    // Prepare mixer and add decks as input sources
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&leftPlayer, false);
    mixerSource.addInputSource(&rightPlayer, false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Delegate to mixer, which combines left and right decks
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being restarted due to a setting change

    leftPlayer.releaseResources();
    rightPlayer.releaseResources(); 
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    // Gap between decks
    int gap = 1;
    int deckWidth = (area.getWidth() - gap) / 2;

    // Position left and right decks side by side
    leftDeck.setBounds(0, 0, deckWidth, getHeight());
    rightDeck.setBounds(deckWidth + gap, 0, deckWidth, getHeight());

    // Position playlist at bottom 28% of the window
    playlistComponent.setBounds(0, getHeight() * 0.72, getWidth(), getHeight() * 0.28);
}