#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <assert.h>
#include "predictor.h"
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
    /******************************************************************************/
    /* 0. OPEN IMAGE                                                              */
    /******************************************************************************/
    /* Check if command line inputs are correctly provided */
    if (argc < 2) {
        cerr << "[ERROR] Usage: " << argv[0] << " <input file>" << endl;
        return -1;
    }

    /* Read file name from command line */
    string fileName(argv[1]);

    /* Load the image from a file (Second param = 0 => Grayscale image) */
    Mat imgInput = imread(fileName, 0);
    
    /* Check if image is empty */
    if (imgInput.empty()) {
        cerr << "[ERROR] Empty image" << endl;
        return -1;
    }

    /******************************************************************************/
    /* 1. SET PREDICTOR                                                           */
    /******************************************************************************/
    cout << "> Set Predictor..." << endl;
    int mode = 1;
    Predictor predictor(mode);
    
    Mat residualMatrix = predictor.calculateResidualMatrix(imgInput);

    // cout << imgInput.rows << " " << imgInput.cols << endl;
    // cout << residualMatrix.rows << " " << residualMatrix.cols << endl;

    /******************************************************************************/
    /* 2. ENCODE RESIDUALS                                                        */
    /******************************************************************************/
    cout << "> Encode Residuals..." << endl;

    int m = 7;

    /* Set golomb coding instance with sign treating */
    GolombCoding golombCoding(m, true);
    
    /* Assert golomb encoder is working properly */
    // cout << "Testing Golomb Encoder..." << endl;
    // for (long long int value=-100000; value<=100000; value++) {
    //     string encodedBits = golombCoding.encode(value);
    //     int decodedValue = golombCoding.decode(encodedBits);

    //     assert (value==decodedValue);
    // }

    for (int i=0; i<residualMatrix.rows; i++) {
        for (int j=0; j<residualMatrix.rows; j++) {
            unsigned int value = residualMatrix.at<uchar>(i, j);
            golombCoding.encode(value);
        }
    }
    
    /******************************************************************************/
    /* 3. WRITE COMPRESSED DATA TO FILE                                           */
    /******************************************************************************/
    cout << "> Write to Output..." << endl;

    /* Create output file */
    ofstream outputFile("../ex_1_data.comp");

    BitStream bitStream = golombCoding.getBitStream();

    /* Check if the file is open */
    if (outputFile.is_open()) {
        /* Metadata header */
            /* Write predictor mode */
        outputFile << mode << "\n";
            /* Write m for golomb decoding */
        outputFile << m << "\n";
            /* Write number of bits in residual part */
        outputFile << bitStream.getSizeInBits() << "\n";
        unsigned int sizeInBytes = bitStream.getSizeInBytes();
        
        bitStream.resetBytePointerPosition();

        while(bitStream.getBytePointerPosition() < sizeInBytes) {
            cout << bitStream.getBytePointerPosition() << "\\" << sizeInBytes << endl;
            unsigned char value = bitStream.getByte();
            outputFile << value;
            bitStream.nextBytePointerPosition();
        }
        bitStream.resetBytePointerPosition();

        outputFile << "\n";

        /* Close the file */
        outputFile.close();
        std::cout << "Data has been written to the file.\n";
    } else {
        /* If the file couldn't be opened */
        std::cerr << "Unable to open the file.\n";
    }

    /******************************************************************************/
    /* 4. DECOMPRESS FILE BACK TO .PPM FILE                                       */
    /******************************************************************************/
    

    // Mat imgOutput;
    // imgOutput.create(imgInput.rows, imgInput.cols, imgInput.type());
    /* Build image frame to show input and output image side by side */
    // Mat frame(imgInput.rows, 2*imgInput.cols, imgInput.type());
    // Mat left(frame, Rect(0, 0, imgInput.cols, imgInput.rows));
    // imgInput.copyTo(left);
    // Mat right(frame, Rect(imgOutput.cols, 0, imgOutput.cols, imgOutput.rows));
    // imgOutput.copyTo(right);

    /* Show the images */
    // imshow("3 channels | 1 channel", frame);

    /* Wait for a keystroke in the window */
    // waitKey(0);

    return 0;
}