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

        void compress(Mat imgInput, string outputPath) {            
            /* Check if image is empty */
            if (imgInput.empty()) {
                cerr << "[ERROR] Empty image" << endl;
                return;
            }

            /******************************************************************************/
            /* 1. SET PREDICTOR                                                           */
            /******************************************************************************/
            cout << "\t> Set Predictor..." << endl;
            int mode = 7;
            Predictor predictor(mode);
            
            Mat residualMatrix = predictor.calculateResidualMatrix(imgInput);

            // cout << imgInput.rows << " " << imgInput.cols << endl;
            // cout << residualMatrix.rows << " " << residualMatrix.cols << endl;

            /******************************************************************************/
            /* 2. ENCODE RESIDUALS                                                        */
            /******************************************************************************/
            cout << "\t> Encode Residuals..." << endl;

            int m = 7;

            /* Set golomb coding instance with sign treating */
            GolombCoding golombCoding(m, true);
            
            long long int max = 0;
            long long int min = 255;
            for (int i=0; i<residualMatrix.rows; i++) {
                for (int j=0; j<residualMatrix.rows; j++) {
                    int value = residualMatrix.at<uchar>(i, j);
                    golombCoding.encode(value);

                    if (value > max) max = value;
                    if (value < min) min = value;
                }
            }

            /******************************************************************************/
            /* 3. WRITE COMPRESSED DATA TO FILE                                           */
            /******************************************************************************/
            cout << "\t> Write to Output..." << endl;


            /* Open a file in writing mode */
            FILE *fptr;
            fptr = fopen(outputPath.c_str(), "w");

            if (fptr == NULL) {
                cerr << "[ERROR] Not possible to open input file in compression" << endl;
                exit(-1);
            }

            BitStream bitStream = golombCoding.getBitStream();

            /* Check if the file is open */
            /* Metadata header */
                /* Write predictor mode */
            fprintf(fptr, "%d ", mode);
                /* Write m for golomb decoding */
            fprintf(fptr, "%d ", m);
                /* Write num of rows */
            fprintf(fptr, "%d ", residualMatrix.rows);
                /* Write num of cols */
            fprintf(fptr, "%d ", residualMatrix.cols);
                /* Write number of bits in residual part */
            long long int sizeInBits = bitStream.getSizeInBits();
            fprintf(fptr, "%lld ", sizeInBits);
                /* Write number of bytes in residual part */
            long long int sizeInBytes = bitStream.getSizeInBytes();
            fprintf(fptr, "%lld\n", sizeInBytes);
            
            bitStream.resetBytePointerPosition();

            cout << "\t> Prediction mode: " << mode << endl;
            cout << "\t> Parameter m: " << m << endl;
            cout << "\t> Num Rows: " << residualMatrix.rows << endl;
            cout << "\t> Num Cols: " << residualMatrix.cols << endl;
            cout << "\t> Size in bits: " << sizeInBits << endl;
            cout << "\t> Size in bytes: " << sizeInBytes << endl;
            int totalBytes=0;
            while(bitStream.getBytePointerPosition() < sizeInBytes) {
                // cout << bitStream.getBytePointerPosition() << "\\" << sizeInBytes-1 << endl;
                char value = bitStream.getByte();
                
                // cout << "char: " << value << endl;
                // cout << "bits: " << bitset<8>(value) << endl;

                fprintf(fptr, "%c", value);
                bitStream.nextBytePointerPosition();
                
                totalBytes++;
            }
            cout << "\t> Bytes written: " << totalBytes << endl;

            bitStream.resetBytePointerPosition();

            /* Close the file */
            std::cout << "\t> Data has been written to the file.\n";

            // Close the file
            fclose(fptr);
        }

        void decompress(string inputPath, string outputPath) {
            /* Open file for reading */
            FILE *fptrInput, *fptrOutput;
            fptrInput = fopen(inputPath.c_str(), "r");
            fptrOutput = fopen(outputPath.c_str(), "w");

            if (fptrInput == NULL) {
                cerr << "[ERROR] Not possible to open input file in decompression" << endl;
                exit(-1);
            }
            if (fptrOutput == NULL) {
                cerr << "[ERROR] Not possible to open output file in decompression" << endl;
                exit(-1);
            }

            cout << "\t> Input file path: " << inputPath << endl;
            cout << "\t> Output file path: " << inputPath << endl;

            /* Metadata variables */
            unsigned int mode, m, nCols, nRows;
            long long int sizeInBits, sizeInBytes;

            /*  */
            fscanf(fptrInput, "%d", &mode);
            cout << "\t> Prediction mode: " << mode << endl;
            /*  */
            fscanf(fptrInput, "%d", &m);
            cout << "\t> Parameter m: " << m << endl;
            /*  */
            fscanf(fptrInput, "%d", &nRows);
            cout << "\t> Num Rows: " << nRows << endl;
            /*  */
            fscanf(fptrInput, "%d", &nCols);
            cout << "\t> Num Cols: " << nCols << endl;
            /*  */
            fscanf(fptrInput, "%lld", &sizeInBits);
            cout << "\t> Size in bits: " << sizeInBits << endl;
            /*  */
            fscanf(fptrInput, "%lld\n", &sizeInBytes);
            cout << "\t> Size in bytes: " << sizeInBytes << endl;

            /* Set golomb coding instance with sign treating */
            GolombCoding golombCoding(m, true);
            /* Mount code-value map */
            map<string, int> codeMap;
            for (int value=0; value<=255; value++) {
                string encodedBits = golombCoding.encode(value);
                codeMap[encodedBits] = value;
            }

            // for (auto k: codeMap) {
                // cout << k.first << ": " << k.second << endl; 
            // }

            long long int nBitsTotal = 0;
            char c;
            int bytesRead = 0;
            string buffer = "";

            /* Rebuild residual matrix */
            Mat residualMatrix = Mat(nRows, nCols, CV_8U);

            int row, col;
            row = col = 0;

            while (bytesRead < sizeInBytes) {
                // if (bytesRead == 15) return;
                bytesRead++;

                fscanf(fptrInput, "%c", &c);
                
                int nBits = 0;
                unsigned char mask = 0x80;

                // cout << "char: " << c << endl;
                // cout << "bits: " << bitset<8>(c) << endl;

                /* Add bits to buffer til end char */
                while (nBits < 8) {
                    unsigned char value = (mask & c) ? '1' : '0';
                    buffer += value;
                    nBits++;
                    nBitsTotal++;
                    mask = mask >> 1;

                    if (codeMap.count(buffer)) {
                        // cout << codeMap[buffer] << endl;
                        residualMatrix.at<uchar>(row, col) = codeMap[buffer];

                        col++;
                        if (col == nCols) {
                            col = 0;
                            row++;
                        }

                        buffer = "";
                    }

                    if (nBitsTotal == sizeInBits) {
                        cout << "\t> READ ALL BITS" << endl;
                        break;
                    } 
                }
            }

            /* Close the files */
            fclose(fptrInput);
            fclose(fptrOutput);

            cout << "\t> Total bytes read: " << sizeInBytes << endl;
            cout << "\t> Total bits read: " << nBitsTotal << endl;
            cout << "\t> Residual Matrix shape: (" << residualMatrix.rows << ", " << residualMatrix.cols << ")" << endl;

            Predictor predictor = Predictor(mode);
            Mat reconstructedImageMatrix = predictor.calculateActualMatrix(residualMatrix);
            cvtColor(reconstructedImageMatrix, reconstructedImageMatrix, COLOR_GRAY2RGB);

            bool isSuccess = imwrite(outputPath, reconstructedImageMatrix);

            cout << "\t> Original image matrix reconstructed" << endl;

            if (!isSuccess) {
                cerr << "[ERROR] Error writing image" << endl;
                return;
            }

        }
};