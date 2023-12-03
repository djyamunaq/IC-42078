#include <iostream>
#include <string>
#include "golomb.h"

using std::cout; 
using std::cerr; 
using std::endl;
using std::invalid_argument;
using std::stoi;

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "[Error] Use: " << argv[0] << " <m> <value>" << endl;
        return -1;
    }
    
    int m;
    try {
        m = stoi(argv[1]);
    } catch(invalid_argument e) {
        cerr << "[ERROR] Invalid type for m: " << argv[1] << endl; 
        return -1;
    }

    int value;
    try {
        value = stoi(argv[2]);
    } catch(invalid_argument e) {
        cerr << "[ERROR] Invalid type for value: " << argv[2] << endl; 
        return -1;
    }

    /* Create GolombCoding object with m = 3 and sign and magnitude for negative numbers */
    GolombCoding golomb(m, true); 

    std::string encodedBits = golomb.encode(value);
    int decodedValue = golomb.decode(encodedBits);

    std::cout << "Original Value: " << value << std::endl;
    std::cout << "Encoded Bits: " << encodedBits << std::endl;
    std::cout << "Decoded Value: " << decodedValue << std::endl;

    return 0;
}