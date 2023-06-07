/*********************************************************************
* Filename:   sha256.c
              This implementation uses little endian byte order.
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "sha256.h"
#include <hls_stream.h>
#include <hls_vector.h>
#include "ap_axi_sdata.h"
#include "ap_int.h"
using namespace std;

/*************************** DATA TYPES *****************************/



/*********************** FUNCTION DEFINITIONS ***********************/
// For two input + one output
// void sha256(hls::stream< ap_axiu<32,2,5,6> > &istateREG,hls::stream< ap_axiu<8,2,5,6> > &idata,hls::stream< ap_axiu<8,2,5,6> > &ohash)
void sha256(hls::stream< ap_axiu<8,2,5,6> > &idata,hls::stream< ap_axiu<8,2,5,6> > &ohash)
{
#pragma HLS INTERFACE axis port=idata
#pragma HLS INTERFACE axis port=ohash
#pragma hls interface s_axilite port=return

	//ap_axiu<32,2,5,6> tmp32;
	ap_axiu<8,2,5,6> tmp8;
	WORD stateREG[8]= {
	        0x6a09e667,
	        0xbb67ae85,
	        0x3c6ef372,
	        0xa54ff53a,
	        0x510e527f,
	        0x9b05688c,
	        0x1f83d9ab,
	        0x5be0cd19
	    };
	BYTE data[64];
	BYTE hash[32];

	/* load_State:for(int n=0; n < 8; n++){

		istateREG.read(tmp32);
		stateREG[n] = tmp32.data;

	} */
	load_data:for(unsigned char n=0; n<64;n++){
#pragma HLS UNROLL

			idata.read(tmp8);
			data[n] = tmp8.data;

		};
	WORD a, b, c, d, e, f, g, h, t1, t2, m[64];
	unsigned char i , j;
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=m

	convert_to_words:for (i = 0, j = 0; i < 16; ++i, j += 4){
#pragma HLS UNROLL factor=2

m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
	}; // data has 64 bytes, convert them to WORDS and store in m

	create_schedule:for ( ; i < 64; ++i){
#pragma HLS PIPELINE

m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16]; // create the rest of the schedule
	};

	a = stateREG[0];
	b = stateREG[1];
	c = stateREG[2];
	d = stateREG[3];
	e = stateREG[4];
	f = stateREG[5];
	g = stateREG[6];
	h = stateREG[7];

//compression
	compression:for (i = 0; i < 64; ++i) {
#pragma HLS PIPELINE II=2
		t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i]; // intermediate hash value 1
		t2 = EP0(a) + MAJ(a,b,c); // intermediate hash value 2
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

//final addition

	stateREG[0] += a;
	stateREG[1] += b;
	stateREG[2] += c;
	stateREG[3] += d;
	stateREG[4] += e;
	stateREG[5] += f;
	stateREG[6] += g;
	stateREG[7] += h;

	WORD r;

	//this shouldn't be synthesized
	// Since this implementation uses little endian byte ordering and SHA uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	convert_endianess:for (r = 0; r < 4; ++r) {
#pragma HLS PIPELINE II=2
  //parallelized
		hash[r]      = (stateREG[0] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 4]  = (stateREG[1] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 8]  = (stateREG[2] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 12] = (stateREG[3] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 16] = (stateREG[4] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 20] = (stateREG[5] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 24] = (stateREG[6] >> (24 - r * 8)) & 0x000000ff;
		hash[r + 28] = (stateREG[7] >> (24 - r * 8)) & 0x000000ff;
	}

	store_hash:for(int n=0; n < 32; n++){

			tmp8.data = hash[n];
			cout << "hash = ";
			cout << tmp8.data << endl;
			ohash.write(tmp8);

			}


}
