/*  Copyright (c) 2006-2008, Philip Busch <philip@0xe3.com>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * Base64 implementation.
 * @ingroup base64
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base64.h"

/** Encode a minimal memory block. This function encodes a minimal memory area
 *  of three bytes into a printable base64-format sequence of four bytes.
 *  It is mainly used in more convenient functions, see below.
 *
 * @attention This function can't check if there's enough space at the memory
 *            memory location pointed to by \c out, so be careful.
 *
 * @param out pointer to destination
 * @param in pointer to source
 * @param len input size in bytes (between 0 and 3)
 * @param charset the base64 character set to use (e.g. urlsafe, regular)
 * @returns nothing
 *
 * @ingroup base64
 */

void base64_encode_block(unsigned char out[4], const unsigned char in[3], const size_t len, const char charset[])
{
  out[0] = charset[in[0] >> 2];
  out[1] = charset[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];

  if (len > 1)
  {
    unsigned char in_2 = 0;
    if (len > 2)
    {
      in_2 = in[2];
      out[3] = charset[in_2 & 0x3f];
    }
    out[2] = charset[((in[1] & 0x0f) << 2) | ((in_2 & 0xc0) >> 6)];
  }
}

/** Decode a minimal memory block. This function decodes a minimal memory area
 *  of four bytes into its decoded equivalent. It is mainly used in more
 *  convenient functions, see below.
 *
 * @attention This function can't check if there's enough space at the memory
 *            memory location pointed to by \c out, so be careful.
 *
 * @param out pointer to destination
 * @param in pointer to source
 * @param len input size in bytes (between 1 and 4)
 * @returns -1 on error (illegal character) or the number of bytes decoded
 *
 * @ingroup base64
 */
int base64_decode_block(unsigned char out[3], const unsigned char in[4], const size_t len)
{
  int i, numbytes = len - 1;
  char tmp[4];

  for (i = len - 1; i >= 0; i--)
  {
    if (in[i] == '=')
    {
      tmp[i] = 0;
      numbytes = i - 1;
    }
    else
    {
      tmp[i] = base64_index[in[i]];
    }

    if (tmp[i] == XX)
    {
      return -1;
    }
  }

  out[0] = (unsigned char)(tmp[0] << 2 | tmp[1] >> 4);
  out[1] = (unsigned char)(tmp[1] << 4 | tmp[2] >> 2);
  if (len > 3)
  {
    out[2] = (unsigned char)(((tmp[2] << 6) & 0xc0) | tmp[3]);
  }

  return numbytes;
}

/** Compute size of needed storage for encoding. This function computes the
 *  \e exact size of a memory area needed to hold the result of an encoding
 *  operation, not including the terminating null character.
 *
 * @param len input size
 * @returns output size
 *
 * @ingroup base64
 */
size_t base64_encoded_size(const size_t len)
{
  /* Base 64 represents 6 bits with each character (log_2(64)).
   *
   * Thus, the size calculation is:
   *
   *      encoded
   *
   *      len * 8
   *      -------
   *         6
   *
   * However, since we can only operate in whole bytes:
   *    __       __
   *    | len * 8 |
   *    | ------- |  or  ceil((len*8)/6)
   *    |    6    |
   *
   */
  return (size_t)ceil(((double)len * 8) / 6);
}

/** Compute size of needed storage for decoding. This function computes the
 *  \e estimated size of a memory area needed to hold the result of a decoding
 *  operation, not including the terminating null character. Note that this
 *  function may return up to two bytes more due to the nature of Base64.
 *
 * @param len input size
 * @returns output size
 *
 * @ingroup base64
 */
size_t base64_decoded_size(const size_t len)
{
  return (size_t)ceil(((double)len / 8) * 6);
}

/** Encode an arbitrary size memory area. This function encodes the first
 *  \c len bytes of the contents of the memory area pointed to by \c in and
 *  stores the result in the memory area pointed to by \c out. The result will
 *  be null-terminated.
 *
 * @attention This function can't check if there's enough space at the memory
 *            memory location pointed to by \c out, so be careful.
 *
 * @param out pointer to destination
 * @param in pointer to source
 * @param len input size in bytes
 * @param charset the base64 character set to use (e.g. urlsafe, regular)
 * @returns nothing
 *
 * @ingroup base64
 */
size_t base64_encode_binary(char *out, const unsigned char *in, const size_t len, const char charset[])
{
  char *out_start = out;
  int size = 4;
  size_t i = 0;

  while (len - i >= 4)
  {
    base64_encode_block((unsigned char*)out, in, size, charset);
    out += 4;
    in += 3;
    i += 3;
  }

  size = len - i;
  if (size)
  {
    base64_encode_block((unsigned char*)out, in, size, charset);
    out += size;
  }

  out++;
  return out - out_start;
}

/** Decode an arbitrary size memory area. This function decodes the
 *  base64-string pointed to by \c in and stores the result in the memory area
 *  pointed to by \c out. The result will \e not be null-terminated.
 *
 * @attention This function can't check if there's enough space at the
 *            memory location pointed to by \c out, so be careful.
 *
 * @param out pointer to destination
 * @param in pointer to source
 * @param len size in bytes of in
 * @returns -1 on error (illegal character) or the number of bytes decoded
 *
 * @ingroup base64
 */
int base64_decode_binary(unsigned char *out, const char *in , const size_t len)
{
  size_t i = 0;
  size_t size = 4;
  int numbytes = 0;
  int ret;

  while (len - i >= 4)
  {
    ret = base64_decode_block(out, (unsigned char*)in, size);
    if (ret == -1)
    {
      return -1;
    }
    numbytes += ret;
    out += 3;
    in += 4;
    i += 4;
  }

  size = len - i;
  if (size)
  {
    ret = base64_decode_block(out, (unsigned char*)in, size);
    if (ret == -1)
    {
      return -1;
    }
    numbytes += ret;
  }

  return numbytes;
}

/** Encode a string. This is a convenience function. It encodes the first
 *  \c size bytes of the string pointed to by \c in, stores the result
 *  into \c out.
 *
 * @attention This function can't check if there's enough space at the memory
 *            memory location pointed to by \c out, so be careful.
 *
 * @param out pointer to string to write encoded chars to
 * @param in pointer to string
 * @param len size in bytes of in
 * @returns length of out, not including terminating null character
 *
 * @ingroup base64
 */
int base64_encode(char *out, const char *in, const size_t len)
{
  size_t outlen;

  if (in == NULL)
  {
    return -1;
  }

  if (len == 0)
  {
    return -1;
  }

  outlen = base64_encode_binary(out, (unsigned char*)in, len, base64_list);
  return outlen;
}

/** Encode a string. This is a convenience function. It encodes the first
 *  \c size bytes of the string pointed to by \c in, stores the result
 *  into \c out.
 *
 *  This is the url safe variant which replaces the +/ with -_, respectively.
 *
 * @attention This function can't check if there's enough space at the memory
 *            memory location pointed to by \c out, so be careful.
 *
 * @param out pointer to string to write the encoded chars to
 * @param in pointer to string
 * @param len size in bytes of len
 * @returns NULL on error (not enough memory) or a pointer to the encoded result
 *
 * @ingroup base64
 */
int url_safe_base64_encode(char *out, const char *in, const size_t len)
{
  size_t outlen;

  if (in == NULL)
  {
    return -1;
  }

  if (len == 0)
  {
    return -1;
  }

  outlen = base64_encode_binary(out, (unsigned char *)in, len, url_safe_base64_list);
  return outlen;
}
