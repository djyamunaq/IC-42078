#include <iostream>
#include <vector>
#include <cmath>
#include <sndfile.hh>
#include <string.h>
#include "wav_hist.h"

#define REGULAR 0
#define MID 1
#define SIDE 2

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

double averageMeanSquaredError(std::vector<short> channelA, std::vector<short> channelB);
short maximumNorm(std::vector<short> channelA, std::vector<short> channelB);
double signalToNoiseRatio(std::vector<short> channelA, std::vector<short> channelB);

int main(int argc, char *argv[]) {
	if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <input original file>\n";
		return 1;
	}

	SndfileHandle sndFile { argv[1] };
	SndfileHandle sndFileOriginal { argv[2] };

	if(sndFile.error() || sndFileOriginal.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    }

	if((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}

    if((sndFileOriginal.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: original file is not in WAV format\n";
		return 1;
	}

	if((sndFileOriginal.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: original file is not in PCM_16 format\n";
		return 1;
	}

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	vector<short> samplesOriginal(FRAMES_BUFFER_SIZE * sndFileOriginal.channels());
	WAVHist *hist = new WAVHist(sndFile);
	WAVHist *histOriginal = new WAVHist(sndFileOriginal);
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());
		hist->update(samples);
	}
    while((nFrames = sndFileOriginal.readf(samplesOriginal.data(), FRAMES_BUFFER_SIZE))) {
		samplesOriginal.resize(nFrames * sndFileOriginal.channels());
		histOriginal->update(samplesOriginal);
	}

    // Print L2 Norms
    std::cout << "Left channel: " << averageMeanSquaredError(histOriginal->getChannelInTime(0), hist->getChannelInTime(0)) << std::endl; 
    std::cout << "Right channel: " << averageMeanSquaredError(histOriginal->getChannelInTime(1), hist->getChannelInTime(1)) << std::endl; 
    std::cout << "Average channel: " << averageMeanSquaredError(histOriginal->getMidChannelInTime(), hist->getMidChannelInTime()) << std::endl; 

    std::cout << std::endl;

    // Print Linf Norms
    std::cout << "Left channel: " << maximumNorm(histOriginal->getChannelInTime(0), hist->getChannelInTime(0)) << std::endl; 
    std::cout << "Right channel: " << maximumNorm(histOriginal->getChannelInTime(1), hist->getChannelInTime(1)) << std::endl; 
    std::cout << "Average channel: " << maximumNorm(histOriginal->getMidChannelInTime(), hist->getMidChannelInTime()) << std::endl; 

    std::cout << std::endl;

    // Print Linf Norms
    std::cout << "Left channel: " << signalToNoiseRatio(histOriginal->getChannelInTime(0), hist->getChannelInTime(0)) << std::endl; 
    std::cout << "Right channel: " << signalToNoiseRatio(histOriginal->getChannelInTime(1), hist->getChannelInTime(1)) << std::endl; 
    std::cout << "Average channel: " << signalToNoiseRatio(histOriginal->getMidChannelInTime(), hist->getMidChannelInTime()) << std::endl; 

	// Free space from histogram
    delete hist;
    delete histOriginal;

	return 0;
}

/* Calculate the L2 Norm for two collections of samples */
double averageMeanSquaredError(std::vector<short> channelA, std::vector<short> channelB) {
    int sizeA = channelA.size();
    int sizeB = channelB.size();

    long int N = sizeA > sizeB ? sizeB : sizeA;
    double sum = 0;
    for (int i=0; i<N; i++) {
        double diffSqr = pow((double) channelA[i] - (double) channelB[i], 2);
        sum += diffSqr;
    }

    return sqrt(sum)/N;
}

/* Calculate the Linf Norm for two collections of samples */
short maximumNorm(std::vector<short> channelA, std::vector<short> channelB) {
    int sizeA = channelA.size();
    int sizeB = channelB.size();

    long int N = sizeA > sizeB ? sizeB : sizeA;
    short res = abs(channelA[0] - channelB[0]);
    for (int i=0; i<N; i++) {
        short diff = abs(channelA[i] - channelB[i]);
        if (diff > res) {
            res = diff;
        }
    }

    return res;
}

/* Calculate the SNR for two collections of samples */
double signalToNoiseRatio(std::vector<short> channelA, std::vector<short> channelB) {
    int sizeA = channelA.size();
    int sizeB = channelB.size();

    long int N = sizeA > sizeB ? sizeB : sizeA;
    std::vector<short> d = std::vector<short>(N);
    for (int i=0; i<N; i++) {
        d[i] = channelA[i] - channelB[i];
    }

    double sumA = 0;
    double sumD = 0;
    for(int i=0; i<N; i++) {
        sumA += pow(channelA[i], 2);
        sumD += pow(d[i], 2);
    }

    double snr = 10*log10(sumA/sumD);

    return snr;
}

