#include <iostream>
#include <string>
#include "golomb.h"

using std::cout; 
using std::cerr; 
using std::endl;
using std::invalid_argument;
using std::stoi;
using std::string;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "[Error] Use: " << argv[0] << " <m> <value>*" << endl << "\t*Optional integer value to be encoded" << endl;
        return -1;
    }
    
    int m;
    try {
        m = stoi(argv[1]);
    } catch(invalid_argument e) {
        cerr << "[ERROR] Invalid type for m: " << argv[1] << endl; 
        return -1;
    }

    /* Create GolombCoding object with sign and magnitude for negative numbers */
    GolombCoding golomb(m, true); 

    /* If user provides output */
    if (argc > 2) {
        int value;
        try {
            value = stoi(argv[2]);
        } catch(invalid_argument e) {
            cerr << "[ERROR] Invalid type for value: " << argv[2] << endl; 
            return -1;
        }

        string encodedBits = golomb.encode(value);
        int decodedValue = golomb.decode(encodedBits);

        cout << "Original Value: " << value << endl;
        cout << "Encoded Bits: " << encodedBits << endl;
        cout << "Decoded Value: " << decodedValue << endl;
    } 
    /* Default behavior: enconde and output 0 to 20 */
    else {
        for (int value=0; value<=20; value++) {
            string encodedBits = golomb.encode(value);
            int decodedValue = golomb.decode(encodedBits);

            cout << "Original Value: " << value << std::endl;
            cout << "Encoded Bits: " << encodedBits << std::endl;
            cout << "Decoded Value: " << decodedValue << std::endl << endl;
        }
    }
    
    return 0;
}