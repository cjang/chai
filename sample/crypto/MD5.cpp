#include <chai/chai.h>
#include <chai/ParseArgs.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

using namespace chai;
using namespace std;

//
// MD5 hash algorithm from: http://en.wikipedia.org/wiki/MD5
// Original RSA code: http://people.csail.mit.edu/rivest/Md5.c
//

char hexChar(const uint8_t a)
{
    switch (a)
    {
        case (0) : return '0';
        case (1) : return '1';
        case (2) : return '2';
        case (3) : return '3';
        case (4) : return '4';
        case (5) : return '5';
        case (6) : return '6';
        case (7) : return '7';
        case (8) : return '8';
        case (9) : return '9';
        case (10) : return 'a';
        case (11) : return 'b';
        case (12) : return 'c';
        case (13) : return 'd';
        case (14) : return 'e';
        case (15) : return 'f';

        // should never happen
        default: return ' ';
    }
}

int main(int argc, char *argv[])
{
    /////////////////////////////////////
    // start Chai runtime

    ParseArgs(argc, argv).initVM();

    /////////////////////////////////////
    // create cleartext messages

    // cleartext
    vector< string > textBuf;

    // vector of cleartext messages
    // (all messages fit in a single 512 bit block for simplicity)
    textBuf.push_back("");
    textBuf.push_back("a");
    textBuf.push_back("abc");
    textBuf.push_back("message digest");
    textBuf.push_back("abcdefghijklmnopqrstuvwxyz");
    textBuf.push_back("The quick brown fox jumps over the lazy dog");
    textBuf.push_back("The quick brown fox jumps over the lazy dog.");

    /////////////////////////////////////
    // prepare message input

    // Note: The reference RSA implementation is more efficient. Input data
    // preparation and hashing are interleaved with each 512 bit block.
    // Separate stages is easier to understand and clearly separates the GPGPU
    // logic.

    // padded cleartext as integers in 512 bit blocks
    vector< vector< uint8_t > > padBuf;
    vector< vector< uint32_t > > wordBuf;

    // minimum number of 512 bit blocks
    size_t numBlocks = -1;

    for (size_t i = 0; i < textBuf.size(); i++)
    {
        // pad buffer
        padBuf.push_back( vector<uint8_t>() );

        // move text into pad buffer
        for (size_t j = 0; j < textBuf[i].size(); j++)
        {
            padBuf[i].push_back(textBuf[i][j]);
        }

        // pad cleartext with LF (fread does this)
        padBuf[i].push_back(0xa);

        // pad with ending bit
        padBuf[i].push_back(0x80);

        // zero padding
        const size_t padRem = padBuf[i].size() % 64;
        if (padRem + 8 <= 64)
        {
            // pad with zeros within last block to last 64 bits
            for (size_t j = padRem + 8; j < 64; j++)
                padBuf[i].push_back(0x0);
        }
        else
        {
            // pad with zeros to end of block
            for (size_t j = padRem; j < 64; j++)
                padBuf[i].push_back(0x0);

            // pad with zeros for another 448 bits (512 bits less 64 bits)
            for (size_t j = 0; j < 56; j++)
                padBuf[i].push_back(0x0);
        }

        // last 64 bits is original message length in bits
        // (plus LF pad character)
        const uint64_t origBits = (textBuf[i].size() + 1) * 8;
        const uint8_t *ptrLength = reinterpret_cast<const uint8_t*>(&origBits);
        for (size_t j = 0; j < 8; j++)
            padBuf[i].push_back(ptrLength[j]);

        // just take smallest number of 512 bit blocks
        const size_t n = padBuf[i].size() / 64;
        if (n < numBlocks)
            numBlocks = n;

        // very explicit conversion from 8 bit data to 32 bit words
        wordBuf.push_back( vector<uint32_t>() );
        for (size_t j = 0; j < padBuf[i].size(); j+=4)
        {
            const uint32_t b0 = padBuf[i][j];
            const uint32_t b1 = padBuf[i][j + 1];
            const uint32_t b2 = padBuf[i][j + 2];
            const uint32_t b3 = padBuf[i][j + 3];
            const uint32_t b = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
            wordBuf[i].push_back(b);
        }
    }

    /////////////////////////////////////
    // MD5 hash algorithm

    // specifies the per-round shift amounts
    const uint32_t r[] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    // binary integer part of the sines of integers (Radians) as constants
    const uint32_t k[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

    // big-endian byte order
    const uint32_t h0 = 0x67452301;
    const uint32_t h1 = 0xefcdab89;
    const uint32_t h2 = 0x98badcfe;
    const uint32_t h3 = 0x10325476;

    // number of cleartext messages
    const size_t N = wordBuf.size();

    uint32_t aGPUoutbuf[N], bGPUoutbuf[N], cGPUoutbuf[N], dGPUoutbuf[N];

    // iterate over blocks (note numBlocks is always 1 by design)
    for (size_t i = 0; i < numBlocks; i++)
    {
        // This is like transposing from row to column major.
        // The input is N rows, each with a 16 * 32 = 512 bit block.
        // For the GPU, want 16 rows and the 512 bit blocks in N columns.
        // Each row contains a piece of the N messages hashed in parallel.
        uint32_t cpuW[16 * N];
        for (size_t j = 0; j < N; j++)
        for (size_t y = 0; y < 16; y++)
            cpuW[y * N + j] = wordBuf[j][y + i * 16];

        //
        // GPU code begins here
        //

        // STL vector of array vector data (this could also be matrix data)
        vector< Arrayu32 > wGPU;
        for (size_t j = 0; j < 16; j++)
            wGPU.push_back( make1(N, &cpuW[j * N]) );

        Arrayu32 aGPU = h0 * ones_u32(N);
        Arrayu32 bGPU = h1 * ones_u32(N);
        Arrayu32 cGPU = h2 * ones_u32(N);
        Arrayu32 dGPU = h3 * ones_u32(N);

        Arrayu32 tempGPU;
        Arrayu32 fGPU;

        for (size_t j = 0; j < 64; j++)
        {
            size_t g;

            if (j <= 15)
            {
                fGPU = (bGPU & cGPU) | ((~ bGPU) & dGPU);
                g = j;
            }
            else if (j <= 31)
            {
                fGPU = (dGPU & bGPU) | ((~ dGPU) & cGPU);
                g = (5 * j + 1) % 16;
            }
            else if (j <= 47)
            {
                fGPU = bGPU ^ cGPU ^ dGPU;
                g = (3 * j + 5) % 16;
            }
            else
            {
                fGPU = cGPU ^ (bGPU | (~ dGPU));
                g = (7 * j) % 16;
            }

            tempGPU = dGPU;
            dGPU = cGPU;
            cGPU = bGPU;
            bGPU = bGPU + rotate(aGPU + fGPU + k[j] + wGPU[g], r[j]);
            aGPU = tempGPU;
        }

        aGPU.read1(aGPUoutbuf, N*sizeof(uint32_t));
        bGPU.read1(bGPUoutbuf, N*sizeof(uint32_t));
        cGPU.read1(cGPUoutbuf, N*sizeof(uint32_t));
        dGPU.read1(dGPUoutbuf, N*sizeof(uint32_t));

        //
        // GPU code ends here
        //
    }

    /////////////////////////////////////
    // stop Chai runtime

    shutdown();

    /////////////////////////////////////
    // assemble hashes

    vector< vector< uint32_t > > digest;
    for (size_t i = 0; i < textBuf.size(); i++)
    {
        digest.push_back( vector<uint32_t>() );

        digest[i].push_back(h0 + aGPUoutbuf[i]);
        digest[i].push_back(h1 + bGPUoutbuf[i]);
        digest[i].push_back(h2 + cGPUoutbuf[i]);
        digest[i].push_back(h3 + dGPUoutbuf[i]);
    }

    /////////////////////////////////////
    // print hashes

    cout << "Hashes should agree with md5sum: "
            "(e.g. echo \"abc\" | md5sum)"
         << endl;

    for (size_t i = 0; i < digest.size(); i++)
    {
        const uint8_t *ptrDigest = reinterpret_cast< const uint8_t* >(
                                       &digest[i][0] );

        for (size_t j = 0; j < 16; j++)
        {
            const uint8_t v = ptrDigest[j];
            cout << hexChar(v >> 4) << hexChar(v & 0xf);
        }

        cout << " \"" << textBuf[i] << "\"" << endl;
    }

    exit(0);
}
