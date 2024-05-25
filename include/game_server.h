#ifndef GAME_SERVER_H_
#define GAME_SERVER_H_

#include "events.h"

// Initializes the server.
void server_init();

// Runs a ringle update (tick) step of the server.
void server_update(const float delta_time);

// Submits an event to the server's event buffer.
int server_submit_event(Event *event);

// Connects to the server.
int server_connect(EventBuffer *client_buffer);

#endif // GAME_SERVER_H_