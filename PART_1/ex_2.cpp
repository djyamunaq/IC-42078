#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <math.h>

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

void setNegativeImage(Mat& input, Mat& output);
void mirrorImage(Mat& input, Mat& output, int direction);
void rotateImage(Mat& input, Mat& output, int n);
void modifyIntensity(Mat& input, Mat& output, int n);

int main(int argc, char** argv) {
    /* Check if command line inputs are correctly provided */
    if (argc < 3) {
        cerr << "[ERROR] Usage: " << argv[0] << " <input file> <option> <arg>*" << endl;
        cerr << "\t0: Negative of image" << endl;
        cerr << "\t1: Mirrored version of image >> 0: Horizontal, 1: Vertical" << endl;
        cerr << "\t2: Rotates image 90º >> n: Number of times" << endl;
        cerr << "\t3: Increases (more light) / decreases (less light) the intensity values of an image >> v: value" << endl;
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
    imgOutput.create(imgInput.rows, imgInput.cols, imgInput.type());

    /* Try to read option from command line */
    int option;
    try {
        option = stoi(argv[2]);
    } catch(invalid_argument e) {
        cerr << "[ERROR] Invalid type for option: " << argv[2] << endl; 
        return -1;
    }

    if (option < 0 || option > 3) {
        cerr << "[ERROR] Wrong value for option: 0, 1, 2 or 3" << endl;
        return -1;
    }

    /* Check if option has correspondent value associated */
    int *value = nullptr;
    if (option != 0 && argc > 3) {
        try {
            value = (int*) malloc(sizeof(int));
            *value = stoi(argv[3]);
        } catch(invalid_argument e) {
            cerr << "[ERROR] Invalid type for option value: " << argv[2] << endl; 
            return -1;
        }
        
    }

    /* Execute option 0: Negative of image */
    if (option == 0) {
        setNegativeImage(imgInput, imgOutput);
    } 

    /* Execute option 1: Mirror image */
    else if (option == 1) {
        if (value == nullptr || (*value != 0 && *value != 1)) {
            cerr << "[ERROR] Wrong selected value for option value >> value = [0, 1]" << endl;
            return -1;
        }

        mirrorImage(imgInput, imgOutput, *value);
    } 

    /* Execute option 2: Rotate image */
    else if (option == 2) {
        if (value == nullptr) {
            cerr << "[ERROR] Wrong value for option 2 >> value is integer" << endl;
            return -1;
        }

        rotateImage(imgInput, imgOutput, *value);
    } 

    /* Execute option 3: Modify image intensity */
    else if (option == 3) {
        if (value == nullptr) {
            cerr << "[ERROR] Wrong value for option 3 >> value is integer" << endl;
            return -1;
        }

        modifyIntensity(imgInput, imgOutput, *value);
    }

    /* Build image frame to show input and output image side by side */
    Mat frame(imgInput.rows, 2*imgInput.cols, imgInput.type());
    Mat left(frame, cv::Rect(0, 0, imgInput.cols, imgInput.rows));
    imgInput.copyTo(left);
    Mat right(frame, cv::Rect(imgOutput.cols, 0, imgOutput.cols, imgOutput.rows));
    imgOutput.copyTo(right);

    /* Show the images */
    cv::imshow("Original | Modified", frame);

    /* Wait for a keystroke in the window */
    cv::waitKey(0);

    /* Write the output image to a new file */
    string outputFileName = "../ex_2_output_image.jpg";
    bool isSuccess = cv::imwrite(outputFileName, imgOutput);

    if (!isSuccess) {
        cerr << "[ERROR] Error writing image" << endl;
        return -1;
    }
    
    return 0;
}

/* 
 * setNegativeImage: modify input RGB matrix and set it to negative (e.g., for a pixel value of X, set pixel to 255 - X)
 * input: matrix of input image, matrix of output image
 * output: void
 */ 
void setNegativeImage(Mat& input, Mat& output) {
    /* Modify image pixel by pixel */ 
    for (int y=0; y<input.rows; y++){
        for (int x=0; x<input.cols; x++) {
            output.at<Vec3b>(y, x)[0] = 255 - input.at<Vec3b>(y, x)[0];
            output.at<Vec3b>(y, x)[1] = 255 - input.at<Vec3b>(y, x)[1];
            output.at<Vec3b>(y, x)[2] = 255 - input.at<Vec3b>(y, x)[2];
        }
    }
}

/* 
 * mirrorImage: mirror image around axis (0: Horizontal / 1: Vertical)
 * input: matrix of input image, matrix of output image, direction
 * output: void
 */ 
void mirrorImage(Mat& input, Mat& output, int direction) {
    /* Modify image pixel by pixel */ 
    switch (direction) {
        case 0:
            for (int x=0; x<input.cols/2; x++) {
                for (int y=0; y<input.rows; y++){
                    output.at<Vec3b>(y, x) = input.at<Vec3b>(y, input.cols-x-1);
                    output.at<Vec3b>(y, input.cols-x-1) = input.at<Vec3b>(y, x);
                }
            }

            break;
        case 1:
            for (int y=0; y<input.rows/2; y++){
                for (int x=0; x<input.cols; x++) {
                    output.at<Vec3b>(y, x) = input.at<Vec3b>(input.rows-y-1, x);
                    output.at<Vec3b>(input.rows-y-1, x) = input.at<Vec3b>(y, x);
                }
            }

            break;
    }
    
}

/* 
 * rotateImage: rotate image n times by 90º
 * input: matrix of input image, matrix of output image, number of turns
 * output: void
 */ 
void rotateImage(Mat& input, Mat& output, int n) {
    /* Reduce redundant turns */ 
    n = n % 4;

    /* Get angle in radians then get cos and sin */
    float angle = n*M_PI/2;
    float cosAngle = cos(angle); 
    float sinAngle = sin(angle);

    /* Modify image pixel by pixel */ 
    for (int y=0; y<input.rows; y++){
        for (int x=0; x<input.cols; x++) {
            int newX = cosAngle*(x-input.cols/2) - sinAngle*(y-input.rows/2) + input.cols/2;
            int newY = sinAngle*(x-input.cols/2) + cosAngle*(y-input.rows/2) + input.rows/2;

            // cout << newX << " | " << newY << endl;

            output.at<Vec3b>(newY, newX) = input.at<Vec3b>(y, x);
        }
    }
}

/* 
 * modifyIntensity: modify the intensity values of an image input in the format of a RGB matrix 
 * input: matrix of input image, matrix of output image, value of change
 * output: void
 */ 
void modifyIntensity(Mat& input, Mat& output, int value) {
    /* Modify image pixel by pixel */ 
    for (int y=0; y<input.rows; y++){
        for (int x=0; x<input.cols; x++) {
            /* Set max value to 255 and min to 0 */
            int newValue = input.at<Vec3b>(y, x)[0] + value;
            output.at<Vec3b>(y, x)[0] = newValue > 255 ? 255 : (newValue < 0 ? 0 : newValue);
            newValue = input.at<Vec3b>(y, x)[1] + value;
            output.at<Vec3b>(y, x)[1] = newValue > 255 ? 255 : (newValue < 0 ? 0 : newValue);
            newValue = input.at<Vec3b>(y, x)[2] + value;
            output.at<Vec3b>(y, x)[2] = newValue > 255 ? 255 : (newValue < 0 ? 0 : newValue);
        }
    }
}