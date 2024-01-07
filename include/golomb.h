#include <iostream>
#include <bitset>
#include <math.h>
#include "bit_stream.h"

using std::cout;
using std::endl;
using std::string;
using std::bitset;
using std::log2;
using std::ceil;

class GolombCoding {
private:
    int m;
    bool useSignMagnitude;
    bool truncatedBinaryEncode = true;
    BitStream bitStream;

    string unaryEncode(int value) const {
        if (value == 0) return "0";
        return string(value, '1') + '0';
    }

    int unaryDecode(const string& encodedBits, int& pos) const {
        int count = 0;
        while (encodedBits[pos] == '1') {
            ++count;
            ++pos;
        }
        /* Skip the 0 */
        ++pos; 
        return count;
    }

    string binaryEncode(int value) const {
        int numBits = static_cast<int>(ceil(log2(m)));

        /* Truncated Binary Encoding */
        if (truncatedBinaryEncode) {
            if (value > pow(2, numBits) - m - 1) {
                return bitset<32>(value + pow(2, numBits) - m).to_string().substr(32 - numBits);
            } 
            else {
                return bitset<32>(value).to_string().substr(32 - numBits + 1);
            }
        }
        /* Binary Encoding */
        else {
            return bitset<32>(value).to_string().substr(32 - numBits + 1);
        }
    }

    int binaryDecode(const string& encodedBits, int& pos) const {
        int numBits = static_cast<int>(ceil(log2(m)));

        std::string binaryStr = encodedBits.substr(pos, encodedBits.size());
        binaryStr = string(encodedBits.size()-pos+1, '0') + binaryStr;
        // cout << binaryStr << endl;
        unsigned long value = std::bitset<32>(binaryStr).to_ulong();

        /* Truncated Binary Encoding */
        if (truncatedBinaryEncode) {
            if (value > pow(2, numBits) - m - 1) {
                return value - pow(2, numBits) + m;
            } else {
                return value;
            }
        }
        /* Binary Encoding */
        else {
            return value;
        }
    }

public:
    GolombCoding(int mValue, bool signMagnitude) : m(mValue), useSignMagnitude(signMagnitude) {
        if ((mValue > 0) && ((mValue & (mValue - 1)) == 0)) truncatedBinaryEncode = false;
    }

    /* Enconding function */
    char* encode(int x) {
        // cout << "m: " << m << endl;
        // cout << "value: " << x << endl;
        // cout << "truncated: " << truncatedBinaryEncode << endl;

        string code;

        /* Handle negative numbers based on user's choice */
        if (useSignMagnitude && x < 0) {
            x = -x;
            code = "1" + unaryEncode(x / m) + binaryEncode(x % m);
        } else if (useSignMagnitude) {
            code = "0" + unaryEncode(x / m) + binaryEncode(x % m);
        } else {
            code = unaryEncode(x / m) + binaryEncode(x % m);
        }

        for(char c: code) {
            unsigned int value = (unsigned int) (c - '0');

            this->bitStream.pushBit(value);                        
        }

        return code.data();
    }

    /* Decoding function */
    int decode(const std::string& encodedBits) const {
        int pos = 0;

        if (useSignMagnitude) pos++;
        int quotient = unaryDecode(encodedBits, pos);
        int remainder = binaryDecode(encodedBits, pos);

        // cout << "Q: " << quotient << endl;
        // cout << "R: " << remainder << endl;

        // Handle negative numbers based on user's choice
        if (useSignMagnitude && encodedBits[0] == '1') {
            return -((quotient * m) + remainder);
        } else {
            return (quotient * m) + remainder;
        }
    }

    void setM(int newM) {
        m = newM;
    }

    void setNegativeRepresentation(bool signMag) {
        useSignMagnitude = signMag;
    }

    BitStream getBitStream() {
        return this->bitStream;
    }
};