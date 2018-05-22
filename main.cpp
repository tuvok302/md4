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
    md4Digest digest = {IV_A,IV_B,IV_C,IV_D};;
    /*digest.A = IV_A;
    digest.B = IV_B;
    digest.C = IV_C;
    digest.D = IV_D;*/
    //cout << digest.A << " DIGEST.A" << endl;
    //uint32_t messageBlock [16];
    uint32_t key [3] = {K_0, K_1, K_2};
    if (stat(argv[1], &results) == 0){
        filesize = results.st_size * 8;
    }
    else {
        cout << "DANGER WILL ROBINSON! DANGER! Either file handle invalid, or ... something. GOOD LUCK Star Fox!";
    }

    //cout << filesize << endl;
    numBlocks = filesize >> 9;//set numBlocks to the filesize divided by blocksize, and increment the block size if the remainder is enough to force an additional block

    if ((filesize % 512) > 447) {
        paddingBlock = true;
    }

    ifstream fileToBeHashed (argv[1], ios::in | ios::binary);

    bool paddedBlock = false;
    bool lastBlock = false;
    for (uint32_t i = 0; i < numBlocks+1; i++){ //
        char buffer[64] = {0};
        uint32_t messageBlock [16] = {};
        fileToBeHashed.seekg(i * 64);
        fileToBeHashed.read(buffer,64);
        //cout << uint8_t(buffer) << "ASDFASDFASDFASDFASDF \r\n";
        if (!fileToBeHashed){ //note to self, data in buffer not in messageBlock
            buffer[fileToBeHashed.gcount()] = 0b10000000;
            //cout << digest.A << " DIGEST.A B4 first if" << endl;
            if(!paddingBlock){
                //cout << digest.A << " DIGEST.A b4 for" << endl;
                for (uint16_t j = fileToBeHashed.gcount()+1; j < (512 - 64) / 8; j++){
                    //cout << digest.A << " DIGEST.A b4 for " << i << endl;
                    buffer[j] = 0;
                }
                //cout << digest.A << " DIGEST.A after for" << endl;
                //cout << "FLIM FLAM FLANNERY " << buffer << "\n";
                //buffer[512 - 64] = lowOrderFix(filesize);
                lastBlock = true;
                //cout << "FLIM FLAM FLANNERY the returnening " << buffer << "\n";
                //cout << digest.A << " DIGEST.A !padding" << endl;
            }
            cout << numBlocks << endl;
            if(paddingBlock && !lastBlock){
                if(paddedBlock) {
                    cout << "INSIDE PADDEDBLOCK" << endl;
                    //numBlocks++;
                    for (uint16_t j = 0; j < (64); j++){
                        buffer[j] = 0;
                    }
                    //buffer[512 - 64] = lowOrderFix(filesize);
                    //paddedBlock = false;
                    //paddingBlock = false;
                    lastBlock = true;
                }
                if(!paddedBlock){
                    cout << "A#$%@#$%@#$%@#$ " << fileToBeHashed.gcount() << endl;
                    numBlocks++;
                    buffer[fileToBeHashed.gcount()] = 0b10000000;
                    for (uint16_t j = fileToBeHashed.gcount()+1; j < 64; j++){
                        buffer[j] = 0;
                    }
                    paddedBlock = true;
                }
            }
            //cout << fileToBeHashed.gcount() <<  "WHAT WHAT!" << endl;
            //cout << endl <<(uint32_t(uint8_t(buffer[0]))) << endl;
            //break; //IN CASE OF EMERGENCY GTFO
        }
        //cout << digest.A << " DIGEST.A - AFTER" << endl;
        //cout << endl << "messageBlock[0]: " << messageBlock[0] <<endl;
        //i = 0;
        for(int j = 0; j < 64; j++) { //tested, seems to work, since it operates on 64 8-bit registers (I was tired.  Please don't judge)
            /*if ((j % 32) == 0){
                //messageBlock[i] = endiannessFix(messageBlock[i]);
                //((messageBlock[i] & 0xff) << 24) | ((messageBlock[i] & 0xff00) << 8) | ((messageBlock[i] & 0xff0000) >> 8) | ((messageBlock[i] & 0xff000000) >> 24);
                i = j/32;
                cout << "JENKIES!\r\n";
                messageBlock[i] = 0;
            }/* //Something something fucking little endian
            if ((j % 64) == 0) {
                k = j/32;
                messageBlock[
            }*/
            //cout << "ASDFASDFASDF " << (uint32_t(uint8_t(buffer[j]))) << " ASDFASDFASDF" << endl;
            messageBlock[j/4] = (messageBlock[j/4] << 8) | uint8_t(buffer[j]);  //convert 8 bit int to 32 bit int by shifting left by
            //cout << "WIGGITY WOGGITY WOO " << (messageBlock[j/4] << 8) << " ASDFASDF " << (uint32_t(uint8_t(buffer[j]))) << endl;
            //cout << " buff:" << uint32_t(buffer[j]) << " mb:" << messageBlock[0] << " j; " << j << " i: " << i;                                                                                                //enough to position the bits in the correct spot.
        }
        if(lastBlock){
            cout << messageBlock[15] << " " << messageBlock[14] << endl;
            messageBlock[15] = endiannessFix(uint32_t(((filesize) & 0xffffffff00000000) >> 32));
            messageBlock[14] = endiannessFix(uint32_t((filesize) & 0x00000000ffffffff));
            cout << "ASDFASDFASDFASDF " << endl;
        }
        /*for(int j = 0; j < 4; j++){
            uint32_t temp_3 = messageBlock[j*4 + 3];
            uint32_t temp_2 = messageBlock[j*4 + 2];
            uint32_t temp_1 = messageBlock[j*4 + 1];
            uint32_t temp_0 = messageBlock[j*4 + 0];
            messageBlock[j*4 +0] = temp_3;
            messageBlock[j*4 +1] = temp_2;
            messageBlock[j*4 +2] = temp_1;
            messageBlock[j*4 +3] = temp_0;
        }*/
        //cout << endl << "messageBlock[0]: " << messageBlock[0] <<endl;
        //for (i = 0; i < 512; i++) {
            //cout << int(buffer[i]);//FILL THIS OUT WITH STUFF THAT HAPPENS WHEN YOU RUN OUT OF DATA (read: padding of the data)
        //}
        //cout << endl;
        md4Digest previousIter = digest; //backup the previous values of A,B,C,D
        //md4Digest previousIter;
        //previousIter = digest;
        //cout << digest.A << "THIS IS PREV ITER" << endl;
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
/*      Round 1: function F
                    [ABCD  0  3]  [DABC  1  7]  [CDAB  2 11]  [BCDA  3 19]
                    [ABCD  4  3]  [DABC  5  7]  [CDAB  6 11]  [BCDA  7 19]
                    [ABCD  8  3]  [DABC  9  7]  [CDAB 10 11]  [BCDA 11 19]
                    [ABCD 12  3]  [DABC 13  7]  [CDAB 14 11]  [BCDA 15 19]
        Round 2: function G
                    [ABCD 0 3]	[DABC 4 5]	[CDAB 8 9]	[BCDA 12 13]
                    [ABCD 1 3]	[DABC 5 5]	[CDAB 9 9]	[BCDA 13 13]
                    [ABCD 2 3]	[DABC 6 5]	[CDAB 10 9]	[BCDA 14 13]
                    [ABCD 3 3]	[DABC 7 5]	[CDAB 11 9]	[BCDA 15 13]
        Round 2: function H
                    [ABCD 0  3]	[DABC 8  9]	[CDAB 4  11]	[BCDA 12 15]
                    [ABCD 2  3]	[DABC 10 9]	[CDAB 6  11]	[BCDA 14 15]
                    [ABCD 1  3]	[DABC 9  9]	[CDAB 5  11]	[BCDA 13 15]
                    [ABCD 3  3]	[DABC 11 9]	[CDAB 7  11]	[BCDA 15 15]

*//*
                for (i = 0; i < 16; i++){
                    cout << messageOrder[i] << ",";
                }
                cout << endl;
                for (i = 0; i < 4; i++) {
                     cout << rollAmount[i %4] << ",";
                }
                cout << endl;*/
                uint32_t temp_B = 0;
                //std::cout << digest.A << " " << digest.B << " " << digest.C << " " << digest.D << " " << temp_B << endl;
                if (j == 0){
                    temp_B = ( (uint32_t(digest.A) + uint32_t(uint32_t(F(uint32_t(digest.B), uint32_t(digest.C), uint32_t(digest.D))) + uint32_t(key[j]) + uint32_t(endiannessFix(messageBlock[messageOrder[k]])))));
                    //cout << ( (temp_B) + 1) << endl;
                }
                if (j == 1) {
                    temp_B = ( (digest.A + (G(digest.B, digest.C, digest.D) + key[j] + endiannessFix(messageBlock[messageOrder[k]]))));
                }
                if (j == 2) {
                    temp_B = ( (digest.A + (H(digest.B, digest.C, digest.D) + key[j] + endiannessFix(messageBlock[messageOrder[k]]))));
                }
                //cout << temp_B << " " << key[j]<< endl;
                //cout << (uint32_t(digest.B)) << " " << (uint32_t((temp_B))) << " " << temp_B << endl;
                temp_B = RLL32(temp_B, rollAmount[(k % 4)]);
                //cout << temp_B << "THIS IS TEMP_B <<<<" << endl;
                //cout << (uint32_t(temp_B)) << " " << uint32_t(~(temp_B)) << " " << digest.A << endl;
                //std::cout << F(uint32_t(digest.B), uint32_t(digest.C), uint32_t(digest.D)) << endl;
                std::cout << "A:" << temp_B  << " B:" << digest.B << " C:" << digest.C << " D:" << digest.D << " j:" << j << " k:" << k << " message block" << endiannessFix(messageBlock[k]) << " filesize:" << lowOrderFix(filesize) << endl;//" temp_b:" << temp_B << " messageBlock[messageOrder[k]]:" << endl;//<< endiannessFix(messageBlock[messageOrder[k]]) << " rollAmount[(k % 4)]:" << rollAmount[(k % 4)] << " key[j]:" << key[j] << " " << " " << endl;
                digest.A = digest.D;
                digest.D = digest.C;
                digest.C = digest.B;
                digest.B = temp_B;
                //std::cout << digest.A << " " << digest.B << " " << digest.C << " " << digest.D << endl;

                //cout << "A" << k << " : " << digest.A << endl; //actual hashing occurs here
                //cout << "B" << k << " : " << digest.B << endl; //actual hashing occurs here
                //cout << "C" << k << " : " << digest.C << endl; //actual hashing occurs here
                //cout << "D" << k << " : " << digest.D << endl; //actual hashing occurs here
            }
        }
        digest.A = previousIter.A + digest.A;
        digest.B = previousIter.B + digest.B;
        digest.C = previousIter.C + digest.C;
        digest.D = previousIter.D + digest.D;
    }
    /*cout << "STRING" << endl;
    int ZZZ = F(0x0,0x0,0x100);
    cout << argv[1] << endl;
    string test = argv[1];
    test = md4Padding(test);
    cout << (test) << endl;
    char* ZZZZ = "a";
    //char  *ptr = NULL;
    char* test2 = "a1h3girqw";
    int ZZ12 = int(ZZZZ);
    cout << (int)(*test2) << test2[1]<< endl;*/
    //cout << filesize << endl;
    fileToBeHashed.close();
    cout << decToHex(endiannessFix(digest.A)) << decToHex(endiannessFix(digest.B)) << decToHex(endiannessFix(digest.C)) << decToHex(endiannessFix(digest.D))<< endl;
    //cout << digest.A << digest.B << digest.C << digest.D << endl; //actual hashing occurs here
    //cout << decToHex(digest.A) << endl;
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
