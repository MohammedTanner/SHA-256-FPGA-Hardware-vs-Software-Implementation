#include <iostream>
#include <fstream>
#include <hls_stream.h>
#include <hls_vector.h>
#include "ap_axi_sdata.h"
#include "sha256.h"

using namespace std;

#define N 64

int main() {
    hls::stream< ap_axiu<32,2,5,6> > stateREG;
    hls::stream< ap_axiu<8,2,5,6> > idata;
    hls::stream< ap_axiu<8,2,5,6> > ohash;

    // Initialize input data
    unsigned char input[N] = {0x61,0x62,0x63,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18};;
    for (int i = 0; i < N; i++) {
        ap_axiu<8,2,5,6> tmp;
        tmp.data = input[i];
        idata.write(tmp);
    }

   /* // Initialize stateREG, UNCOMMENT IF istateREG is added back
    WORD state[8] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };
    for (int i = 0; i < 8; i++) {
        ap_axiu<32,2,5,6> tmp;
        tmp.data = state[i];
        stateREG.write(tmp);
    } */

    // Call sha256 function
    // sha256(stateREG, idata, ohash);
    sha256(idata, ohash);

    // Get output hash
    unsigned char output[32];
    for (int i = 0; i < 32; i++) {
        ap_axiu<8,2,5,6> tmp;
        ohash.read(tmp);
        output[i] = tmp.data;
    }

    // Print output hash
    cout << "Output hash:" << endl;
    for (int i = 0; i < 32; i++) {
        cout << hex << (int) output[i];
    }
    cout << endl;

    return 0;
}
