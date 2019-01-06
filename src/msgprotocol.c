/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 21st, 2018
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "log.h"
#include "buffer.h"
#include "protocolbase.h"
#include "netinterface.h"
#include "msginterface.h"

#include "msgprotocol.h"

bool msgprotocol_handle_packet_recv(transport_conn_t *transport_conn, pkt_type_t pkt_type, buffer_t *buffer)
{
  bool success = false;
  switch (pkt_type)
  {
    default:
    {
      msgprotocol_handle_invalid_packet(PKT_DIRECTION_RECV, pkt_type);
      break;
    }
  }
  return success;
}

bool msgprotocol_handle_packet_send(transport_conn_t *transport_conn, pkt_type_t pkt_type, va_list args)
{
  bool success = false;
  switch (pkt_type)
  {
    default:
    {
      msgprotocol_handle_invalid_packet(PKT_DIRECTION_SEND, pkt_type);
      break;
    }
  }
  return success;
}

bool msgprotocol_handle_packet(transport_conn_t *transport_conn, pkt_direction_t pkt_direction, pkt_type_t pkt_type, ...)
{
  va_list args;
  va_start(args, pkt_type);
  bool success = false;
  switch (pkt_direction)
  {
    case PKT_DIRECTION_RECV:
    {
      buffer_t *buffer = va_arg(args, buffer_t*);
      success = msgprotocol_handle_packet_recv(transport_conn, pkt_type, buffer);
      break;
    }
    case PKT_DIRECTION_SEND:
    {
      success = msgprotocol_handle_packet_send(transport_conn, pkt_type, args);
      break;
    }
    default:
    {
      msgprotocol_handle_invalid_packet(pkt_direction, pkt_type);
      break;
    }
  }
  va_end(args);
  return success;
}

void msgprotocol_handle_invalid_packet(pkt_direction_t pkt_direction, pkt_type_t pkt_type)
{
  log_debug("Received unknown packet <direction=%s, pkt_type=%d>!", msgprotocol_get_packet_direction_str(pkt_direction), pkt_type);
}

bool msgprotocol_handle_incoming_packet(transport_conn_t *transport_conn, buffer_t *buffer)
{
  while (buffer_get_remaining_size(buffer) > 0)
  {
    pkt_type_t pkt_type = buffer_read_uint8(buffer);
    if (!msgprotocol_handle_packet(transport_conn, PKT_DIRECTION_RECV, pkt_type, buffer))
    {
      log_error("Failed to handle packet with packet type: %d!", pkt_type);
      return false;
    }
  }
  buffer_free(buffer);
  return true;
}

const char* msgprotocol_get_packet_direction_str(pkt_direction_t pkt_direction)
{
  switch (pkt_direction)
  {
    case PKT_DIRECTION_RECV:
      return "recv";
    case PKT_DIRECTION_SEND:
      return "send";
    default:
      return "unknown";
  }
}

const char* msgprotocol_get_packet_type_str(pkt_type_t pkt_type)
{
  switch (pkt_type)
  {
    default:
      return "unknown";
  }
}
