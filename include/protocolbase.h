/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 21st, 2018
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum PacketDirection
{
  PKT_DIRECTION_RECV = 0,
  PKT_DIRECTION_SEND
} pkt_direction_t;

typedef enum PacketType
{
  PKT_TYPE_CONNECT_REQ = 0,
  PKT_TYPE_CONNECT_RESP,
  PKT_TYPE_KEYPAIR_REQ,
  PKT_TYPE_KEYPAIR_RESP,
  PKT_TYPE_PEERLIST_REQ,
  PKT_TYPE_PEERLIST_RESP,
  PKT_TYPE_RELAYMSG
} pkt_type_t;

#ifdef __cplusplus
}
#endif
