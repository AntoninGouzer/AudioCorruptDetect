# AudioCorruptDetect
Automatic detection of Digital degradations in audio files.
V1: Detection of consecutive identical Samples values.
This default is observed during audio CD ripping.

Exemple (4 samples threshold) : 
./AudioCorruptDetect test.wav NULL 19.0 20 300 1024 1 1 1 4 

the output is compatible with Audacity Marker files:

1.58293	1.58293	Suspicious Sample Hold of  4 samples, channel 0 
2.13336	2.13336	Suspicious Sample Hold of  4 samples, channel 0 
3.23209	3.23209	Suspicious Sample Hold of  4 samples, channel 0 
3.41395	3.41395	Suspicious Sample Hold of  4 samples, channel 0 
3.59549	3.59549	Suspicious Sample Hold of  4 samples, channel 1 
3.59576	3.59576	Suspicious Sample Hold of  4 samples, channel 1 
3.59932	3.59932	Suspicious Sample Hold of  4 samples, channel 0 
3.77694	3.77694	Suspicious Sample Hold of  4 samples, channel 1 
5.05966	5.05966	Suspicious Sample Hold of  4 samples, channel 1 
5.05980	5.05980	Suspicious Sample Hold of  4 samples, channel 1 
