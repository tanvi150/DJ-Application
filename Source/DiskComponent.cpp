/*
  ==============================================================================

    DiskComponent.cpp
    Created: 16 Feb 2026 1:44:12pmm
    Author:  ASUS

  ==============================================================================
*/

#include "DiskComponent.h"

// Initialises audio thumbnail, sets initial state variables, adds a ChangeListener to the thumbnail and starts a 60Hz timer
DiskComponent::DiskComponent(juce::AudioFormatManager& formatManagerToUse, juce::AudioThumbnailCache& cacheToUse) : audioThumb(1000, formatManagerToUse, cacheToUse), fileLoaded(false), position(0)
{
    // Listen for thumbnail updates
    audioThumb.addChangeListener(this);
    // 60Hz timer for disk animation
    startTimerHz(60);
}

DiskComponent::~DiskComponent() 
{
    audioThumb.removeChangeListener(this);
}

void DiskComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}

void DiskComponent::setWaveformColour(juce::Colour newColour)
{
    waveformColour = newColour;
    repaint();
}

void DiskComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    auto bounds = getLocalBounds();

    // Layout

    // Waveform Area
    auto waveformArea = bounds.removeFromTop(30); // waveform height
    bounds.removeFromTop(5); //spacing

    // Disk Area
    auto diskArea = bounds.reduced(5).toFloat(); // padding around the disk
    auto center = diskArea.getCentre();
    float radius = juce::jmin(diskArea.getWidth(), diskArea.getHeight()) / 2.0f - 10.0f;

    // Draw Disk
    
    // Rotation Start
    g.saveState();
    g.addTransform(juce::AffineTransform::rotation(rotationAngle, center.x, center.y));
    
    // Draw vinyl
    g.setColour(juce::Colours::darkgrey);
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);

    // Vinyl marker
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.fillRect(center.x - 2, center.y - radius, 4.0f, 20.0f);
    
    // Center label
    float labelRadius = radius * 0.25;

    g.setColour(juce::Colours::red.darker(0.3f));
    g.fillEllipse(center.x - labelRadius, center.y - labelRadius, labelRadius * 2.0f, labelRadius * 2.0f);

    // Rotation End
    g.restoreState();

    // Draw the duration text
    if (fileLoaded && audioThumb.getTotalLength() > 0.0)
    {
        double totalSeconds = audioThumb.getTotalLength();
        double remainingSeconds = totalSeconds - (totalSeconds * position);
        
        remainingSeconds = juce::jmax(0.0, remainingSeconds);

        int minutes = static_cast<int>(remainingSeconds / 60);
        int seconds = static_cast<int>(remainingSeconds) % 60;

        juce::String durationText = juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0', 2);

        g.setColour(juce::Colours::white);
        g.setFont(labelRadius * 0.6f);

        juce::Rectangle<float> labelArea(center.x - labelRadius, center.y - labelRadius, labelRadius * 2.0f, labelRadius * 2.0f);
        g.drawFittedText(durationText, labelArea.toNearestInt(), juce::Justification::centred, 1);
    }
    
    // Outline
    g.setColour(juce::Colours::black);
    g.drawEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2, 2.0f);

    // Draw Waveform
    if (fileLoaded)
    {
        g.setColour(waveformColour.withAlpha(0.7f));
        audioThumb.drawChannel(g, waveformArea, 0, audioThumb.getTotalLength(), 0, 1.0f);

        // Add a dynamic playhead
        float playhead = waveformArea.getX() + (waveformArea.getWidth() * position);

        g.setColour(juce::Colours::white);
        g.drawVerticalLine(playhead, (float)waveformArea.getY(), (float)waveformArea.getBottom());

        g.fillEllipse(playhead - 3, (float)waveformArea.getY() - 3, 6.0f, 6.0f);
    }

    // Display BPM
    if (fileLoaded)
    {
        g.setColour(juce::Colours::cornflowerblue);
        g.setFont(14.0f);
        juce::String bpmText = "BPM: " + juce::String((int)currentBPM);

        // Position
        auto bpmX = bounds.getRight() - 80;
        auto bpmY = bounds.getBottom() - 20;

        g.drawText(bpmText, bpmX, bpmY, 70, 20, juce::Justification::centredRight);
    }
}

void DiskComponent::resized() {}

// Load a new audio file into the waveform, clear previous thumbnail and resets position
void DiskComponent::loadURL(juce::URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));

    if (fileLoaded)
    {
        // Reset rotation for new track
        rotationAngle = 0.0; 
    }

    repaint();
}

// Updates the position of a track and repaints the waveform 
void DiskComponent::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }

}

// Start or stop disk rotation
void DiskComponent::setPlaying(bool shouldPlay)
{
    isPlaying = shouldPlay;
}

// Rotates the disk continuously when playing and stops rotation when the track ends
void DiskComponent::timerCallback()
{
    if (fileLoaded && isPlaying)
    {
        // Stop rotation if the track has finished playing
        if (position >= 1.0)
        {
            rotationAngle = 0.0;
            isPlaying = false;
            return;
        }

        rotationAngle += (0.1 * currentRotationSpeed);

        if (rotationAngle > juce::MathConstants<double>::twoPi)
        {
            rotationAngle -= juce::MathConstants<double>::twoPi;
        }

        repaint();
    }
}

// Set rotation speed of the disk
void DiskComponent::setRotationSpeed(double speed)
{
    currentRotationSpeed = speed;
}