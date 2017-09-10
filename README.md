# matrix-audio-visualizer
A real time audio visualization system using the MATRIX Creator everloop.

This program utilizes the BTrack beat detection library as well as a series of routines which compute average power over a defined time window in order to derive the beat and volume level of any ALSA audio stream in real time. This data is then used to render an "image" of a circular segment on the everloop of the MATRIX creator where the color of the segment is changed and the starting point advanced by one LED for each detected beat and the length of the segment is proportional to the power/volume level of the audio source.

In terms of audio sources, any program capable of outputting an audio stream via ALSA is compatible. This includes 99% of current Linux programs that output audio. This system has been specifically tested with the music player *cmus* (https://cmus.github.io/) and  the Pandora client *pianobar* (https://6xq.net/pianobar/).

# Getting started

In order to use this audio visualization system, please install the dependencies/requirements, build the source code, and configure ALSA to route audio streams through this system as described below.

Once the system is installed, the easiest means of getting things running is to create a Pandora account on pandora.com and install pianobar by running:

    sudo apt-get install pianobar

Furthermore, since the version of pianobar in the Debian repositories is out of date, pianobar will refuse to interact with pandora.com due to an incorrect certificate. The fix for such is to either build the most recent version of pianobar form source or create the file "~/.config/pianobar/config" and place the following lines inside of such:

        tls_fingerprint = FC2E6AF49FC63AEDAD1078DC22D1185B809E7534
        user = youremail@example.com
        password = YOUR_PANDORA.COM_PASSWORD

If you would like to use this system with audio files you have locally, you can install the command line audio player *cmus* by running:

    sudo apt-get install cmus

Please refer to cmus' documentation for usage since there are far to many options and cases to list here.

Finally, be aware that these are merely suggestions and that any graphical or command line program capable of playing audio should work with this system.

# Requirements
 * MATRIX Creator HAL
 * matrix-creator-init package
 * libsamplerate
 * alsa-utils
 * GNU Make
 * C and C++ compiler (tested with gcc and g++)
 
These dependendencies can be installed on Raspbian by running:

    sudo apt-get install wget
    sudo sh -c "$(wget https://raw.githubusercontent.com/matrix-io/matrix-creator-quickstart/master/install.sh -O -)"
    sudo apt-get install build-essential libsamplerate-dev alsa-utils

# Building
1. Install the necessary dependencies onto the Raspberry Pi by following the steps above.
2. Clone this repository.
3. Run: "git submodule init && git submodule update"
4. Build the executable by running "make"

# Configuration
  Once the binary is built, determine the full path to the *audio-proc* binary (which will be in this directory) and replace the placeholder value on line 17 of the included *asound.conf* configuration file. Make sure not to remove the | / "pipe" character from the beginning of the line when doing so. After such, backup your *asound.conf* file and copy the new file into place by running:

        sudo cp /etc/asound.conf /etc/asound.conf.bak
        sudo cp asound.conf /etc/asound.conf

Furthermore, if there are multiple users on the system that will be playing audio, ensure the *audio-proc* binary is in a location such as /opt with file permissions set such that those users may execute such binary or else audio playback will fail.

Should you which to revert to the original ALSA configuration and no longer process audio streams through this program, run:

        sudo mv /etc/asound.conf.bak /etc/asound.conf

# Hacking
The behavior of this audio processing system can be modified in a series of ways. First, this program can be used as a template for a more advanced system interfacing with the MATRIX creator everloop and/or performing real-time audio beat detection. Additionally, thereare a few constants that can be edited by the user that will change the behavior of the visualization.

* On line 11 of main.cpp, AMPLITUDE_WINDOW, which defines how many beat detection windows the average power in dB is computed over. Larger values will result in a less sensitive and slower length change of the segment while smaller values will result in faster reaction to changes in volume.
* On line 12 of main.cpp ROTATE_SPEED defines how many LED positions the arc is advanced during each beat.
* From line 80 to the end of main.cpp, the last 4 arguments to setEverloopCircular represent the R, G, B and W values of each of the colors displayed and can be modified to change such colors.
