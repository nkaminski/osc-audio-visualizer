#!/bin/bash
OSC_ADDR="localhost"
OSC_PORT="7700"
NOISEFLOOR="-40"

if [ -n "$1" ] ; then
    pacat -r -d $1 --channels=2 --format=s16le | ./osc-audio-proc -a $OSC_ADDR -p $OSC_PORT -n $NOISEFLOOR
else
    pacmd list-sources | grep "Name:"
fi
