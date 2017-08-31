# matrix-audio-visualizer
An real time audio  visualization system using the MATRIX Creator everloop.

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
    sudo apt-get install build-essential libsamplerate-dev 

# Building
1. Install the necessary dependencies onto the Raspberry Pi by following the steps above.
2. Clone this repository.
3. Run: "git submodule init && git submodule update"
4. Bulid the executable by running "make"

# Configuration
  TODO (need physical access to pi)
