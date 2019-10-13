/* 
   A C-program for MT19937-64 (2014/2/23 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Modified for use here by mcgrew.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using mt_init(seed)  
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, 2014, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and 
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and 
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/


#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "mt64.h"

#define NN 312
#define MM 156
#define MATRIX_A UINT64_C(0xB5026F5AA96619E9)
#define UM UINT64_C(0xFFFFFFFF80000000) /* Most significant 33 bits */
#define LM UINT64_C(0x7FFFFFFF) /* Least significant 31 bits */


/* The array for the state vector */
static uint64_t mt[NN]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

/* initializes mt[NN] with a seed */
void mt_init(uint64_t seed)
{
    mt[0] = seed;
    for (mti=1; mti<NN; mti++) {
        mt[mti] =  (UINT64_C(6364136223846793005) * 
                (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
    }
}


/* generates a random number on [0, 2^64-1]-interval */
static uint64_t genrand64_int64()
{
    int i;
    uint64_t x;
    static uint64_t mag01[2]={UINT64_C(0), MATRIX_A};

    if (mti >= NN) { /* generate NN words at one time */

        /* if mt_init() has not been called, */
        /* a default initial seed is used     */
        if (mti == NN+1) 
            mt_init(UINT64_C(5489)); 

        for (i=0;i<NN-MM;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&UINT64_C(1))];
        }
        for (;i<NN-1;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&UINT64_C(1))];
        }
        x = (mt[NN-1]&UM)|(mt[0]&LM);
        mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&UINT64_C(1))];

        mti = 0;
    }
  
    x = mt[mti++];

    x ^= (x >> 29) & UINT64_C(0x5555555555555555);
    x ^= (x << 17) & UINT64_C(0x71D67FFFEDA60000);
    x ^= (x << 37) & UINT64_C(0xFFF7EEE000000000);
    x ^= (x >> 43);

    return x;
}

uint64_t mt_rand(uint64_t min, uint64_t max)
{
    if (min >= max) return min;

    return min + (genrand64_int64() % (max - min + 1));
}

uint64_t mt_rand64()
{
    return genrand64_int64();
}

double mt_rand_double()
{
    return (genrand64_int64() >> 11) * (1.0/9007199254740992.0);
}


int mt_rand_bool()
{
    return genrand64_int64() & 0x1;
}

void mt_shuffle(void *array, size_t n, size_t size)
{
	char *tmp = malloc(size);
    char *arr = array;
    size_t i,j, stride = size * sizeof(char);

    if (n > 1) {
        for (i = 0; i < n - 1; ++i) {
            j = mt_rand(i, n-1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
	free(tmp);
}

