# osc-audio-visualizer
A real time audio visualization system which outputs beat and amplitude information using the Open Sound Control protocol.

This program utilizes the BTrack beat detection library as well as a series of routines which compute average power over a defined time window in order to derive the beat and volume level of any S16 little-endian audio stream provided via standard input in real time. This data is then used to produce a data stream output via the OSC protocol.

In terms of audio sources, any program capable of outputting an audio stream is compatible. This includes 99% of current Linux programs that output audio. This system has been specifically tested with the music player *cmus* (https://cmus.github.io/) and  the Pandora client *pianobar* (https://6xq.net/pianobar/).

# Getting started

In order to use this audio visualization system, please install the dependencies/requirements, build the source code, and configure ALSA to route audio streams through this system as described below.

# Requirements
 * libsamplerate
 * liblo
 * GNU Make
 * C and C++ compiler (tested with gcc and g++)


# Building
1. Install the necessary dependencies onto the Raspberry Pi by following the steps above.
2. Clone this repository.
3. Run: "git submodule init && git submodule update"
4. Build the executable by running "make"

# ALSA Configuration
  Once the binary is built, determine the full path to the *osc-audio-proc* binary (which will be in this directory) and replace the placeholder value on line 17 of the included *asound.conf* configuration file. Make sure not to remove the | / "pipe" character from the beginning of the line when doing so. After such, backup your *asound.conf* file and copy the new file into place by running:

        sudo cp /etc/asound.conf /etc/asound.conf.bak
        sudo cp asound.conf /etc/asound.conf

Furthermore, if there are multiple users on the system that will be playing audio, ensure the *osc-audio-proc* binary is in a location such as /opt with file permissions set such that those users may execute such binary or else audio playback will fail.

Should you which to revert to the original ALSA configuration and no longer process audio streams through this program, run:

        sudo mv /etc/asound.conf.bak /etc/asound.conf
