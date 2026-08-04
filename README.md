# 🎧 JUCE DJ Application

A digital DJ mixing application developed using **C++ and the JUCE framework** through **Projucer**. The application recreates essential DJ software functionality, allowing users to load, mix, and manipulate audio tracks through dual playback decks.

The project focuses on real-time audio processing, user interaction, and persistent data management. It includes features such as playlist management, hot cues, three-band equalisation, BPM detection, and DJ-style visual feedback.

---

# ✨ Features

## 🎵 Dual Deck Audio Playback

The application provides two independent DJ decks, allowing users to load and mix multiple tracks simultaneously.

### Supported functionality:

* Load local audio files
* Play two tracks at the same time
* Control each deck independently
* Adjust playback position
* Monitor playback progress

Audio processing is handled using JUCE audio classes including:

* `AudioFormatManager`
* `AudioFormatReader`
* `AudioFormatReaderSource`
* `AudioTransportSource`

The two deck outputs are combined using `MixerAudioSource` to create a complete DJ mixing workflow.

---

# 🎚️ Track Mixing Controls

## Volume Control

Each deck contains an independent volume slider.

Users can:

* Increase or decrease track volume
* Balance two tracks during playback
* Perform manual crossfades

Volume changes are applied in real time through JUCE's audio processing pipeline.

## Playback Speed Control

Each deck includes adjustable playback speed control.

Range:

```
0.5x - 2.0x
```

The speed system uses JUCE's `ResamplingAudioSource`.

Examples:

* `1.0x` → Original speed
* Above `1.0x` → Faster playback
* Below `1.0x` → Slower playback

This allows users to perform tempo adjustments for beat matching.

---

# 📚 Playlist Management

## Music Library

The application includes a persistent playlist system where users can build and manage their music collection.

Supported features:

* Add multiple audio files
* Display track information
* Show track duration
* Load tracks directly into either deck
* Remove unwanted tracks

Track duration is calculated using JUCE audio metadata:

* Total sample length
* Sample rate

---

## Playlist Persistence

Playlist data is automatically saved and restored between sessions.

Stored information includes:

* Track file paths
* Playlist contents

The application creates a local storage file:

```
playlist.txt
```

When the application starts, previously saved tracks are automatically reloaded.

---

# 🔥 Hot Cue System

The application includes an 8-point hot cue system designed for live remixing and performance.

## Assigning Hot Cues

Users can create cue points while a track is playing or paused.

Each hot cue stores:

* Playback position
* Assignment status

Assigned hot cues are visually indicated through button colour changes.

Example uses:

* Vocal entry points
* Drum breaks
* Drops
* Important sections of a track

---

## Editing Hot Cues

Existing hot cues can be updated without deleting them.

This allows users to fine-tune cue positions during preparation or live mixing.

---

## Clearing Hot Cues

A dedicated clear function allows users to:

* Remove all assigned hot cues
* Reset the hot cue interface
* Update stored data

---

## Hot Cue Persistence

Hot cues are saved per track and restored automatically.

This allows users to close and reopen the application without losing previously prepared cue points.

---

# 🎛️ Three-Band Equaliser

Each deck includes a three-band EQ mixer.

## EQ Controls

| Band | Filter Type    | Range         |
| ---- | -------------- | ------------- |
| Low  | Low Shelf      | -24dB to +6dB |
| Mid  | Peaking Filter | -24dB to +6dB |
| High | High Shelf     | -24dB to +6dB |

The EQ controls use rotary sliders to replicate a traditional DJ mixer interface.

---

## Real-Time Audio Processing

The equaliser is implemented using JUCE's DSP module:

```
ProcessorChain
│
├── Low Shelf Filter
├── Mid Peaking Filter
└── High Shelf Filter
```

EQ adjustments are applied immediately during playback.

---

## EQ Persistence

Each track stores its own EQ configuration:

* Low gain
* Mid gain
* High gain

Settings are saved and restored automatically when tracks are loaded again.

---

# 🥁 BPM Detection and Visualisation

## BPM Analysis

The application calculates the tempo of the currently playing track by analysing audio peaks.

The BPM calculation is based on:

```
BPM = 60000 / Time Between Beats (milliseconds)
```

An envelope follower detects peaks in the audio signal, estimates beat intervals, and updates the BPM value dynamically.

---

## BPM Display

The detected BPM is displayed directly on the user interface.

This helps DJs:

* Compare track tempos
* Select compatible tracks
* Prepare smoother transitions

---

# 💿 DJ Visual Features

## Rotating Vinyl Display

The application includes a vinyl-style disk visualisation.

Features:

* Rotates during playback
* Rotation speed follows playback speed
* Stops when the track finishes

This provides visual feedback similar to traditional DJ equipment.

---

## Track Duration Display

The remaining track duration is displayed in the centre of the vinyl disk.

This helps users:

* Monitor playback time
* Prepare transitions
* Plan hot cue placements

---

# 🛠️ Technical Details

## Development Tools

| Component        | Technology      |
| ---------------- | --------------- |
| Language         | C++             |
| Audio Framework  | JUCE            |
| Project Tool     | Projucer        |
| Audio Processing | JUCE DSP Module |

---

## Main JUCE Components Used

| JUCE Component          | Purpose                   |
| ----------------------- | ------------------------- |
| `AudioTransportSource`  | Track playback control    |
| `MixerAudioSource`      | Combining deck outputs    |
| `ResamplingAudioSource` | Playback speed adjustment |
| `AudioFormatManager`    | Audio file handling       |
| `AudioFormatReader`     | Reading audio data        |
| `ProcessorChain`        | EQ processing             |
| `Timer`                 | UI animation updates      |

---

# 🚀 Building the Application

## Requirements

Install:

* JUCE Framework
* Projucer
* A supported C++ IDE

Examples:

* Visual Studio
* Xcode

---

## Build Instructions

1. Open the `.jucer` file using **Projucer**.
2. Configure the desired IDE exporter.
3. Save the project.
4. Open the generated IDE project.
5. Build and run the application.

---

# ✅ Implemented Requirements

| Requirement | Feature                             |
| ----------- | ----------------------------------- |
| R1          | Core DJ playback and mixing         |
| R2          | Playlist management and persistence |
| R3          | Hot cue assignment and storage      |
| R4          | Three-band EQ mixer                 |
| R5          | BPM detection and visualisation     |

---

# 🔮 Future Improvements

Potential improvements include:

* Automatic beat matching
* Crossfader control
* Waveform beat markers
* Looping functionality
* Additional audio effects:

  * Reverb
  * Delay
  * Filters
* AI-assisted track recommendations

---

# 📌 Conclusion

This JUCE-based DJ application demonstrates the implementation of a complete digital mixing environment using C++.

By combining real-time audio processing, DSP effects, persistent storage, and interactive visual elements, the project provides a functional DJ workflow with features inspired by professional DJ software.
