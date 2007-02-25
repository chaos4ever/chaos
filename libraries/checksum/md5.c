/* $Id$ */
/* Abstract: RSA Data Security, Inc., MD5 message-digest algorithm. */
/* Author: Per Lundberg <plundis@chaosdev.org>,
           Derived from the RSA Data Security, Inc. MD5
           Message-Digest Algorithm. */

/* Copyright 1991 RSA Data Security, Inc. */
/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include <checksum/checksum.h>
#include <memory/memory.h>

static void checksum_md5_transform (u32 state[4], const u8 block[64]);


/* Check endianess. */
/* FIXME: Have a define for the endianess instead of doing like this. */

#ifdef i386
#define encode memory_copy
#define decode memory_copy
#else /* i386 */

/* Encodes input into output. Assumes length is a multiple of 4. */

static void encode (u8 *output, const u32 *input, unsigned int length)
{
  unsigned int input_index, output_index;
  
  for (input_index = 0, output_index = 0; output_index < length;
       input_index++, output_index += 4) 
  {
    output[output_index + 0] = (u8)((input[input_index] >> 0)  & 0xFF);
    output[output_index + 1] = (u8)((input[input_index] >> 8)  & 0xFF);
    output[output_index + 2] = (u8)((input[input_index] >> 16) & 0xFF);
    output[output_index + 3] = (u8)((input[input_index] >> 24) & 0xFF);
  }
}

/* Decodes input into output. Assumes length is a multiple of 4. */

static void decode (u32 *output, const u8 *input, unsigned int length)
{
  unsigned int output_index, input_index;
  
  for (output_index = 0, input_index = 0; input_index < length; 
       output_index++, input_index += 4)
  {
    output[output_index] = ((((u32) input[input_index + 0]) << 0)  |
                            (((u32) input[input_index + 1]) << 8)  |
                            (((u32) input[input_index + 2]) << 16) |
                            (((u32) input[input_index + 3]) << 24);
  }
}

#endif /* i386 */

static unsigned char PADDING[64] = 
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions. */

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
/* FIXME: This could be an inline function on i386 (using rol) */

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation. */

#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (u32) (ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (u32) (ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (u32) (ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (u32) (ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}

/* MD5 initialization. Begins an MD5 operation, writing a new context. */

static void checksum_md5_init (checksum_md5_context_type *context)
{
  context->count[0] = context->count[1] = 0;
  
  /* Load magic initialization constants. */

  context->state[0] = 0x67452301;
  context->state[1] = 0xEFCDAB89;
  context->state[2] = 0x98BADCFE;
  context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
   operation, processing another message block, and updating the
   context. */

static void checksum_md5_update
  (checksum_md5_context_type *context, const u8 *input, 
   unsigned int input_length)
{
  unsigned int i, index, part_length;
  
  /* Compute number of bytes mod 64 */
  
  index = (unsigned int) ((context->count[0] >> 3) & 0x3F);
  
  /* Update number of bits */
  
  context->count[0] += ((u32) input_length << 3);

  if (context->count[0] < input_length << 3)
  {
    context->count[1]++;
  }
  context->count[1] += ((u32) input_length >> 29);
  
  part_length = 64 - index;
  
  /* Transform as many times as possible. */
  
  if (input_length >= part_length) 
  {
    memory_copy ((void *) &context->buffer[index], 
                 (const void *) input, part_length);
    checksum_md5_transform (context->state, context->buffer);
    
    for (i = part_length; i + 63 < input_length; i += 64)
    {
      checksum_md5_transform (context->state, &input[i]);
    }
    
    index = 0;
  }
  else
  {
    i = 0;
  }  

  /* Buffer remaining input */

  memory_copy ((void *) &context->buffer[index], (const void *) &input[i],
               input_length - i);
}

/* MD5 padding. Adds padding followed by original length. */

static void checksum_md5_pad (checksum_md5_context_type *context)
{
  unsigned char bits[8];
  unsigned int index, pad_length;

  /* Save number of bits */

  encode (bits, context->count, 8);
  
  /* Pad out to 56 mod 64. */

  index = (unsigned int) ((context->count[0] >> 3) & 0x3F);
  pad_length = (index < 56) ? (56 - index) : (120 - index);
  checksum_md5_update (context, PADDING, pad_length);
  
  /* Append length (before padding) */

  checksum_md5_update (context, bits, 8);
}
  
/* MD5 finalization. Ends an MD5 message-digest operation, writing the
   message digest and zeroizing the context. */

static void checksum_md5_final 
  (checksum_md5_digest_type *digest, checksum_md5_context_type *context)
{
  /* Do padding. */

  checksum_md5_pad (context);

  /* Store state in digest */

  encode (digest, context->state, 16);

  /* Zeroize sensitive information. */

  memory_set_u8 ((u8 *) context, 0, sizeof (checksum_md5_context_type));
}

/* MD5 basic transformation. Transforms state based on block. */

static void checksum_md5_transform (u32 state[4], const u8 block[64])
{
  u32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
  
  decode (x, block, 64);
  
  /* Round 1 */

#define S11 7
#define S12 12
#define S13 17
#define S14 22

  FF (a, b, c, d, x[ 0], S11, 0xD76AA478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xE8C7B756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070DB); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xC1BDCEEE); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xF57C0FAF); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787C62A); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xA8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xFD469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098D8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8B44F7AF); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xFFFF5BB1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895CD7BE); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6B901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xFD987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xA679438E); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49B40821); /* 16 */
  
  /* Round 2 */

#define S21 5
#define S22 9
#define S23 14
#define S24 20

  GG (a, b, c, d, x[ 1], S21, 0xF61E2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xC040B340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265E5A51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xE9B6C7AA); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xD62F105D); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xD8A1E681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xE7D3FBC8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21E1CDE6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xC33707D6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xF4D50D87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455A14ED); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xA9E3E905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xFCEFA3F8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676F02D9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8D2A4C8A); /* 32 */
  
  /* Round 3 */

#define S31 4
#define S32 11
#define S33 16
#define S34 23

  HH (a, b, c, d, x[ 5], S31, 0xFFFA3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771F681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6D9D6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xFDE5380C); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xA4BEEA44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4BDECFA9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xF6BB4B60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xBEBFBC70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289B7EC6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xEAA127FA); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xD4EF3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881D05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xD9D4D039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xE6DB99E5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1FA27CF8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xC4AC5665); /* 48 */
  
  /* Round 4 */

#define S41 6
#define S42 10
#define S43 15
#define S44 21

  II (a, b, c, d, x[ 0], S41, 0xF4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432AFF97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xAB9423A7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xFC93A039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655B59C3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8F0CCC92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xFFEFF47D); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845DD1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6FA87E4F); /* 57 */
  II (d, a, b, c, x[15], S42, 0xFE2CE6E0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xA3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4E0811A1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xF7537E82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xBD3AF235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2AD7D2BB); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xEB86D391); /* 64 */
  
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  
  /* Zeroize sensitive information. */

  memory_set_u8 ((u8 *) x, 0, sizeof (x));
}

/* Wrapper function that does everything. */

void checksum_md5 
  (void *input, unsigned int length, checksum_md5_digest_type *digest)
{
  checksum_md5_context_type context;

  checksum_md5_init (&context);
  checksum_md5_update (&context, input, length);
  checksum_md5_final (digest, &context);
}
