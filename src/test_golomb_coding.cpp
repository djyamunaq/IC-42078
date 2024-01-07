#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <assert.h>
#include "golomb.h"

using std::cout;
using std::cerr;
using std::endl;
using std::stoi;
using std::flush;
using std::string;
using std::invalid_argument;
using std::ofstream;
using cv::imread;
using cv::imwrite;
using cv::Vec3b;
using cv::Mat;
using cv::cvtColor;
using cv::imread;
using cv::imshow;
using cv::waitKey;
using cv::Rect;
using cv::COLOR_BGR2GRAY;
using std::to_string;

int main (int argc, char** argv) {
    GolombCoding golombCoding(5, true);

    /* Assert golomb encoder is working properly */
    cout << "Testing Golomb Encoder..." << endl;
    for (long long int value=0; value<=255; value++) {
        string encodedBits = golombCoding.encode(value);
        int decodedValue = golombCoding.decode(encodedBits);
        cout << value << " | " << encodedBits << " | " << decodedValue << endl;
    }

    return 0;
}