#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <string.h>
#include "wav_hist.h"

#define REGULAR 0
#define MID 1
#define SIDE 2

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {
	if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <channel> ?<MID/SIDE>\n? OPTIONAL\n";
		return 1;
	}

	int mode = REGULAR;
	if(argc > 3) {
		if(strcmp(argv[3], "MID") == 0) {
			mode = MID;
		} else if(strcmp(argv[3], "SIDE") == 0) {
			mode = SIDE;
		} else {
			cerr << "Error: invalid working mode selected\n";
		}
	}

	SndfileHandle sndFile { argv[1] };
	if(sndFile.error()) {
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

	int channel { stoi(argv[2]) };
	if(channel >= sndFile.channels()) {
		cerr << "Error: invalid channel requested\n";
		return 1;
	}

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

	WAVHist *hist;
	if (mode == REGULAR) {
		 hist = new WAVHist(sndFile);
	} else {
		hist = new WAVHist();
	}

	int numChannels = sndFile.channels();

	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * numChannels);

		if (mode == MID) {
			// Calculate MID channel from L and R
    		vector<short> midChannel(FRAMES_BUFFER_SIZE);
        	for (size_t i = 0; i < nFrames; ++i) {
            	midChannel[i] = (samples[i * numChannels] + samples[i * numChannels + 1]) / 2;
        	}
			samples = midChannel;
		} else if (mode == SIDE) {
			// Calculate SIDE channel from L and R
		    vector<short> sideChannel(FRAMES_BUFFER_SIZE);
			for (size_t i = 0; i < nFrames; ++i) {
            	sideChannel[i] = (samples[i * numChannels] - samples[i * numChannels + 1]) / 2;
        	}
			samples = sideChannel;
		}
		
		hist->update(samples);
	}

	if (mode == MID || mode == SIDE) channel = 0;

	hist->dump(channel);

	// Free space from histogram
	delete hist;

	return 0;
}

