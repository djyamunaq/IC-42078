#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bitset>

class BitStream {
    private:
        std::fstream* file;
        unsigned int size;
    public:
        BitStream(char* fileName) {
            file = new std::fstream(fileName);

            if (!file) {
                std::cerr << "Unable to open file: " << fileName << std::endl;
            }

            // Move to the end of the file
            file->seekg(0, std::ios::end);
            // Get the position, which is the size in bytes             
            size = file->tellg();  
        }

        ~BitStream() {
            file->close();

            delete file;
        }

        int getBitAt(unsigned int pos) {
            if (pos >= 8*size) {
                std::cerr << "Not a valid bit position for the file provided" << std::endl;
                return -1;
            }

            // In which byte the bit is located
            unsigned int byteBand = floor(pos/8);
            unsigned int bitPosInByte = pos - 8*byteBand;

            // Create mask to get bit
            unsigned char mask = 0x80;
            mask = mask >> bitPosInByte;

            // Move reader to byte that contains bit of interest
            file->seekg(byteBand, std::ios::beg);

            // Read byte and apply bit mask
            unsigned char byte = file->get();

            //std::cout << std::bitset<8>(byte) << std::endl;
            //std::cout << std::bitset<8>(mask) << std::endl;

            return (byte & mask) == 0x00 ? 0 : 1;
        }

        void setBitAt(unsigned int pos, unsigned int value) {
            if (pos >= 8*size) {
                std::cerr << "Not a valid bit position for the file provided" << std::endl;
                return;
            }

            // In which byte the bit is located
            unsigned int byteBand = floor(pos/8);
            unsigned int bitPosInByte = pos - 8*byteBand;
            unsigned char mask = 0x80;
            mask = mask >> bitPosInByte;

            // Move reader to byte that contains bit of interest
            file->seekg(byteBand, std::ios::beg);
            // Read byte and apply bit mask
            char byte = file->get();
            // Reset bit of interest to 0
            byte &= ~mask;
            // Set the bit to the correct value
            mask = value ? 0x80 : 0x00;
            mask = mask >> bitPosInByte;
            byte |= mask;
            // Move writer to byte that contains bit of interest
            file->seekp(byteBand, std::ios::beg);
            // Write to the file
            file->write(&byte, sizeof(byte));
        }

        std::vector<unsigned char> getNBits(int N) {
            int lim = N-1;
            std::vector<unsigned char> buffer = std::vector<unsigned char>(N);

            for (int i=0; i<=lim; i++) {
                buffer[i] = getBitAt(i) ? '1' : '0';
            }

            return buffer;
        }

        void setNBits(std::vector<unsigned char> buffer) {
            for (int i=0; i<buffer.size(); i++) {
                unsigned int value = buffer[i] == '0' ? 0 : 1;
                setBitAt(i, value);
            }
        }

        std::fstream encode(std::fstream& fileIn) {
            // File to write
            std::fstream fileOut("encoded_file.txt", std::ios::out);

            if (!fileOut) {
                std::cerr << "Error opening the file for writing." << std::endl;
                return std::fstream();
            }

            // Set reader to the begining of the file
            fileIn.seekg(0, std::ios::end);
            int inSize = fileIn.tellg();
            fileIn.seekg(0, std::ios::beg);

            // 
            char byte = 0x00;
            int shift = 0;
            // Character in counter
            int i = 0;
            // Character out counter
            int j = 0;
            while (1) {
                // Read bit
                char bit = fileIn.get();

                if (bit == '1') {
                    byte = byte | (0x80 >> shift);
                } else if (bit == '0') {
                    // Do nothing
                } else {
                    std::cerr << "Bad character in text to be encoded: " << std::bitset<8>(bit) << std::endl;
                    return std::fstream();
                }

                // Go to next bit
                shift++;

                if (shift == 8) {
                    //std::cout << std::bitset<8>(byte) << std::endl;
                    fileOut.seekg(j++, std::ios::beg);
                    fileOut.write(&byte, sizeof(byte));
                    shift = 0;
                    byte = 0x00;
                } 

                i++;
                if (i == inSize) break;
                
                fileIn.seekg(i, std::ios::beg);
            }

            return fileOut;
        }

        std::fstream decode(std::fstream& fileIn) {
            // File to write
            std::fstream fileOut("decoded_file.txt", std::ios::out);

            if (!fileOut) {
                std::cerr << "Error opening the file for writing." << std::endl;
                return std::fstream();
            }

            // Set reader to the begining of the file
            fileIn.seekg(0, std::ios::end);
            int inSize = fileIn.tellg();
            fileIn.seekg(0, std::ios::beg);

            int mask = 0x80;
            int shift = 0;
            // Character in counter
            int i = 0;
            // Character out counter
            int j = 0;
            while (1) {
                // Read bit
                char byte = fileIn.get();

                std::cout << std::bitset<8>(byte) << std::endl;

                for (int k=0; k<8; k++) {
                    char bit = (byte & (mask >> k)) == 0 ? '0' : '1';
                    fileOut.seekg(j++, std::ios::beg);
                    fileOut.write(&bit, sizeof(bit));
                }

                i++;
                if (i == inSize) break;
                
                fileIn.seekg(i, std::ios::beg);
            }

            return fileOut;
        }
};