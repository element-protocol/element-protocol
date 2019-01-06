/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 8th, 2018
 */

#pragma once

#include <stdbool.h>
#include <string.h>

#include "sodium.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct KeypairInfo
{
  unsigned char our_public_key[crypto_box_PUBLICKEYBYTES];
  unsigned char our_private_key[crypto_box_SECRETKEYBYTES];
  unsigned char their_public_key[crypto_box_PUBLICKEYBYTES];
  unsigned char their_private_key[crypto_box_SECRETKEYBYTES];
  unsigned char nonce[crypto_box_NONCEBYTES];
} keypair_info_t;

int crypto_get_cipher_size(int size);
int crypto_get_sign_size(int size);

keypair_info_t* crypto_init_keypair(void);
void crypto_generate_nonce(keypair_info_t *keypair_info);
keypair_info_t* crypto_generate_keypair(void);
void crypto_free_keypair(keypair_info_t *keypair_info);

char* crypto_export_key(unsigned char *key);
unsigned char* crypto_import_key(const char *encoded_key, int size);

void crypto_import_keypair(unsigned char *public_key, unsigned char *private_key, unsigned char* nonce,
  const char *encoded_public_key, const char *encoded_private_key, const char *encoded_nonce);

void crypto_import_our_keypair(keypair_info_t *keypair_info, const char *encoded_public_key, const char *encoded_private_key, const char *encoded_nonce);
void crypto_import_their_keypair(keypair_info_t *keypair_info, const char *encoded_public_key, const char *encoded_private_key, const char *encoded_nonce);

#ifdef __cplusplus
}
#endif
