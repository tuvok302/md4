#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <fstream>
/*
#define IV_A	0x10325476
#define IV_B	0x98badcfe
#define IV_C	0xefcdab89
#define IV_D	0x67452301
*/
#define IV_A	0x67452301
#define IV_B	0xefcdab89
#define IV_C	0x98badcfe
#define IV_D	0x10325476

#define F(X, Y, Z) (((X) & (Y)) | ((~(X)) & (Z)))
#define G(X, Y, Z) (((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))
#define H(X, Y, Z) (((X) ^ (Y) ^ (Z)))
#define RLL32(X, C) (((X) << (C)) | ((X) >> ((32) - (C))))

#define K_0 0x00000000
#define K_1	0x5A827999
#define K_2	0x6ED9EBA1

#define BLOCKSIZE 512
#define DIGESTSIZE 128

std::string md4Padding(std::string inputString);
std::string decToHex(uint32_t value);
uint32_t endiannessFix(uint32_t value);
uint64_t lowOrderFix(uint64_t value);

struct md4Digest {
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
};

using namespace std;

int main(int argc, char *argv[])
{
    //Create a 512 bit struct, with 4 128 bit objects that are made of 4 32 bit objects
    //the 32 bit objects are A,B,C,D for each hash step, the 128 bit object is everything for 1 full hashing stage
    //read from a file as raw binary until either 512 bit struct is full or I run out of file.  If 1: perform hashing process
    //if 2: perform padding, then perform hashing and return result
    uint64_t filesize; //64 bit because MD4 spec
    struct stat results; //blindly following instructions for file size
    uint32_t numBlocks;
    bool paddingBlock = false;
    md4Digest digest = {IV_A,IV_B,IV_C,IV_D};

    uint32_t key [3] = {K_0, K_1, K_2};

    if (stat(argv[1], &results) == 0){
        filesize = results.st_size * 8;
    }
    else {
        cout << "DANGER WILL ROBINSON! DANGER! Either file handle invalid, or ... something. GOOD LUCK Star Fox!";
    }

    numBlocks = filesize >> 9;//set numBlocks to the filesize divided by blocksize, and increment the block size if the remainder is enough to force an additional block

    if ((filesize % 512) > 447) {   //need to iterate through the empty block if
        paddingBlock = true;        //there exists a padding block
        numBlocks++;
    }

    ifstream fileToBeHashed (argv[1], ios::in | ios::binary);

    bool paddedBlock = false; //used for program flow control
    bool lastBlock = false;

    for (uint32_t i = 0; i < numBlocks+1; i++){ //iterate through hashing function loop
        char buffer[64] = {0}; //holds file input
        uint32_t messageBlock [16] = {}; //blocks of message data that are used in hashing
        fileToBeHashed.seekg(i * 64);
        fileToBeHashed.read(buffer,64);
        if (!fileToBeHashed){ //note to self, data in buffer not in messageBlock
            buffer[fileToBeHashed.gcount()] = 0b10000000; //magic constant for padding, needs fixed
            if(!paddingBlock){//if there isn't a padding block pad with all 0's
                for (uint16_t j = fileToBeHashed.gcount()+1; j < (512 - 64) / 8; j++){
                    buffer[j] = 0;
                }
                lastBlock = true;
            }
            if(paddingBlock && !lastBlock){//if there is a padding block need to first determine if we're in the padding block or not and act accordingly
                if(paddedBlock) { //if we'd in the padded block it's all zero's except for the last 64 bits
                    for (uint16_t j = 0; j < (64); j++){
                        buffer[j] = 0;
                    }
                    lastBlock = true;
                }
                if(!paddedBlock){//if we're not we append a 1 then all 0's
                    buffer[fileToBeHashed.gcount()] = 0b10000000; //magic constant for padding, needs fixed
                    for (uint16_t j = fileToBeHashed.gcount()+1; j < 64; j++){
                        buffer[j] = 0;
                    }
                    paddedBlock = true;
                }
            }
        }
        //64 8-bit ints in buffer; so we fix the ordering to little endian and shove them into message blocks
        for(int j = 0; j < 64; j++) { //tested, seems to work, since it operates on 64 8-bit registers (I was tired.  Please don't judge)
            messageBlock[j/4] = (messageBlock[j/4] << 8) | uint8_t(buffer[j]);  //convert 8 bit int to 32 bit int by shifting left by
        }
        //if we're in the last block append the file size.  If it's >64 bit we just ignore any higher order bits
        //because of spec it's appended low order first
        //hard coded magic constants; need fixed
        if(lastBlock){
            messageBlock[15] = endiannessFix(uint32_t(((filesize) & 0xffffffff00000000) >> 32));
            messageBlock[14] = endiannessFix(uint32_t((filesize) & 0x00000000ffffffff));
        }

        md4Digest previousIter = digest; //backup the previous values of A,B,C,D by spec

        for(int j = 0; j < 3; j++) { //add j<3 to command arguments; 3 replace with NUM_ROUNDS, 16 with NUM_OPERATIONS
            int messageOrder [16];
            int rollAmount [4];
            if (j == 0) {
                int messageOrder_0 [16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15};
                int rollAmount_0 [4] = {3, 7, 11, 19};
                std::copy(messageOrder_0, messageOrder_0+16, messageOrder);
                std::copy(rollAmount_0, rollAmount_0+4, rollAmount);
                //rollAmount = rollAmount_0;
            }
            if (j == 1) {
                int messageOrder_1 [16] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6 ,10, 14, 3, 7, 11, 15};
                int rollAmount_1 [4] = {3, 5, 9, 13};
                std::copy(messageOrder_1, messageOrder_1+16, messageOrder);
                std::copy(rollAmount_1, rollAmount_1+4, rollAmount);
            }
            if (j == 2) {
                int messageOrder_2 [16] = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
                int rollAmount_2 [4] = {3, 9, 11, 15};
                std::copy(messageOrder_2, messageOrder_2+16, messageOrder);
                std::copy(rollAmount_2, rollAmount_2+4, rollAmount);
            }
            for(int k = 0; k < 16; k++) { //and now, to hash
                uint32_t temp_B = 0;
                if (j == 0){ //casting not required; needs fixed
                    temp_B = ( (uint32_t(digest.A) + uint32_t(uint32_t(F(uint32_t(digest.B), uint32_t(digest.C), uint32_t(digest.D))) + uint32_t(key[j]) + uint32_t(endiannessFix(messageBlock[messageOrder[k]])))));
                }
                if (j == 1) {
                    temp_B = ( (digest.A + (G(digest.B, digest.C, digest.D) + key[j] + endiannessFix(messageBlock[messageOrder[k]]))));
                }
                if (j == 2) {
                    temp_B = ( (digest.A + (H(digest.B, digest.C, digest.D) + key[j] + endiannessFix(messageBlock[messageOrder[k]]))));
                }
                temp_B = RLL32(temp_B, rollAmount[(k % 4)]);
                //std::cout << "A:" << temp_B  << " B:" << digest.B << " C:" << digest.C << " D:" << digest.D << " j:" << j << " k:" << k << " message block" << endiannessFix(messageBlock[k]) << " filesize:" << lowOrderFix(filesize) << endl;//" temp_b:" << temp_B << " messageBlock[messageOrder[k]]:" << endl;//<< endiannessFix(messageBlock[messageOrder[k]]) << " rollAmount[(k % 4)]:" << rollAmount[(k % 4)] << " key[j]:" << key[j] << " " << " " << endl;
                digest.A = digest.D;
                digest.D = digest.C;
                digest.C = digest.B;
                digest.B = temp_B;
                //std::cout << digest.A << " " << digest.B << " " << digest.C << " " << digest.D << endl;
            }
        }
        digest.A = previousIter.A + digest.A;
        digest.B = previousIter.B + digest.B;
        digest.C = previousIter.C + digest.C;
        digest.D = previousIter.D + digest.D;
    }
    fileToBeHashed.close();
    cout << decToHex(endiannessFix(digest.A)) << decToHex(endiannessFix(digest.B)) << decToHex(endiannessFix(digest.C)) << decToHex(endiannessFix(digest.D))<< endl;
    return 0;
}

std::string decToHex(uint32_t value){
    string hex = "";
    while(value > 0){
        if ((value % 16) < 10){
            hex += (value % 16) + 48;
        }
        if ((value % 16) > 9) {
            hex += (value % 16) + 55;
        }
        value = value >> 4;
    }
    string endian_fix = "";
    for(uint8_t i = 1; i <= hex.length(); i++){
        endian_fix += hex[hex.length() - i];
    }
    return endian_fix;
}

uint32_t endiannessFix(uint32_t value){
    return (((value & 0xff) << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | ((value & 0xff000000) >> 24));
}

uint64_t lowOrderFix(uint64_t value){
    return (((value & 0x00000000ffffffff) << 32) | ((value & 0xffffffff00000000) >> 32));
}

std::string md4Padding(std::string inputString){
    int paddingLength = 448 - (inputString.size() % BLOCKSIZE);
    cout << paddingLength << "  " << inputString.size() << endl;
    if (paddingLength < 0){
        paddingLength = paddingLength + 512; //If it's between 448 and 512, a whole 512 0's needs to be appended
        }
    string paddingString = "1";
    while(paddingLength > 0){
        paddingString.append("0");
        paddingLength--;
        }
    return inputString.append(paddingString);
    }
