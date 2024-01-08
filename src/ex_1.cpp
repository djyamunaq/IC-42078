#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <assert.h>
#include <filesystem>
#include "compressor1frame.h"

using std::cout;
using std::cerr;
using std::endl;
using std::stoi;
using std::stoll;
using std::flush;
using std::getline;
using std::string;
using std::invalid_argument;
using std::ofstream;
using std::ifstream;
using cv::imread;
using cv::imwrite;
using cv::Vec3b;
using cv::Mat;
using cv::cvtColor;
using cv::imshow;
using cv::waitKey;
using cv::Rect;
using cv::COLOR_BGR2GRAY;
using std::to_string;
using std::filesystem::path;
using std::filesystem::absolute;

int main (int argc, char** argv) {
    /******************************************************************************/
    /* 0. OPEN IMAGE                                                              */
    /******************************************************************************/
    /* Check if command line inputs are correctly provided */
    if (argc < 2) {
        cerr << "[ERROR] Usage: " << argv[0] << " <input file>" << endl;
        return -1;
    }

    /* Read file name from command line */
    path inputPath(argv[1]);

    /* Load the image from a file (Second param = 0 => Grayscale image) */
    Mat imgInput = imread(inputPath, 0);

    /* Compressor object with compress and decompress methods */
    Compressor compressor;

    /******************************************************************************/
    /* 1. COMPRESS IMAGE to .COMP FORMAT                                          */
    /******************************************************************************/
    cout << "> compress file..." << endl;
    path outputPath = absolute("../ex_1_data.comp");
    compressor.compress(imgInput, outputPath);

    /******************************************************************************/
    /* 2. DECOMPRESS FILE BACK TO .PPM FORMAT                                     */
    /******************************************************************************/
    cout << "> Decompress file..." << endl;

    inputPath = absolute("../ex_1_data.comp");
    outputPath = absolute("../ex_1_data_decompressed.ppm");
    compressor.decompress(inputPath, outputPath);
    
    Mat imgOutput = imread(outputPath);

    /* Show the images */
    imshow("Original", imgInput);
    imshow("Compressed >> Decompressed", imgOutput);
    waitKey(0);

    return 0;
}