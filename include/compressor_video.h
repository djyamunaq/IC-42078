#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <assert.h>
#include <filesystem>
#include <map>
#include <filesystem>
#include <bitset>
#include "predictor.h"
#include "golomb.h"

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
using cv::imread;
using cv::imshow;
using cv::waitKey;
using cv::Rect;
using cv::COLOR_BGR2GRAY;
using cv::COLOR_GRAY2RGB;
using std::to_string;
using std::map;
using std::filesystem::path;
using std::bitset;

class Compressor {
    private:

    public:
        Compressor() {
        }

        ~Compressor() {
        }

        void compress(string inputPath, string outputPath) {    
            /******************************************************************************/
            /* 1. SET PREDICTOR                                                           */
            /******************************************************************************/ 
            FILE* outputFile = fopen(outputPath.c_str(), "w");

            if (outputFile == NULL) {
                cerr << "[ERROR] Not possible to open input file in compression" << endl;
                exit(-1);
            }

            /******************************************************************************/
            /* 1. SET PREDICTOR                                                           */
            /******************************************************************************/
            cout << "\t> Set Predictor..." << endl;
            int mode = 7;
            int m = 7;

            Predictor predictor(mode);
            
            /******************************************************************************/
            /* 2. ITERATE THROUGH VIDEO FRAMES                                            */
            /******************************************************************************/
            cout << "\t> Compressing file..." << endl;
            
            cv::VideoCapture cap(inputPath);
            
            int fps = (int) cap.get(cv::CAP_PROP_FPS);
            int w = (int) cap.get(cv::CAP_PROP_FRAME_WIDTH);
            int h = (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT);
            fprintf(outputFile, "%d %d %d %d %d\n", mode, m, fps, w, h);

            /* Check if the video file is opened successfully */
            if (!cap.isOpened()) {
                std::cerr << "[ERROR] Problem opening the video file!" << std::endl;
                exit(-1);
            }

            /* Loop through frames */
            while (true) {
                /* Read a frame from the video */
                cv::Mat frame;
                cap >> frame;

                /* Check if the frame is empty (end of video) */
                if (frame.empty()) {
                    cout << "\t> End of video!" << endl;
                    break;
                }

                Mat residualMatrix = predictor.calculateResidualMatrix(frame);

                /******************************************************************************/
                /* 3. ENCODE RESIDUALS                                                        */
                /******************************************************************************/
                /* Set golomb coding instance with sign treating */
                GolombCoding golombCoding(m, true);
                
                for (int i=0; i<residualMatrix.rows; i++) {
                    for (int j=0; j<residualMatrix.rows; j++) {
                        int value = residualMatrix.at<uchar>(i, j);
                        golombCoding.encode(value);
                    }
                }

                /******************************************************************************/
                /* 4. WRITE TO OUTPUT                                                         */
                /******************************************************************************/
                BitStream bitStream = golombCoding.getBitStream();

                /* Write number of bits in residual part */
                long long int sizeInBits = bitStream.getSizeInBits();
                long long int sizeInBytes = bitStream.getSizeInBytes();
                fprintf(outputFile, "%lld %lld\n", sizeInBits, sizeInBytes);

                int totalBytes=0;
                bitStream.resetBytePointerPosition();
                while(bitStream.getBytePointerPosition() < sizeInBytes) {
                    // cout << bitStream.getBytePointerPosition() << "\\" << sizeInBytes-1 << endl;
                    char value = bitStream.getByte();
                    
                    // cout << "char: " << value << endl;
                    // cout << "bits: " << bitset<8>(value) << endl;

                    fprintf(outputFile, "%c", value);
                    bitStream.nextBytePointerPosition();
                    
                    totalBytes++;
                }
                /* Skip line for new frame */
                fprintf(outputFile, "\n");
                bitStream.resetBytePointerPosition();

                /* Do something with the frame (e.g., display it) */
                // cv::imshow("Video Frame", frame);

                /* Break the loop if the user presses 'ESC' */
                // if (cv::waitKey(30) == 27) {
                    // break;
                // }
            }

            fclose(outputFile);

            cout << "\t> File compressed" << endl;;
        }

        void decompress(string inputPath, string outputPath) {
            /* Open file for reading */
            FILE *fptrInput;
            fptrInput = fopen(inputPath.c_str(), "r");

            if (fptrInput == NULL) {
                cerr << "[ERROR] Not possible to open input file in decompression" << endl;
                exit(-1);
            }

            cout << "\t> Input file path: " << inputPath << endl;
            cout << "\t> Output file path: " << inputPath << endl;

            /* Metadata variables */
            unsigned int mode, m;
            int fps, w, h;
            /*  */
            fscanf(fptrInput, "%d %d %d %d %d\n", &mode, &m, &fps, &w, &h);
            cout << "\t> Prediction mode: " << mode << endl;
            cout << "\t> Parameter m: " << m << endl;
            cout << "\t> FPS: " << fps << endl;

            /* Video writer */
            cv::VideoWriter videoWriter(outputPath, 0, fps, cv::Size(h, w), true);

            /* Set golomb coding instance with sign treating */
            GolombCoding golombCoding(m, true);

            /* Mount code-value map */
            map<string, int> codeMap;
            for (int value=0; value<=255; value++) {
                string encodedBits = golombCoding.encode(value);
                codeMap[encodedBits] = value;
            }
            
            while (true) {
                /* Rebuild residual matrix */
                Mat residualMatrix = Mat(h, w, CV_8U);

                int row, col, nBits;
                row = col = 0;

                long long int sizeInBits, sizeInBytes;
                long long int bitsRead = 0;
                char c;
                string buffer = "";

                fscanf(fptrInput, "%lld %lld\n", &sizeInBits, &sizeInBytes);
                while (bitsRead < sizeInBits) {
                    fscanf(fptrInput, "%c", &c);
                    
                    unsigned char mask = 0x80;

                    /* Add bits to buffer til end char */
                    while (nBits < 8) {
                        unsigned char value = (mask & c) ? '1' : '0';
                        buffer += value;
                        bitsRead++;
                        nBits++;
                        mask = mask >> 1;

                        if (codeMap.count(buffer)) {
                            // cout << codeMap[buffer] << endl;
                            residualMatrix.at<uchar>(row, col) = codeMap[buffer];

                            col++;
                            if (col == w) {
                                col = 0;
                                row++;
                            }

                            buffer = "";
                        }

                        if (bitsRead == sizeInBits) {
                            nBits = 9;
                        } 
                    }
                }

                Predictor predictor = Predictor(mode);
                Mat reconstructedImageMatrix = predictor.calculateActualMatrix(residualMatrix);

                videoWriter.write(reconstructedImageMatrix);
            }

            /* Close the files */
            fclose(fptrInput);
            videoWriter.release();
        }
};