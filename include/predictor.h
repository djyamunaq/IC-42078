
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
using cv::cvtColor;
using cv::imread;
using cv::imshow;
using cv::waitKey;
using cv::Rect;
using cv::COLOR_BGR2GRAY;
using std::to_string;

class Predictor {
    private:
        unsigned int mode;

    public:
        Predictor(int mode) : mode(mode) {
        }

        Mat calculateActualMatrix(Mat residualMatrix) {
            Mat actualMatrix = Mat::zeros(residualMatrix.rows, residualMatrix.cols, residualMatrix.type());

            for(int i=0; i<residualMatrix.rows; i++) {
                for(int j=0; j<residualMatrix.cols; j++) {
                    int a, b, c;
                    a = b = c = 0;
                    if (j > 0) {
                        a = (int) actualMatrix.at<uchar>(i, j-1);

                        if (i > 0) {
                            c = (int) actualMatrix.at<uchar>(i-1, j-1);
                        }
                    }
                    if (i > 0) {
                        b = (int) actualMatrix.at<uchar>(i-1, j);
                    }

                    int prediction = predict(a, b, c);
                    int actualValue = prediction + residualMatrix.at<uchar>(i, j);
                    actualMatrix.at<uchar>(i, j) = actualValue;
                }
            }

            // cout << residualMatrix << endl;

            return actualMatrix;
        }

        Mat calculateResidualMatrix(Mat inputMatrix) {
            Mat residualMatrix(inputMatrix.rows, inputMatrix.cols, inputMatrix.type());

            for(int i=0; i<inputMatrix.rows; i++) {
                for(int j=0; j<inputMatrix.cols; j++) {
                    unsigned int a, b, c;
                    a = b = c = 0;

                    if (j > 0) {
                        a = (int) inputMatrix.at<uchar>(i, j-1);

                        if (i > 0) {
                            c = (int) inputMatrix.at<uchar>(i-1, j-1);
                        }
                    }
                    if (i > 0) {
                        b = (int) inputMatrix.at<uchar>(i-1, j);
                    }

                    unsigned int prediction = predict(a, b, c);

                    // cout << "a: " << a << " | b: " << b << " | c: " << c << endl;
                    // cout << "actual value: " << (int) inputMatrix.at<uchar>(i, j) << " | prediction: " << prediction << endl;
                    
                    residualMatrix.at<uchar>(i, j) = inputMatrix.at<uchar>(i, j) - prediction;
                }
            }

            // cout << residualMatrix << endl;

            return residualMatrix;
        }

        double predict(double a, double b, double c) {
            switch (this->mode) {
                case 1:
                    return a;
                    break;
                case 2:
                    return b;
                    break;
                case 3:
                    return c;
                    break;
                case 4:
                    return a + b - c;
                    break;
                case 5:
                    return a + (b-c)/2;
                    break;
                case 6:
                    return b + (a-c)/2;
                    break;
                case 7:
                    return (a+b)/2;
                    break;
            }
            
            return 0;
        }

        unsigned int getMode() {
            return this->mode;
        }
};