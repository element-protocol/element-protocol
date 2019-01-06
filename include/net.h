/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "dyad.h"
#include "queue.h"
#include "task.h"
#include "netbase.h"

#ifdef __cplusplus
extern "C"
{
#endif

static const char *net_bind_address = DEFAULT_ADDRESS;
static int net_bind_port = DEFAULT_PORT;
static int net_backlog = DEFAULT_BACKLOG;
static bool net_want_port_mapping = true;

static dyad_Stream *net_stream;

static queue_t *net_accept_queue;
static queue_t *net_connection_queue;

static task_t *net_poll_events_task;
static task_t *net_poll_resync_task;

bool net_init(int num_connection_entries, connection_entry_t connection_entries[]);
bool net_shutdown(void);

void net_set_bind_address(const char *address);
const char* net_get_bind_address(void);

void net_set_bind_port(int port);
int net_get_bind_port(void);

void net_set_backlog(int backlog);
int net_get_backlog(void);

void net_set_want_port_mapping(bool want_port_mapping);
bool net_get_want_port_mapping(void);

connection_t* net_init_connection(dyad_Stream *stream, dyad_Stream *remote);
void net_free_connection(connection_t *connection);
void net_setup_portmapping(int port);

void net_on_connect(dyad_Event *event);
void net_on_data(dyad_Event *event);
void net_on_close(dyad_Event *event);
void net_on_error(dyad_Event *event);
void net_on_accept(dyad_Event *event);

bool net_open_tcp_server(dyad_Stream *stream, const char *address, int port, size_t backlog);
bool net_open_tcp_connection(dyad_Stream *stream, const char *address, int port);

task_result_t net_poll_events(task_t *task, va_list args);
task_result_t net_poll_resync_peers(task_t *task, va_list args);

#ifdef __cplusplus
}
#endif
