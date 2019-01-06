/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 8th, 2018
 */

#include <stdbool.h>
#include <string.h>

#include "sodium.h"
#include "aes.h"
#include "base64.h"

#include "crypto.h"

int crypto_get_cipher_size(int size)
{
  return crypto_box_MACBYTES + size;
}

int crypto_get_sign_size(int size)
{
  return crypto_sign_BYTES + size;
}

keypair_info_t* crypto_init_keypair(void)
{
  keypair_info_t *keypair_info = malloc(sizeof(keypair_info_t));
  return keypair_info;
}

void crypto_generate_nonce(keypair_info_t *keypair_info)
{
  randombytes_buf(keypair_info->nonce, sizeof(keypair_info->nonce));
}

keypair_info_t* crypto_generate_keypair(void)
{
  keypair_info_t *keypair_info = crypto_init_keypair();
  crypto_box_keypair(keypair_info->our_public_key, keypair_info->our_private_key);
  return keypair_info;
}

void crypto_free_keypair(keypair_info_t *keypair_info)
{
  free(keypair_info);
}

char* crypto_export_key(unsigned char *key)
{
  char *out = malloc(sizeof(key));
  url_safe_base64_encode(out, (const char*)key, sizeof(key));
  return out;
}

unsigned char* crypto_import_key(const char *encoded_key, int size)
{
  unsigned char *out = malloc(size);
  base64_decode_binary(out, encoded_key, size);
  return out;
}

void crypto_import_keypair(unsigned char *public_key, unsigned char *private_key, unsigned char *nonce,
  const char *encoded_public_key, const char *encoded_private_key, const char *encoded_nonce)
{
  // import public key
  unsigned char *decoded_public_key = crypto_import_key(encoded_public_key, strlen(encoded_public_key));
  memcpy(public_key, decoded_public_key, sizeof(&public_key));

  // import private key
  unsigned char *decoded_private_key = crypto_import_key(encoded_private_key, strlen(encoded_private_key));
  memcpy(private_key, decoded_private_key, sizeof(&private_key));

  // import nonce
  unsigned char *decoded_nonce = crypto_import_key(encoded_nonce, strlen(encoded_nonce));
  memcpy(nonce, decoded_nonce, sizeof(&nonce));
}

void crypto_import_our_keypair(keypair_info_t *keypair_info, const char *encoded_public_key, const char *encoded_private_key, const char *encoded_nonce)
{
  crypto_import_keypair(keypair_info->our_public_key, keypair_info->our_private_key, keypair_info->nonce, encoded_public_key, encoded_private_key, encoded_nonce);
}

void crypto_import_their_keypair(keypair_info_t *keypair_info, const char *encoded_public_key, const char *encoded_private_key, const char *encoded_nonce)
{
  crypto_import_keypair(keypair_info->their_public_key, keypair_info->their_private_key, keypair_info->nonce, encoded_public_key, encoded_private_key, encoded_nonce);
}
