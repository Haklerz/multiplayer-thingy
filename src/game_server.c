#include "game_server.h"

#include <stdio.h>
#include <string.h>

#define MAX_CLIENTS 1

static float current_time_s = 0.0;
static float next_heartbeat_s = 0.0;
static const float heartbeat_interval_s = 1.0;

static EventBuffer *client_buffers[MAX_CLIENTS];
static EventBuffer events_from_clients;

void server_init() {
	event_buffer_init(&events_from_clients);
	memset(client_buffers, 0, sizeof(client_buffers));
}

void server_update(const float dt) {
	current_time_s += dt;

	fprintf(stderr, "t=%.2fs Server: Updating...\n", current_time_s);

	// Process all events from clients.
	Event e;
	while (event_buffer_read(&events_from_clients, &e) == 0) {
		fprintf(stderr, "Server: Got heartbeat from client %d.\n", e.source_id);
	}

	if (current_time_s > next_heartbeat_s) {
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (!client_buffers[i])
				continue;

			fprintf(stderr, "Server: Requesting heartbeat from client %d.\n", i + 1);
			
			Event e = {
				.kind = EK_HEARTBEAT,
				.sequence_number = 0,
				.source_id = 0
			};
			event_buffer_write(client_buffers[i], &e);
		}

		next_heartbeat_s = current_time_s + heartbeat_interval_s;
	}
}

int server_submit_event(Event *event) {
	if (!event)
		return 1;
	
	event_buffer_write(&events_from_clients, event);
	return 0;
}

int server_connect(EventBuffer *client_buffer) {
	if (!client_buffer)
		return 1;
	
	client_buffers[0] = client_buffer;
	return 0;
}
