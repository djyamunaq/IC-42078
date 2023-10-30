#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>

class WAVQuant {
  private:

  public:
		WAVQuant() {
        }

        std::vector<short> quantize(std::vector<short> signal, int L) {
            // Ensure there are enough levels and signal isn't empty
            if (L <= 1 || signal.empty()) {
                return {};
            }

            // 1. Determine the Range of Signal
            short min_val = *std::min_element(signal.begin(), signal.end());
            short max_val = *std::max_element(signal.begin(), signal.end());

            // 2. Calculate Step Size
            double delta = double (max_val - min_val) / L;

            // 3. Quantize Each Sample
            std::vector<short> quantized_signal(signal.size());
            for (size_t i = 0; i < signal.size(); i++) {
                int level = static_cast<int>((signal[i] - min_val) / delta);
                quantized_signal[i] = min_val + (level + 0.5) * delta;
            }

            return quantized_signal;
        }
};

#endif

