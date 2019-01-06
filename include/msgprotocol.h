/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 21st, 2018
 */

#pragma once

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "buffer.h"
#include "protocolbase.h"
#include "netinterface.h"
#include "msginterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool msgprotocol_handle_packet_recv(transport_conn_t *transport_conn, pkt_type_t pkt_type, buffer_t *buffer);
bool msgprotocol_handle_packet_send(transport_conn_t *transport_conn, pkt_type_t pkt_type, va_list args);

bool msgprotocol_handle_packet(transport_conn_t *transport_conn, pkt_direction_t pkt_direction, pkt_type_t pkt_type, ...);
void msgprotocol_handle_invalid_packet(pkt_direction_t pkt_direction, pkt_type_t pkt_type);
bool msgprotocol_handle_incoming_packet(transport_conn_t *transport_conn, buffer_t *buffer);

const char* msgprotocol_get_packet_direction_str(pkt_direction_t pkt_direction);
const char* msgprotocol_get_packet_type_str(pkt_type_t pkt_type);

#ifdef __cplusplus
}
#endif
