/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "sodium.h"
#include "log.h"
#include "dyad.h"
#include "version.h"
#include "buffer.h"
#include "aes.h"
#include "netbase.h"
#include "net.h"
#include "protocolbase.h"
#include "netinterface.h"
#include "keypairinterface.h"
#include "msginterface.h"
#include "msgprotocol.h"
#include "util.h"

#include "protocol.h"

bool write_connect_req(connection_t *connection, va_list args)
{
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_CONNECT_REQ);
  buffer_write_string(buffer, APPLICATION_VERSION, strlen(APPLICATION_VERSION));
  buffer_write_string(buffer, APPLICATION_RELEASE_NAME, strlen(APPLICATION_RELEASE_NAME));
  buffer_write_uint32(buffer, net_get_bind_port());
  handle_write_packet(connection, buffer);
  return true;
}

bool on_connect_req(connection_t *connection, buffer_t *buffer, va_list args)
{
  char *version_str = buffer_read_string(buffer);
  char *release_name_str = buffer_read_string(buffer);

  const char *address = dyad_getAddress(connection->remote);
  int port = buffer_read_uint32(buffer);

  // verify client version info
  if (!string_equals(version_str, APPLICATION_VERSION) || !string_equals(release_name_str, APPLICATION_RELEASE_NAME))
  {
    log_error("Failed to add new peer, invalid version info <version=%s, release_name=%s>!", version_str, release_name_str);
    return false;
  }

  // add the connection to our peer list
  peer_t *peer = add_peer(connection, address, port);
  if (!peer)
  {
    log_error("Failed to add already existant peer <%s:%d>!", address, port);
    return false;
  }

  free(version_str);
  free(release_name_str);

  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_CONNECT_RESP);
  return true;
}

bool write_connect_resp(connection_t *connection, va_list args)
{
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_CONNECT_RESP);
  buffer_write_uint32(buffer, net_get_bind_port());
  handle_write_packet(connection, buffer);
  return true;
}

bool on_connect_resp(connection_t *connection, buffer_t *buffer, va_list args)
{
  const char *address = dyad_getAddress(connection->remote);
  int port = buffer_read_uint32(buffer);

  // add the connection to our peer list
  peer_t *peer = add_peer(connection, address, port);
  if (!peer)
  {
    log_error("Failed to add already existant peer <%s:%d>!", address, port);
    return false;
  }

  // generate our keypair
  connection->keypair_info = crypto_generate_keypair();
  crypto_generate_nonce(connection->keypair_info);

  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_KEYPAIR_REQ);
  return true;
}

bool write_keypair_req(connection_t *connection, va_list args)
{
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_KEYPAIR_REQ);
  buffer_write_string(buffer, (const char*)connection->keypair_info->our_public_key, sizeof(connection->keypair_info->our_public_key));
  buffer_write_string(buffer, (const char*)connection->keypair_info->our_private_key, sizeof(connection->keypair_info->our_private_key));
  buffer_write_string(buffer, (const char*)connection->keypair_info->nonce, sizeof(connection->keypair_info->nonce));
  handle_write_packet(connection, buffer);
  return true;
}

bool on_keypair_req(connection_t *connection, buffer_t *buffer, va_list args)
{
  char *their_public_key = buffer_read_string(buffer);
  char *their_private_key = buffer_read_string(buffer);
  char *nonce = buffer_read_string(buffer);

  // generate our keypair
  connection->keypair_info = crypto_generate_keypair();

  // copy the keypair data to the struct
  memcpy(connection->keypair_info->their_public_key, their_public_key, sizeof(connection->keypair_info->their_public_key));
  memcpy(connection->keypair_info->their_private_key, their_private_key, sizeof(connection->keypair_info->their_private_key));
  memcpy(connection->keypair_info->nonce, nonce, sizeof(connection->keypair_info->nonce));

  free(their_public_key);
  free(their_private_key);
  free(nonce);

  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_KEYPAIR_RESP);
  connection->authenticated = true;
  connection->encrypted = true;
  return true;
}

bool write_keypair_resp(connection_t *connection, va_list args)
{
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_KEYPAIR_RESP);
  buffer_write_string(buffer, (const char*)connection->keypair_info->our_public_key, sizeof(connection->keypair_info->our_public_key));
  buffer_write_string(buffer, (const char*)connection->keypair_info->our_private_key, sizeof(connection->keypair_info->our_private_key));
  handle_write_packet(connection, buffer);
  return true;
}

bool on_keypair_resp(connection_t *connection, buffer_t *buffer, va_list args)
{
  char *their_public_key = buffer_read_string(buffer);
  char *their_private_key = buffer_read_string(buffer);

  // copy the keypair data to the struct
  memcpy(connection->keypair_info->their_public_key, their_public_key, sizeof(connection->keypair_info->their_public_key));
  memcpy(connection->keypair_info->their_private_key, their_private_key, sizeof(connection->keypair_info->their_private_key));

  free(their_public_key);
  free(their_private_key);

  connection->authenticated = true;
  connection->encrypted = true;
  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_PEERLIST_REQ);
  return true;
}

bool write_peerlist_req(connection_t *connection, va_list args)
{
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_PEERLIST_REQ);
  handle_write_packet(connection, buffer);
  return true;
}

bool on_peerlist_req(connection_t *connection, buffer_t *buffer, va_list args)
{
  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_PEERLIST_RESP);
  return true;
}

bool write_peerlist_resp(connection_t *connection, va_list args)
{
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_PEERLIST_RESP);
  if (!serialize_peerlist_to_buffer(buffer))
  {
    log_error("Failed to serialize peerlist to buffer!");
    return false;
  }
  handle_write_packet(connection, buffer);
  return true;
}

bool on_peerlist_resp(connection_t *connection, buffer_t *buffer, va_list args)
{
  if (!deserialize_peerlist_from_buffer(buffer))
  {
    log_error("Failed to deserialize peerlist from buffer!");
    return false;
  }
  return true;
}

bool write_relaymsg(connection_t *connection, va_list args)
{
  keypair_info_t *keypair_info = va_arg(args, keypair_info_t*);
  int data_size = va_arg(args, int);
  const char *data = va_arg(args, const char*);
  time_t timestamp = va_arg(args, time_t);

  // check to see if the msg has expired, if so don't relay it...
  if (get_msg_has_expired(timestamp))
  {
    return true;
  }

  // sign the data so they can identify the data before,
  // attempting to decrypt it...
  unsigned char signed_message[crypto_get_sign_size(data_size)];
  unsigned long long signed_message_len;

  if (crypto_sign(signed_message, &signed_message_len, (const unsigned char*)data,
    data_size, keypair_info->our_private_key) != 0)
  {
    log_error("Failed to sign message with private key!");
    return false;
  }

  // encrypt the data
  unsigned char ciphertext[crypto_get_cipher_size(data_size)];

  if (crypto_box_easy(ciphertext, (unsigned char*)data, data_size, keypair_info->nonce,
    keypair_info->our_public_key, keypair_info->our_private_key) != 0)
  {
    log_error("Failed to encrypt message with keypair!");
    return false;
  }
  buffer_t *buffer = buffer_init();
  buffer_write_uint8(buffer, PKT_TYPE_RELAYMSG);
  buffer_write_uint16(buffer, data_size);
  buffer_write_string(buffer, (const char*)signed_message, sizeof(signed_message));
  buffer_write_uint16(buffer, data_size);
  buffer_write_string(buffer, (const char*)ciphertext, sizeof(ciphertext));
  buffer_write_int32(buffer, timestamp);

  for (int i = 0; i <= get_next_peer_id(); i++)
  {
    peer_t *peer = get_peer_from_id(i);
    if (!peer)
    {
      continue;
    }

    buffer_t *other_buffer = buffer_init();
    buffer_copy(other_buffer, buffer);
    handle_write_packet(peer->connection, other_buffer);
  }
  return true;
}

bool on_relaymsg(connection_t *connection, buffer_t *buffer, va_list args)
{
  int signature_size = buffer_read_uint16(buffer);
  char *signature = buffer_read_string(buffer);

  int data_size = buffer_read_uint16(buffer);
  char *data = buffer_read_string(buffer);

  time_t timestamp = buffer_read_int32(buffer);

  // check to see if the msg has expired, if so don't unpack it...
  if (get_msg_has_expired(timestamp))
  {
    return true;
  }

  // find the keypair that was used to sign this signature,
  // we will use that keypair to decrypt the data as well...
  keypair_info_t *keypair_info = get_keypair_from_sig(signature_size, signature);
  if (keypair_info)
  {
    unsigned char decrypted[data_size];

    if (crypto_box_open_easy(decrypted, (unsigned char*)data, crypto_get_cipher_size(data_size),
      keypair_info->nonce, keypair_info->our_public_key, keypair_info->our_private_key) == 0)
    {
      transport_conn_t *transport_conn = get_transport_conn_from_keypair(keypair_info);
      if (!transport_conn)
      {
        transport_conn = add_transport_conn(keypair_info);
      }

      if (!get_msg_has_expired(timestamp))
      {
        unsigned char hash[crypto_generichash_BYTES];
        crypto_generichash(hash, sizeof(hash), decrypted, data_size, NULL, 0);
        const char *checksum = (const char*)hash;

        if (!has_msg_by_checksum(checksum))
        {
          pending_msg_t *pending_msg = add_msg(checksum, data_size, timestamp);
          buffer_t *buffer = buffer_init_data(0, (const unsigned char*)decrypted, data_size);
          msgprotocol_handle_incoming_packet(transport_conn, buffer);
        }
      }
    }
  }

  // always relay the message to our peers, in some cases we can decrypt the message,
  // which indicates the message is being sent to us. In order to reduce the chance that
  // one of our peers determine that we recv'd the msg, continue to relay it...
  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_RELAYMSG, data_size, data, timestamp);

  free(data);
  return true;
}

keypair_info_t* get_keypair_from_sig(int signature_size, const char* signature)
{
  for (int i = 0; i <= get_next_keypair_id(); i++)
  {
    keypair_storage_t *keypair_storage = get_keypair_from_id(i);
    if (!keypair_storage)
    {
      continue;
    }
    keypair_info_t *keypair_info = keypair_storage->keypair_info;

    unsigned char unsigned_message[signature_size];
    unsigned long long unsigned_message_len;

    if (crypto_sign_open(unsigned_message, &unsigned_message_len, (const unsigned char*)signature,
        crypto_get_sign_size(signature_size), keypair_info->our_public_key) == 0)
    {
      return keypair_info;
    }
  }
  return NULL;
}

bool handle_write_packet(connection_t *connection, buffer_t *other_buffer)
{
  buffer_t *buffer = buffer_init();
  int payload_size = buffer_get_size(other_buffer);
  const unsigned char *payload = buffer_get_data(other_buffer);

  if (connection->encrypted)
  {
    unsigned char ciphertext[crypto_get_cipher_size(payload_size)];
    int result = crypto_box_easy(ciphertext, (unsigned char*)payload, payload_size, connection->keypair_info->nonce,
      connection->keypair_info->our_public_key, connection->keypair_info->their_private_key);

    if (result != 0)
    {
      log_error("Failed to encrypt outgoing packet data with error code <%d>!", result);
      return false;
    }

    buffer_write_uint16(buffer, crypto_get_cipher_size(payload_size));
    buffer_write_uint16(buffer, payload_size);
    buffer_write(buffer, ciphertext, crypto_get_cipher_size(payload_size));
  }
  else
  {
    buffer_write_uint16(buffer, payload_size);
    buffer_write(buffer, payload, payload_size);
  }

  dyad_write(connection->remote, buffer_get_data(buffer), buffer_get_size(buffer));

  buffer_free(other_buffer);
  buffer_free(buffer);
  return true;
}

bool handle_packet_recv_authenticated(connection_t *connection, pkt_type_t pkt_type, buffer_t *buffer, va_list args)
{
  bool success = false;
  switch (pkt_type)
  {
    case PKT_TYPE_PEERLIST_REQ:
    {
      success = on_peerlist_req(connection, buffer, args);
      break;
    }
    case PKT_TYPE_PEERLIST_RESP:
    {
      success = on_peerlist_resp(connection, buffer, args);
      break;
    }
    case PKT_TYPE_RELAYMSG:
    {
      success = on_relaymsg(connection, buffer, args);
      break;
    }
    default:
    {
      handle_invalid_packet(PKT_DIRECTION_RECV, pkt_type);
      break;
    }
  }
  return success;
}

bool handle_packet_recv_unauthenticated(connection_t *connection, pkt_type_t pkt_type, buffer_t *buffer, va_list args)
{
  bool success = false;
  switch (pkt_type)
  {
    case PKT_TYPE_CONNECT_REQ:
    {
      success = on_connect_req(connection, buffer, args);
      break;
    }
    case PKT_TYPE_CONNECT_RESP:
    {
      success = on_connect_resp(connection, buffer, args);
      break;
    }
    case PKT_TYPE_KEYPAIR_REQ:
    {
      success = on_keypair_req(connection, buffer, args);
      break;
    }
    case PKT_TYPE_KEYPAIR_RESP:
    {
      success = on_keypair_resp(connection, buffer, args);
      break;
    }
    default:
    {
      handle_invalid_packet(PKT_DIRECTION_RECV, pkt_type);
      break;
    }
  }
  return success;
}

bool handle_packet_send(connection_t *connection, pkt_type_t pkt_type, va_list args)
{
  bool success = false;
  switch (pkt_type)
  {
    case PKT_TYPE_CONNECT_REQ:
    {
      success = write_connect_req(connection, args);
      break;
    }
    case PKT_TYPE_CONNECT_RESP:
    {
      success = write_connect_resp(connection, args);
      break;
    }
    case PKT_TYPE_KEYPAIR_REQ:
    {
      success = write_keypair_req(connection, args);
      break;
    }
    case PKT_TYPE_KEYPAIR_RESP:
    {
      success = write_keypair_resp(connection, args);
      break;
    }
    case PKT_TYPE_PEERLIST_REQ:
    {
      success = write_peerlist_req(connection, args);
      break;
    }
    case PKT_TYPE_PEERLIST_RESP:
    {
      success = write_peerlist_resp(connection, args);
      break;
    }
    case PKT_TYPE_RELAYMSG:
    {
      success = write_relaymsg(connection, args);
      break;
    }
    default:
    {
      handle_invalid_packet(PKT_DIRECTION_SEND, pkt_type);
      break;
    }
  }
  return success;
}

bool handle_packet(connection_t *connection, pkt_direction_t pkt_direction, pkt_type_t pkt_type, ...)
{
  va_list args;
  va_start(args, pkt_type);
  bool success = false;
  switch (pkt_direction)
  {
    case PKT_DIRECTION_RECV:
    {
      buffer_t *buffer = va_arg(args, buffer_t*);
      if (connection->authenticated)
      {
        success = handle_packet_recv_authenticated(connection, pkt_type, buffer, args);
        break;
      }
      else
      {
        success = handle_packet_recv_unauthenticated(connection, pkt_type, buffer, args);
        break;
      }
    }
    case PKT_DIRECTION_SEND:
    {
      success = handle_packet_send(connection, pkt_type, args);
      break;
    }
    default:
    {
      handle_invalid_packet(pkt_direction, pkt_type);
      break;
    }
  }
  va_end(args);
  return success;
}

void handle_invalid_packet(pkt_direction_t pkt_direction, pkt_type_t pkt_type)
{
  log_debug("Received unknown packet <direction=%s, pkt_type=%d>!", get_packet_direction_str(pkt_direction), pkt_type);
}

bool handle_incoming_packet(connection_t *connection, buffer_t *buffer)
{
  while (buffer_get_remaining_size(buffer) > 0)
  {
    pkt_type_t pkt_type = buffer_read_uint8(buffer);
    if (!handle_packet(connection, PKT_DIRECTION_RECV, pkt_type, buffer))
    {
      log_error("Failed to handle packet with packet type: %d!", pkt_type);
      return false;
    }
  }
  buffer_free(buffer);
  return true;
}

const char* get_packet_direction_str(pkt_direction_t pkt_direction)
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

const char* get_packet_type_str(pkt_type_t pkt_type)
{
  switch (pkt_type)
  {
    case PKT_TYPE_CONNECT_REQ:
      return "connect_req";
    case PKT_TYPE_CONNECT_RESP:
      return "connect_resp";
    case PKT_TYPE_KEYPAIR_REQ:
      return "keypair_req";
    case PKT_TYPE_KEYPAIR_RESP:
      return "keypair_resp";
    case PKT_TYPE_PEERLIST_REQ:
      return "peerlist_req";
    case PKT_TYPE_PEERLIST_RESP:
      return "peerlist_resp";
    case PKT_TYPE_RELAYMSG:
      return "relaymsg";
    default:
      return "unknown";
  }
}
