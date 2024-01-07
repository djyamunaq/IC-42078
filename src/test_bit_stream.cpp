#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <assert.h>
#include "bit_stream.h"

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
    BitStream bitStream;

    /* A = 0x41 | B = 0x42 */
    int A[8] = {0, 1, 0, 0, 0, 0, 0, 1};
    int B[8] = {0, 1, 0, 0, 0, 0, 1, 0};
    int C[8] = {0, 1, 0, 0, 0, 0, 1, 1};

    for(int i=0; i<8; i++) {
        int value = A[i];
        cout << value << " ";
        bitStream.pushBit(value);  
    }
    for(int i=0; i<8; i++) {
        int value = B[i];
        cout << value << " ";
        bitStream.pushBit(value);  
    }
    for(int i=0; i<8; i++) {
        int value = C[i];
        cout << value << " ";
        bitStream.pushBit(value);  
    } cout << endl;

    bitStream.printStreamInBytes();

    bitStream.resetBitPointerPosition();
    for(int i=0; i<24; i++) {
        // cout << "Bit position: " << bitStream.getBitPointerPosition() << endl;
        // cout << "Bit: " << bitStream.getBit() << " " << endl;
        cout << bitStream.getBit() << " ";
        bitStream.nextBitPointerPosition();
    } cout << endl;

    return 0;
}