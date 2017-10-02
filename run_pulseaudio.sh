#!/bin/bash
OSC_ADDR="localhost"
OSC_PORT="7700"
NOISEFLOOR="-40"

if [ -n "$1" ] ; then
    pacmd list-sources
else
    pacat -r -d $1 --channels=2 --format=s16le | ./osc-audio-proc -a $OSC_ADDR -p $OSC_PORT -n $NOISEFLOOR
fi