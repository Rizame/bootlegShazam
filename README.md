# Installing necessary packages inside WSL:

## FFT and python essentials
sudo apt update && sudo apt install -y \
build-essential cmake pkg-config \
libfftw3-dev \
python3 python3-dev python3-pip

## Python packages for plotting
python3 -m pip install --user numpy matplotlib

# Main concept and how it works:
A song recognition application. Based on audio processing and working within the frequency domain. This application is a mere attempt to re create the main idea behind Shazam, provide a FAST hash based song matching and do so also pointing out the exact piece of played song.

## Pipeline

In more detail here is how the algorithm is implemented from song up loading to the final decision of a song.
### Audio preparation
![img.png](images/img.png)
### Converting to frequency domain
![img_1.png](images/img_1.png)
### Encoding and scoring sound
![img_2.png](images/img_2.png)

### Nifty things
In the code it is possible to allow spectrogram creation to see how sound looks plotted in a frequency domain.
Also in the main root of the project after running a song recognition algorithm a sound file will appear - a reconstruction of the inputted audio after the lowpass filter and downsampling were applied!

### Working example
When running the recognition function you will see such logs in the console:
![img.png](images/audioProcessing.png)

The audio gets filtered, turned to mono, downsampled,
and turned into a spectrogram and a bunch of fingerprints.

![img.png](images/songsFiltering.png)
Then you will see a print out of the matching function. That filters out songs where a lot of complete
TGZ's are missing. And then it will count the most appearing offset in comparison to db matches.

![img.png](images/result.png)

Lastly it prints out the decision.