# AudioCorruptDetect
Automatic detection of Digital degradations in audio files.
Currently supported:

−> Detection of consecutive identical Samples values [SampleHoldDetect]
This default is observed during audio CD ripping.
−> Detection of anormal "hard cut" start and End of files [CutStartDetect,CutEndDetect]
Files with which start and end at a level beyond a treshhold (ex > -18 dB)


# Install
- Clone the repository
- install libsndfile: http://www.mega-nerd.com/libsndfile/#Download
- compile: make


# Commande line Example
(4 samples threshold) : 
./AudioCorruptDetect --i endcut.wav --SampleHoldDetect --sampleHoldTresh 4 --mergeTimeHoldSec 0 --audacityMode --CutStartDetect --CutEndDetect --CutEndTreshdB -18 CutStartTreshdB -18

See CommandeLineExamples.txt

The output is compatible with Audacity Marker files:

0.00002	0.00002	Start Level beyond the threshold (-18.00): -5.62 dB, channel #0

0.00002	0.00002	Start Level beyond the threshold (-18.00): -5.57 dB, channel #1 

0.02941	0.02941	Suspicious Sample Hold of  4 samples, channel 1 

0.03290	0.03290	Suspicious Sample Hold of  4 samples, channel 0 

1.86399	1.86399	Suspicious Sample Hold of  4 samples, channel 0 

2.23045	2.23045	Suspicious Sample Hold of  4 samples, channel 0 

2.61005	2.61005	Suspicious Sample Hold of  4 samples, channel 0 

2.63748	2.63748	Suspicious Sample Hold of  4 samples, channel 1 

7.54256	7.54256	Suspicious Sample Hold of  4 samples, channel 1 

7.59692	7.59692	End Level beyond the threshold (-18.00): -12.96 dB , channel #0 

7.59692	7.59692	End Level beyond the threshold (-18.00): -12.55 dB , channel #1 
