#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::stoi;
using std::string;
using std::invalid_argument;
using std::ofstream;
using cv::imread;
using cv::imwrite;
using cv::Vec3b;
using cv::Mat;
using std::to_string;

int main(int argc, char** argv) {
    /* Check if command line inputs are correctly provided */
    if (argc < 3) {
        cerr << "[ERROR] Usage: " << argv[0] << " <input file> <channel>" << endl;
        return -1;
    }

    /* Try to read channel from command line */
    int channel;
    try {
        channel = stoi(argv[2]);
    } catch(invalid_argument e) {
        cerr << "[ERROR] Invalid type for channel input: " << argv[2] << endl; 
        return -1;
    }

    if (channel < 0 || channel > 2) {
        cerr << "[ERROR] Wrong value for channel: 0, 1 or 2" << endl;
        return -1;
    }

    /* Read file name from command line */
    string fileName(argv[1]);

    /* Load the image from a file */
    Mat imgInput = cv::imread(fileName);
    
    /* Check if image is empty */
    if (imgInput.empty()) {
        cerr << "[ERROR] Empty image" << endl;
        return -1;
    }

    /* Create output image */
    Mat imgOutput;
    imgOutput.create(imgInput.rows, imgInput.cols, CV_8UC1);

    /* Modify output image pixel by pixel */ 
    for (int y=0; y<imgInput.rows; y++){
        for (int x=0; x<imgInput.cols; x++) {
            imgOutput.at<uchar>(y, x) = imgInput.at<Vec3b>(y, x)[channel];
        }
    }

    /* Build image frame to show input and output image side by side */
    Mat frame(imgInput.rows, 2*imgInput.cols, imgInput.type());
    Mat left(frame, cv::Rect(0, 0, imgInput.cols, imgInput.rows));
    imgInput.copyTo(left);
    cv::cvtColor(imgOutput, imgOutput, cv::COLOR_GRAY2BGR);
    Mat right(frame, cv::Rect(imgOutput.cols, 0, imgOutput.cols, imgOutput.rows));
    imgOutput.copyTo(right);

    /* Show the images */
    cv::imshow("3 channels | 1 channel", frame);

    /* Wait for a keystroke in the window */
    cv::waitKey(0);

    /* Write the output image to a new file */
    string outputFileName = "../ex_1_channel_" + to_string(channel) + "_image.jpg";
    bool isSuccess = cv::imwrite(outputFileName, imgOutput);

    if (!isSuccess) {
        cerr << "[ERROR] Error writing image" << endl;
        return -1;
    }
    
    return 0;
}