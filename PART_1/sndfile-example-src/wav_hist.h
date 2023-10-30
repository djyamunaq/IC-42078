#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm>

class WAVHist {
  private:
		std::vector<std::map<short, size_t>> counts;
		std::map<short, size_t> sideChannel;
		std::map<short, size_t> midChannel;
		std::vector<short> midChannelVec;
		std::vector<short> sideChannelVec;
		std::vector<short> channel0;
		std::vector<short> channel1;

		/* Provide SIDE channel */
		void calculateSideChannel() {
			if (counts.size() != 2) {
				std::cout << "Not an stereo audio! It's not possible to calculate side channel!" << std::endl;
			}

			int channelSize = channel0.size();
			sideChannelVec.resize(channelSize);
			for (int i=0; i<channelSize; i++) {
				sideChannelVec[i] = (channel0[i] - channel1[i])/2;
				sideChannel[sideChannelVec[i]]++;
			}
		}

		/* Provide MID channel */
		void calculateMidChannel() {
			if (counts.size() != 2) {
				std::cout << "Not an stereo audio! It's not possible to calculate mid channel!" << std::endl;
			}

			int channelSize = channel0.size();

			midChannelVec.resize(channelSize);
			for (int i=0; i<channelSize; i++) {
				midChannelVec[i] = (channel0[i] + channel1[i])/2;
				midChannel[midChannelVec[i]]++;
			}
		}

  public:
		WAVHist() {
			counts.resize(1);
		}

		WAVHist(const SndfileHandle& sfh) {
			counts.resize(sfh.channels());
		}

		void update(const std::vector<short>& samples) {
			size_t n { };
			for(auto s : samples) {
				if (n % counts.size() == 0) {
					channel0.push_back(s);
				} else {
					channel1.push_back(s);
				}
				counts[n++ % counts.size()][s]++;
			}
		}

		void dump(const size_t channel) const {
			for(auto [value, counter] : counts[channel])
				std::cout << value << '\t' << counter << '\n';
		}

		/**/
		std::map<short, size_t> getChannel(int channel) {
			if (counts.size() > channel) {
				return counts[channel];
			}
		}

		/**/
		std::map<short, size_t> getSideChannel() {
			if (sideChannel.empty()) {
				calculateSideChannel();
			}

			return sideChannel;
		}

		/**/
		std::map<short, size_t> getMidChannel() {
			if (midChannel.empty()) {
				calculateMidChannel();
			}

			return midChannel;
		}

		/**/
		void displayHistogram(std::map<short, size_t> values, int binWidth) {
			// 
			if (values.empty()) {
				std::cerr << "Data is empty" << std::endl;
				return;
			}

			// Bin the values
			std::map<short, size_t> binned;
			for (const auto& [k, f] : values) { 
				binned[k/binWidth * binWidth] += f; 
			}

			// Extract values from the map and store in a vector
			std::vector<float> xValues;
			std::vector<float> yValues;
			for (auto [key, value] : binned) {
				xValues.push_back((float) key);
				yValues.push_back((float) value);
			}

			int max_y = *std::max_element(yValues.begin(), yValues.end());

			// Parameters for the histogram image
			int hist_w = 512; 
			int hist_h = 400; 
			cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

			// Define the frequency of x-axis annotation
    		int annotateFreq = 1000;

			cv::Point prevPoint;

			// Draw the histogram bars
			for (size_t i = 0; i < xValues.size(); i++) {
				int x = xValues[i];
				int y;
				if (max_y != 0) {
					y = (yValues[i] * hist_h) / max_y;  // Normalize y value to fit within the image
				} else {
					y = 0;
				}
				cv::Point currentPoint(x, hist_h - y);

				// Draw the point
				cv::circle(histImage, currentPoint, 2, cv::Scalar(0, 0, 255), -1);

				// Connect the current point with the previous one
				if (i > 0) {
					cv::line(histImage, prevPoint, currentPoint, cv::Scalar(0, 0, 255), 2);
				}
        
				// Annotate the x-axis at regular intervals
				if (i % annotateFreq == 0 || i == xValues.size() - 1) { // the second condition ensures the last value is also annotated
					cv::putText(histImage, std::to_string((int) xValues[i]), cv::Point(x-10, hist_h - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
				}

				prevPoint = currentPoint;
			}

			// Annotate the y-axis at regular intervals
			int annotateFreqY = hist_h / 5;
			for (int i = 1; i < 5; i++) {
				int y = i * annotateFreqY;
				int value = (max_y * y) / hist_h;

				cv::putText(histImage, std::to_string(value), cv::Point(5, hist_h - y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
				cv::line(histImage, cv::Point(0, hist_h - y), cv::Point(hist_w, hist_h - y), cv::Scalar(200, 200, 200), 1, cv::LINE_8, 0); // Optional: draw a faint horizontal line for reference
			}

			cv::imshow("Histogram", histImage);
			
			cv::waitKey(0);
		}
};

#endif

