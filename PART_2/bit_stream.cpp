#include "bit_stream.h"

int main(int argc, char *argv[]) {
    if(argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <input file> <file to encode>\n";
		return 1;
	}

    BitStream bitStream(argv[1]);
    
    for (int i=0; i<32; i++) {
        std::cout << bitStream.getBitAt(i);
        if (i % 8 == 7) std::cout << " ";
    } std::cout << std::endl;

    bitStream.setBitAt(19, 1);
    
    for (int i=0; i<32; i++) {
        std::cout << bitStream.getBitAt(i);
        if (i % 8 == 7) std::cout << " ";
    } std::cout << std::endl;

    for (int i=0; i<32; i++) {
        std::cout << bitStream.getBitAt(i);
        if (i % 8 == 7) std::cout << " ";
    } std::cout << std::endl;

    std::vector<unsigned char> bufferOut = bitStream.getNBits(13);

    std::cout << bufferOut.data() << std::endl; 

    std::vector<unsigned char> bufferIn = {'0', '1', '1', '0', '0', '1', '0', '1'}; 
    bitStream.setNBits(bufferIn);

    for (int i=0; i<32; i++) {
        std::cout << bitStream.getBitAt(i);
        if (i % 8 == 7) std::cout << " ";
    } std::cout << std::endl;

    std::fstream fileToEncode = std::fstream(argv[2]);
    std::fstream encodedFile = bitStream.encode(fileToEncode);
    std::fstream decodedFile = bitStream.decode(encodedFile);

    fileToEncode.close();
    encodedFile.close();
    decodedFile.close();

    return 0;
}
