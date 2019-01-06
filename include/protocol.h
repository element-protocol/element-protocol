/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "buffer.h"
#include "p2p.h"
#include "protocolbase.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool write_connect_req(connection_t *connection, va_list args);
bool write_connect_resp(connection_t *connection, va_list args);
bool write_keypair_req(connection_t *connection, va_list args);
bool write_keypair_resp(connection_t *connection, va_list args);
bool write_peerlist_req(connection_t *connection, va_list args);
bool write_peerlist_resp(connection_t *connection, va_list args);
bool write_relaymsg(connection_t *connection, va_list args);

bool on_connect_req(connection_t *connection, buffer_t *buffer, va_list args);
bool on_connect_resp(connection_t *connection, buffer_t *buffer, va_list args);
bool on_keypair_req(connection_t *connection, buffer_t *buffer, va_list args);
bool on_keypair_resp(connection_t *connection, buffer_t *buffer, va_list args);
bool on_peerlist_req(connection_t *connection, buffer_t *buffer, va_list args);
bool on_peerlist_resp(connection_t *connection, buffer_t *buffer, va_list args);
bool on_relaymsg(connection_t *connection, buffer_t *buffer, va_list args);

keypair_info_t* get_keypair_from_sig(int signature_size, const char* signature);

bool handle_write_packet(connection_t *connection, buffer_t *other_buffer);
bool handle_packet_recv_authenticated(connection_t *connection, pkt_type_t pkt_type, buffer_t *buffer, va_list args);
bool handle_packet_recv_unauthenticated(connection_t *connection, pkt_type_t pkt_type, buffer_t *buffer, va_list args);
bool handle_packet_send(connection_t *connection, pkt_type_t pkt_type, va_list args);
bool handle_packet(connection_t *connection, pkt_direction_t pkt_direction, pkt_type_t pkt_type, ...);
void handle_invalid_packet(pkt_direction_t pkt_direction, pkt_type_t pkt_type);
bool handle_incoming_packet(connection_t *connection, buffer_t *buffer);

const char* get_packet_direction_str(pkt_direction_t pkt_direction);
const char* get_packet_type_str(pkt_type_t pkt_type);

#ifdef __cplusplus
}
#endif
