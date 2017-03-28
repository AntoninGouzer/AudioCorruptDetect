# AudioCorruptDetect
Automatic detection of Digital degradations in audio files.
V1.0: Detection of consecutive identical Samples values.
This default is observed during audio CD ripping.

# Install
- Clone the repository
- install libsndfile: http://www.mega-nerd.com/libsndfile/#Download
- compile: make


# Commande line Example
(4 samples threshold) : 
./AudioCorruptDetect -i in.wav -SampleHold -sampleHoldTresh 4 -mergeTimeHoldSec 0 -audacityMode

The output is compatible with Audacity Marker files:

1.58293	1.58293	Suspicious Sample Hold of  4 samples, channel 0 
