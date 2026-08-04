/*
  ==============================================================================

    DeckGUI.cpp
    Created: 24 Feb 2026 3:24:57p
    Author:  ASUS

  ==============================================================================
*/

#include "DeckGUI.h"
#include "../JuceLibraryCode/JuceHeader.h"

DeckGUI::DeckGUI(DJAudioPlayer& _player, juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse) : player(_player), disk(formatManagerToUse, cacheToUse)
{
    addAndMakeVisible(disk);

    // Deck controls
    addAndMakeVisible(play);
    addAndMakeVisible(pause);
    addAndMakeVisible(load);

    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&vol, true);
    addAndMakeVisible(vol);

    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speed, true);
    addAndMakeVisible(speed);

    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.attachToComponent(&pos, true);
    addAndMakeVisible(pos);

    // Adding event listeners
    play.addListener(this);
    pause.addListener(this);
    load.addListener(this);
    vol.addListener(this);
    speed.addListener(this);
    pos.addListener(this);

    // Set volume ranging from 0 to 1
    vol.setRange(0.0, 1.0);

    // Speed ranging from 0.5x to 2.0x
    speed.setRange(0.5, 2.0, 0.01);

    // Position ranging from 0 to 1
    pos.setRange(0.0, 1.0);

    // Default normal volume
    vol.setValue(0.5);

    // Default normal speed
    speed.setValue(1.0);

    // Timer
    startTimer(500);

    // Hotcues
    addAndMakeVisible(editToggle);

    editToggle.onClick = [this]()
        {
            // Update edit mode based on toggle state
            edit = editToggle.getToggleState();
            DBG("Hotcue Edit Mode: " << (edit ? "ON" : "OFF"));
        };

    addAndMakeVisible(clearButton);
    clearButton.onClick = [this]()
        {
            for (int i = 0; i < 8; ++i)
            {
                // Reset data
                hotcues[i].assigned = false;
                hotcues[i].position = 0;

                // Reset UI colour
                hotCueButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);

                // Sync with playlist if a song is loaded
                if (playlistComponent != nullptr)
                {
                    auto it = std::find_if(playlistComponent->tracks.begin(), playlistComponent->tracks.end(), [this](const Track& t) {return t.url == player.getCurrentURL(); });

                    if (it != playlistComponent->tracks.end())
                    {
                        it->hotcues[i] = hotcues[i];
                        playlistComponent->saveHotcuesAndEQToDisk();
                    }
                }
            }

            repaint();
        };

    for (int i = 0; i < 8; ++i)
    {
        auto btn = std::make_unique<juce::TextButton>(juce::String(i + 1));

        btn->setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        int index = i;
        btn->onClick = [this, index]()
            {
                if (cleared)
                {
                    // Clear the hotcue
                    hotcues[index].assigned = false;
                    cleared = false; // exit clearance mode after one click
                }
                else if (!hotcues[index].assigned) // Unassigned
                {
                    // Assign hotcue at current position
                    hotcues[index].position = player.getPositionRelative();
                    hotcues[index].assigned = true;
                    hotCueButtons[index]->setColour(juce::TextButton::buttonColourId, juce::Colours::limegreen);
                }
                else if (edit) // Assigned
                {
                    hotcues[index].position = player.getPositionRelative();
                    hotCueButtons[index]->setColour(juce::TextButton::buttonColourId, juce::Colours::limegreen);
                }
                else
                {
                    // Jump to hotcue position
                    player.setPositionRelative(hotcues[index].position);
                }

                // Update track in the playlist
                if (playlistComponent != nullptr)
                {
                    auto it = std::find_if(playlistComponent->tracks.begin(), playlistComponent->tracks.end(), [this](const Track& t) {return t.url == player.getCurrentURL(); });  
                    
                    if (it != playlistComponent->tracks.end())
                    {
                        it->hotcues[index] = hotcues[index];
                    }
                }
            };

        addAndMakeVisible(*btn);
        hotCueButtons.push_back(std::move(btn));
    }

    // High EQ Knob
    addAndMakeVisible(highEQ);
    highEQ.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highEQ.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highEQ.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::aliceblue);
    highEQ.setColour(juce::Slider::thumbColourId, juce::Colours::violet);

    highLabel.setText("High", juce::dontSendNotification);
    highLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    highLabel.setJustificationType(juce::Justification::centred);
    highLabel.attachToComponent(&highEQ, false);
    addAndMakeVisible(highLabel);


    // Mid EQ Knob
    addAndMakeVisible(midEQ);
    midEQ.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    midEQ.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    midEQ.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::aliceblue);
    midEQ.setColour(juce::Slider::thumbColourId, juce::Colours::magenta);

    midLabel.setText("Mid", juce::dontSendNotification);
    midLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    midLabel.setJustificationType(juce::Justification::centred);
    midLabel.attachToComponent(&midEQ, false);
    addAndMakeVisible(midLabel);

    // Low EQ Knob
    addAndMakeVisible(lowEQ);
    lowEQ.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowEQ.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowEQ.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::aliceblue);
    lowEQ.setColour(juce::Slider::thumbColourId, juce::Colours::deeppink);

    lowLabel.setText("Low", juce::dontSendNotification);
    lowLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    lowLabel.setJustificationType(juce::Justification::centred);
    lowLabel.attachToComponent(&lowEQ, false);
    addAndMakeVisible(lowLabel);

    // Set EQ ranges
    highEQ.setRange(-24.0, 6.0, 0.1);
    highEQ.setValue(0.0);

    midEQ.setRange(-24.0, 6.0, 0.1);
    midEQ.setValue(0.0);

    lowEQ.setRange(-24.0, 6.0, 0.1);
    lowEQ.setValue(0.0);

    // Add listeners for EQ
    highEQ.addListener(this);
    midEQ.addListener(this);
    lowEQ.addListener(this);

}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colours::black.brighter(0.1f), 0, 0,
                                  juce::Colours::black, 0, getHeight(), false);

    g.setGradientFill(gradient);
    g.fillAll();
}

void DeckGUI::resized()
{
    auto area = getLocalBounds();

    // Full width for single deck
    int width = area.getWidth();

    // 1/3 height for the disk
    int height = area.getHeight() / 3;

    // EQ Knobs
    int eqWidth = width * 0.12;
    int eqHeight = height;

    // Vertical spacing between the knobs
    int knobHeight = eqHeight / 3;

    // High EQ
    highEQ.setBounds(5, 0, eqWidth - 10, knobHeight - 10);
    highLabel.setBounds(5, knobHeight - 10, eqWidth - 10, 20);

    // Mid EQ
    midEQ.setBounds(5, knobHeight, eqWidth - 10, knobHeight - 10);
    midLabel.setBounds(5, (knobHeight * 2) - 10, eqWidth - 10, 20);

    // Low EQ
    lowEQ.setBounds(5, knobHeight * 2, eqWidth - 10, knobHeight);
    lowLabel.setBounds(5, (knobHeight * 3) - 10, eqWidth - 10, 20);

    // Single disk uses full width
    disk.setBounds(eqWidth, 0, width - eqWidth, height);

    // Slider and button heights
    int buttonWidth = width / 4;
    int buttonHeight = height / 6;
    int sliderHeight = height / 6;
    int spacing = width / 10;

    // Deck controls
    play.setBounds(spacing, height + height / 10, width / 4, buttonHeight);
    pause.setBounds(spacing + buttonWidth + 10, height + height / 10, width / 4, buttonHeight);
    load.setBounds(spacing + (buttonWidth + 10) * 2, height + height / 10, width / 4, buttonHeight);
    vol.setBounds(width / 6, height + height / 3 + 65, width - width / 5, sliderHeight);
    speed.setBounds(width / 6, height + height / 2 + 65, width - width / 5, sliderHeight);
    // ASSISTANCE TAKEN
    pos.setBounds(width / 6, height + height / 2 + sliderHeight + (height + height / 2 - (height + height / 3 + sliderHeight)) + 65, width - width / 5, sliderHeight);
    // ASSISTANCE ENDED

    for (int i = 0; i < hotCueButtons.size(); ++i)
    {
        hotCueButtons[i]->setBounds(spacing + i * (2 + spacing), height + height / 10 + buttonHeight + 40, 30, 30);
    }

    int centerX = (width - ((buttonWidth * 0.6) * 2)) / 2;
    int centerY = (height + height / 10 + buttonHeight) + (buttonHeight * 0.8) / 2 - (buttonHeight * 0.5) / 2;
    // Hotcue edit toggle
    editToggle.setBounds(centerX, centerY, buttonWidth * 0.6, buttonHeight * 0.5);

    // Hotcue clear button
    clearButton.setBounds(centerX + buttonWidth * 0.6 + 5, centerY, buttonWidth * 0.6, buttonHeight * 0.5);
}

// Handles all button click events in the deck
void DeckGUI::buttonClicked(juce::Button* button)
{
    // Play button logic
    if (button == &play)
    {
        DBG("Play button for the left disk was clicked");
        player.start();
        disk.setPlaying(true);
    }

    // Pause button logic
    if (button == &pause)
    {
        DBG("Pause button for the left disk was clicked");
        player.stop();
        disk.setPlaying(false);
    }

    // Load button logic
    if (button == &load)
    {
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles
                               | juce::FileBrowserComponent::canSelectMultipleItems;
        // Open asynchronous file chooser
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                auto files = chooser.getResults();

                if (files.size() > 0)
                {
                    juce::File firstFile{ files[0] };

                    // Load selected track into the audio player and disk component
                    player.loadURL(juce::URL{ files[0]});
                    disk.loadURL(juce::URL{ files[0] });

                    // Add the files to playlist
                    if (playlistComponent != nullptr)
                    {
                        for (auto& file : files)
                        {
                            playlistComponent->addTrack(file);
                            currentTrackIndex = playlistComponent->tracks.size() - 1;
                        }
                    }

                    // Load hotcues from playlist
                    if (playlistComponent != nullptr)
                    {
                        auto it = std::find_if(playlistComponent->tracks.begin(), playlistComponent->tracks.end(), [&firstFile](const Track& t) {return t.url.getLocalFile() == firstFile; });

                        if (it != playlistComponent->tracks.end())
                        {
                            Track& track = *it;
                            currentTrack = &track;
                            currentTrackIndex = std::distance(playlistComponent->tracks.begin(), it);

                            playlistComponent->tableComponent.selectRow(currentTrackIndex);

                            // Update EQ Sliders
                            lowEQ.setValue(track.lowGain, juce::dontSendNotification);
                            midEQ.setValue(track.midGain, juce::dontSendNotification);
                            highEQ.setValue(track.highGain, juce::dontSendNotification);

                            // Update hot cues button UI
                            for (int i = 0; i < 8; ++i)
                            {
                                hotcues[i] = track.hotcues[i];

                                if (hotcues[i].assigned)
                                {
                                    hotCueButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::limegreen);
                                }
                                else
                                {
                                    hotCueButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
                                }
                            }
                        }
                    }
                }
            });
    }
}

// Responds to slider value changes
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    // Adjust volume
    if (slider == &vol)
    {
        player.setGain(slider->getValue());
    }

    // Adjust speed
    if (slider == &speed)
    {
        player.setSpeed(slider->getValue());
        disk.setRotationSpeed(slider->getValue());
    }

    // Adjust playback position
    if (slider == &pos)
    {
        player.setPositionRelative(slider->getValue());
    }

    // EQ sliders
    if (slider == &lowEQ)
    {
        player.setLowGain(slider->getValue());
        if (currentTrack != nullptr)
        {
            currentTrack->lowGain = slider->getValue();
            playlistComponent->saveHotcuesAndEQToDisk();
        }
    }

    if (slider == &midEQ)
    {
        player.setMidGain(slider->getValue());
        if (currentTrack != nullptr)
        {
            currentTrack->midGain = slider->getValue();
            playlistComponent->saveHotcuesAndEQToDisk();
        }
    }

    if (slider == &highEQ)
    {
        player.setHighGain(slider->getValue());
        if (currentTrack != nullptr)
        {
            currentTrack->highGain = slider->getValue();
            playlistComponent->saveHotcuesAndEQToDisk();
        }
    }
}

// Updates deck visuals in real-time
void DeckGUI::timerCallback()
{
    // Update playhead and BPM
    disk.setPositionRelative(player.getPositionRelative());
    disk.setBPM(player.getCurrentBPM());
}

void DeckGUI::loadAndPlayTrack(Track& track)
{
    // Store track pointer
    currentTrack = &track;

    // Load the track into both players
    player.loadURL(track.url);
    disk.loadURL(track.url);

    // Apply saved per-track EQ
    player.setLowGain(track.lowGain);
    player.setMidGain(track.midGain);
    player.setHighGain(track.highGain);

    // Update GUI EQ sliders
    lowEQ.setValue(track.lowGain, juce::dontSendNotification);
    midEQ.setValue(track.midGain, juce::dontSendNotification);
    highEQ.setValue(track.highGain, juce::dontSendNotification);

    // Start playback
    player.start();
    disk.setPlaying(true);

    // Restore hotcues
    for (int i = 0; i < 8; ++i)
    {
        hotcues[i] = track.hotcues[i];

        if (hotcues[i].assigned)
        {
            hotCueButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::limegreen);
        }
        else
        {
            hotCueButtons[i]->setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }

        repaint();
    }
}