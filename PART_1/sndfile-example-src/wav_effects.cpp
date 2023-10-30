#include <iostream>
#include <vector>
#include <cmath>
#include <sndfile.hh>
#include <string.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

void applyEcho(std::vector<short>& samples, int sampleRate, double delayInSeconds, double decay);

int main(int argc, char *argv[]) {
	if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		return 1;
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

    SndfileHandle sfhOut { argv[2], SFM_WRITE, sndFile.format(), sndFile.channels(), sndFile.samplerate() };

    if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

	size_t nFrames;
	vector<short> samples(sndFile.frames() * sndFile.channels());
	sndFile.read(samples.data(), samples.size());

    applyEcho(samples, 1000, 0.5, 0.5);

    sfhOut.write(samples.data(), samples.size());

	return 0;
}

void applyEcho(std::vector<short>& samples, int sampleRate, double delayInSeconds, double decay) {
    int delaySamples = static_cast<int>(delayInSeconds * sampleRate);
    
    for (int i = delaySamples; i < samples.size(); i++) {
        samples[i] += samples[i - delaySamples] * decay;
    }
}
