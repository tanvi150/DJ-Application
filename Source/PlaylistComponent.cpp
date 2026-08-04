/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 25 Feb 2026 6:03:24pm
    Author:  ASUS

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent()
{
    // Add columns to the playlist table, display column title, index and cell width
    tableComponent.getHeader().addColumn("Track Title", 1, 640);
    tableComponent.getHeader().addColumn("Duration", 2, 100);
    tableComponent.getHeader().addColumn("Left Deck", 3, 180);
    tableComponent.getHeader().addColumn("Right Deck", 4, 180);
    tableComponent.getHeader().addColumn("Remove", 5, 180);

    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    // Create a subtle black gradient
    juce::ColourGradient gradient(
        juce::Colours::black, 0.0f, 0.0f,
        juce::Colours::black.brighter(0.1f), 0.0f, (float)getHeight(), false
    );
        
    g.setGradientFill(gradient);
    g.fillAll();
}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child components that your component contains
    tableComponent.setBounds(0, 0, getWidth(), getHeight());

    // Set consistent row height
    tableComponent.setRowHeight(20);
}

int PlaylistComponent::getNumRows()
{
    // Returns number of tracks in a playlist
    return tracks.size();
}

// Paints the background of a row in a table
void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        // Highlight orange for a selected row
        g.fillAll(juce::Colours::orange);
    }
    else if (rowNumber == leftPlayingRow)
    {
        // Highlight yellow for the row playing at the left deck
        g.fillAll(juce::Colours::gold.withAlpha(0.5f));
    }
    else if (rowNumber == rightPlayingRow)
    {
        // Highlight cyan for the row playing at the right deck
        g.fillAll(juce::Colours::cyan.withAlpha(0.5f));
    }
    else
    {
        // Default row colour
        g.fillAll(juce::Colours::black);
    }

    // Draw horizontal grids at the bottom of row
    g.setColour(juce::Colours::lightgrey);
    g.drawLine(0.0f, (float)height - 1.0f, (float)width, float(height) - 1.0f, 1.0f);
}

// Paints the content of a cell
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= (int)tracks.size())
    {
        return;
    }

    juce::String text;

    switch (columnId)
    {
    case 1: text = tracks[rowNumber].title; break;
    case 2: text = tracks[rowNumber].duration; break;
    default: text = ""; break;

    }

    if (rowIsSelected)
    {
        g.setColour(juce::Colours::black);
    }
    else
    {
        g.setColour(juce::Colours::white);
    }
    
    g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);

    // Draw vertical grids
    g.setColour(juce::Colours::lightgrey);
    g.drawLine((float)width - 1.0f, 0.0f, float(width) - 1.0f, float(height), 1.0f);
}

// Creates or updates buttons in the table (PLAY / REMOVE)
juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    // Left deck PLAY button
    if (columnId == 3)
    {
        auto* btn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);

        if (btn == nullptr)
        {
            btn = new OrangeButton{ "PLAY" };
            existingComponentToUpdate = btn;
        }

        // Button Appearance
        if (rowNumber == leftPlayingRow)
        {
            // Set button colour if clicked
            btn->setColour(juce::TextButton::buttonColourId, juce::Colours::gold);
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        }
        else
        {
            btn->setColour(juce::TextButton::buttonColourId, juce::Colours::black);
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        }

        // On click, load and play the track
        btn->onClick = [this, btn, rowNumber]()
            {
                auto track = tracks[rowNumber];
                if (leftDeck != nullptr)
                {
                    leftDeck->loadAndPlayTrack(tracks[rowNumber]);
                    leftPlayingRow = rowNumber;

                    tableComponent.selectRow(rowNumber);

                    tableComponent.updateContent();
                }
            };
    }

    // Right deck PLAY button
    if (columnId == 4)
    {
        auto* btn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);

        if (btn == nullptr)
        {
            btn = new OrangeButton{ "PLAY" };
            existingComponentToUpdate = btn;
        }

        // Button Appearance
        if (rowNumber == rightPlayingRow)
        {
            // Set button colour if clicked
            btn->setColour(juce::TextButton::buttonColourId, juce::Colours::cyan);
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        }
        else
        {
            btn->setColour(juce::TextButton::buttonColourId, juce::Colours::black);
            btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        }

        // On click, load and play the track
        btn->onClick = [this, btn, rowNumber]()
            {
                auto track = tracks[rowNumber];
                if (rightDeck != nullptr)
                {
                    rightDeck->loadAndPlayTrack(tracks[rowNumber]);
                    rightPlayingRow = rowNumber;

                    tableComponent.selectRow(rowNumber);

                    tableComponent.updateContent();
                }
            };
    }

    // Remove button
    if (columnId == 5)
    {
        auto* btn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);

        if (btn == nullptr)
        {
            btn = new OrangeButton{ "REMOVE" };
            existingComponentToUpdate = btn;
        }

        // On click, remove the track and update the table
        btn->onClick = [this, rowNumber]()
            {
                if (rowNumber < tracks.size())
                {
                    tracks.erase(tracks.begin() + rowNumber);
                    tableComponent.updateContent();
                }
            };
    }

    return existingComponentToUpdate;

    return existingComponentToUpdate;
}

// Adds a track to the playlist and calculates its duration
void PlaylistComponent::addTrack(const juce::File& file)
{
    if (file.existsAsFile())
    {
        Track newTrack;
        newTrack.title = file.getFileNameWithoutExtension();
        newTrack.url = juce::URL(file);

        // Get duration from AudioFormatReader
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        if (auto* reader = formatManager.createReaderFor(file))
        {
            double lengthInSeconds = reader->lengthInSamples / reader->sampleRate;
            int minutes = static_cast<int>(lengthInSeconds / 60);
            int seconds = static_cast<int>(lengthInSeconds) % 60;
            newTrack.duration = juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0', 2);
            delete reader;
        }

        tracks.push_back(newTrack);
        tableComponent.updateContent();

        savePlaylistToDisk();
    }
}

// Saves the playlist to a text file on the disk
void PlaylistComponent::savePlaylistToDisk()
{
    // Get a file location inside the user's application data directory
    // The playlist is stored as 'playlist.txt'
    juce::File saveFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("playlist.txt");

    // Create a StringArray to hold each track path as a separate line
    juce::StringArray lines;

    // Loop through all tracks currently in the playlist
    for (auto& track : tracks)
    {
        // Convert the track's URL to a local file and get its path
        // Add the file path as a new line in the array
        lines.add(track.url.getLocalFile().getFullPathName());
    }

    // Write all file paths directly to the disk, separated by newline characters
    // If the file already exists, it will be replaced
    saveFile.replaceWithText(lines.joinIntoString("\n"));
}

// Loads the playlist from the disk and restores tracks into the playlist
void PlaylistComponent::loadPlaylistFromDisk()
{
    // Get the location of the saved playlist file
    juce::File saveFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("playlist.txt");

    // Only attempt to load if the file actually exists
    if (saveFile.existsAsFile())
    {
        // Prevent duplicates
        tracks.clear();

        // Create a StringArray to hold each track path as a separate line
        juce::StringArray lines;

        // Read all lines from the file into the array
        saveFile.readLines(lines);

        // Loop through each saved file path
        for (auto& line : lines)
        {
            juce::File file(line);

            // Check that the file still exists before adding it
            if (file.existsAsFile())
            {
                // Re-add the track to the playlist
                addTrack(file);
            }
        }
    }
}

void PlaylistComponent::saveHotcuesAndEQToDisk()
{
    // File location for hotcue and eq data
    juce::File hotcueFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("hotcuesAndEQ.txt");

    juce::StringArray lines;

    for (auto& track : tracks)
    {
        juce::StringArray hcStrings;
        for (int i = 0; i < 8; ++i)
        {
            hcStrings.add(juce::String(track.hotcues[i].assigned ? "1" : "0") + "," + juce::String(track.hotcues[i].position, 6));
        }

        // Combine the file path, hotcue and EQs info
        juce::String line = track.url.getLocalFile().getFullPathName() 
                            + "|" + hcStrings.joinIntoString(";")
                            + "|" + juce::String(track.lowGain, 2) 
                            + "|" + juce::String(track.midGain, 2)
                            + "|" + juce::String(track.highGain, 2);

        // Add the full line to the array
        lines.add(line);
    }

    // Save all hotcue and eq data to disk
    hotcueFile.replaceWithText(lines.joinIntoString("\n"));
}

void PlaylistComponent::loadHotcuesAndEQFromDisk()
{
    // Get the location of the saved hotcues and eqs file
    juce::File saveFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("hotcuesAndEQ.txt");

    if (!saveFile.existsAsFile())
    {
        return;
    }

    juce::StringArray lines;
    saveFile.readLines(lines);

    int numTracks = juce::jmin((int)lines.size(), (int)tracks.size());

    for (auto& line : lines)
    {
        // Split file path from hotcue string
        int sepIndex = line.indexOfChar('|');
        if (sepIndex < 0)
        {
            continue;
        }

        juce::String filePath = line.substring(0, sepIndex);
        juce::String hcAndEQData = line.substring(sepIndex + 1);

        // Split hotcue & EQ
        int eqSepIndex = hcAndEQData.indexOfChar('|');
        juce::String hcData = (eqSepIndex >= 0) ? hcAndEQData.substring(0, eqSepIndex) : hcAndEQData;
        juce::String eqData = (eqSepIndex >= 0) ? hcAndEQData.substring(eqSepIndex + 1) : "";

        // Find the track with the file path
        auto it = std::find_if(tracks.begin(), tracks.end(), [&filePath](const Track& t) 
            {
                return t.url.getLocalFile().getFullPathName() == filePath; 
            });    

        if (it != tracks.end())
        {
            Track& track = *it;

            juce::StringArray hcPairs;
            hcPairs.addTokens(hcData, ";", "");

            for (int i = 0; i < 8 && i < hcPairs.size(); ++i)
            {
                juce::StringArray parts;
                parts.addTokens(hcPairs[i], ",", "");

                if (parts.size() == 2)
                {
                    track.hotcues[i].assigned = (parts[0] == "1");
                    track.hotcues[i].position = parts[1].getDoubleValue();
                }
            }

            // Load EQ if available
            if (eqData.isNotEmpty())
            {
                juce::StringArray eqParts;
                eqParts.addTokens(eqData, "|", "");

                if (eqParts.size() >= 3)
                {
                    track.lowGain = eqParts[0].getFloatValue();
                    track.midGain = eqParts[1].getFloatValue();
                    track.highGain = eqParts[2].getFloatValue();
                }
            }
        }
    }
}