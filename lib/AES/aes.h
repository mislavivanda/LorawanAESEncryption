/*
 ---------------------------------------------------------------------------
 Copyright (c) 1998-2008, Brian Gladman, Worcester, UK. All rights reserved.

 LICENSE TERMS

 The redistribution and use of this software (with or without changes)
 is allowed without the payment of fees or royalties provided that:

  1. source code distributions include the above copyright notice, this
     list of conditions and the following disclaimer;

  2. binary distributions include the above copyright notice, this list
     of conditions and the following disclaimer in their documentation;

  3. the name of the copyright holder is not used to endorse products
     built using this software without specific written permission.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue 09/09/2006

 This is an AES implementation that uses only 8-bit byte operations on the
 cipher state.
 */
#include <Arduino.h>
#ifndef AES_H
#define AES_H

// POTREBNO ZA UKLJUCIVANJE METODA IZ C file U .cpp FILE -> https://community.platformio.org/t/artificial-undefined-reference/12495/2
#ifdef __cplusplus
extern "C"
{
#endif

#if 1
#define AES_ENC_PREKEYED /* AES encryption with a precomputed key schedule  */
#endif

#define N_ROW 4
#define N_COL 4
#define N_BLOCK (N_ROW * N_COL)
#define N_MAX_ROUNDS 14

    typedef unsigned char uint_8t;

    typedef uint_8t return_type;

    /*  Warning: The key length for 256 bit keys overflows a byte
        (see comment below)
    */

    typedef uint_8t length_type;

    typedef struct
    {
        uint_8t ksch[(N_MAX_ROUNDS + 1) * N_BLOCK];
        uint_8t rnd;
    } aes_context;

    /*  The following calls are for a precomputed key schedule

        NOTE: If the length_type used for the key length is an
        unsigned 8-bit character, a key length of 256 bits must
        be entered as a length in bytes (valid inputs are hence
        128, 192, 16, 24 and 32).
    */
    extern void xor_block(void *d, const void *s);

#if defined(AES_ENC_PREKEYED) || defined(AES_DEC_PREKEYED)
    // extern -> PROBLEM S UKLJUCIVANJEM C METODA U C++
    extern return_type aes_set_key(const unsigned char key[],
                                   length_type keylen,
                                   aes_context ctx[1]);
#endif

#if defined(AES_ENC_PREKEYED)

    extern return_type aes_encrypt(const unsigned char in[N_BLOCK],
                                   unsigned char out[N_BLOCK],
                                   const aes_context ctx[1]);
#endif
#ifdef __cplusplus
}
#endif

#endif
