#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bitset>

using std::vector;
using std::cout;
using std::endl;

/* BitStream (FIFO) */
class BitStream {
    private:
        /* Size of stream in bits */
        unsigned int sizeInBits;
        /* Position of a pointer to the bits */
        unsigned int bitPosPointer;
        /* Position of a pointer to the bytes */
        unsigned int bytePosPointer;
        /* Stream of bits in byte blocks */
        vector<unsigned char> stream;  

        /* Set a bit in a byte */
        void setBitInByte(unsigned char &byte, unsigned int pos, unsigned int value) {
            /* Create mask to isolate bit */
            unsigned char mask = 0x80;
            mask = mask >> pos;
            
            /* Set the bit to zero */
            byte &= ~mask;

            /* Set the bit to the desired value */ 
            mask = value ? 0x80 : 0x00;
            mask = mask >> pos;
            byte |= mask;
        }

        /* Get a bit in a byte */
        unsigned int getBitInByte(unsigned char byte, unsigned int pos) {
            /* Create mask to isolate bit */
            unsigned char mask = 0x80;
            mask = mask >> pos;

            /* Set other bits to zero */
            byte &= mask;

            return byte ? 1 : 0;
        }

    public:
        BitStream() : sizeInBits(0), bitPosPointer(0), bytePosPointer(0) {
            stream.push_back(0x00);
        }

        ~BitStream() {
        }

        void printStreamInBytes() {
            cout << "Size in bytes: " << this->stream.size() << endl;
            cout << "Size in bits: " << this->sizeInBits << endl;
            for (unsigned char c: this->stream) {
                cout << c << " ";
            } cout << endl;
        }

        /* Set bit pointer position back to 0 */
        void resetBitPointerPosition() {
            this->bitPosPointer = 0;
        }

        /* Move pointer to bits to the next position */
        void nextBitPointerPosition() {
            this->bitPosPointer++;
        }

        /* Get pointer to bits position */
        unsigned int getBitPointerPosition() {
            return this->bitPosPointer;
        }

        /* Set bytes pointer position back to 0 */
        void resetBytePointerPosition() {
            this->bytePosPointer = 0;
        }

        /* Move pointer to bytes to the next position */
        void nextBytePointerPosition() {
            this->bytePosPointer++;
        }

        /* Get pointer to bytes position */
        unsigned int getBytePointerPosition() {
            return this->bytePosPointer;
        }

        /* Get byte at position indicated by pointer */
        unsigned char getByte() {
            return this->stream.at(this->bytePosPointer);
        }

        /* Get bit at position indicated by pointer */
        unsigned int getBit() {
            int bytePos = ceil(this->bitPosPointer/8);
            // cout << "\t" << "Byte pos: " << bytePos << endl;

            if (bytePos < 0) bytePos = 0; 

            unsigned int bitPos = this->bitPosPointer % 8;
            // cout << "\t" << "Bit pos: " << bitPos << endl;

            unsigned int bit = this->getBitInByte(this->stream.at(bytePos), bitPos);
            
            return bit;
        }

        /* Push bit to the end of the stream */
        void pushBit(unsigned int value) {
            /* Increse size of the stream in bits */
            this->sizeInBits++;
            /* Find byte where to push new bit */
            int bytePos = ceil((sizeInBits-1)/8);

            // cout << bytePos << endl;

            /* If bytePos > size of stream then add new byte to stream */
            if (bytePos > stream.size() - 1) {
                stream.push_back(0x00);
            }

            /* If there's no elements, go to first byte */
            if (bytePos < 0) bytePos = 0;

            /*  */
            unsigned int posInByte = (this->sizeInBits - 1) % 8;

            /* Set bit in byte */
            this->setBitInByte(this->stream.at(bytePos), posInByte, value);
        }

        unsigned int getSizeInBits() {
            return this->sizeInBits;
        }

        unsigned int getSizeInBytes() {
            return this->stream.size();
        }
};