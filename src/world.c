#include "world.h"

#include <stdio.h>
#include <string.h>

// Timing.
static float current_time = 0.0;
static float next_heartbeat_request = 0.0;
static const float heartbeat_request_interval = 1.0;

// Entities.
#define MAX_ENTITY_COUNT 10
static Entity *entitites[MAX_ENTITY_COUNT];

static EventBuffer world_event_buffer;

void world_init() {
	event_buffer_init(&world_event_buffer);
}

void world_update(const float delta_time) {
	current_time += delta_time;

	fprintf(stderr, "World t=%.2fs: Updating...\n", current_time);

	// Process all events from entities.
	Event event;
	while (event_buffer_read(&world_event_buffer, &event) == 0) {
		switch (event.kind) {

		case EK_HEARTBEAT:
			fprintf(stderr, "World: Got heartbeat from entity %d.\n", event.entity_slot);
			break;

		case EK_ENTITY_MOVED:
			fprintf(stderr, "World: Got move request from entity %d.\n", event.entity_slot);

			Event move_event = {
				.entity_slot = event.entity_slot,
				.kind = EK_ENTITY_MOVED,
				.sequence_number = event.sequence_number + 1,
				.entity_moved = {
					.delta_x = event.entity_moved.delta_x,
					.delta_y = event.entity_moved.delta_y
				}
			};

			for (int slot = 0; slot < MAX_ENTITY_COUNT; ++slot) {
				if (!entitites[slot])
					continue;
				
				event_buffer_write(&entitites[slot]->world_events, &move_event);
			}
			break;
		
		default:
			break;
		}
	}

	// Request heartbeats from all entities.
	if (current_time > next_heartbeat_request) {
		for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
			if (!entitites[i])
				continue;

			fprintf(stderr, "World: Requesting heartbeat from entity %d.\n", i);
			
			Event event = {
				.kind = EK_HEARTBEAT,
				.sequence_number = 0,
				.entity_slot = i
			};
			event_buffer_write(&entitites[i]->world_events, &event);
		}

		next_heartbeat_request = current_time + heartbeat_request_interval;
	}
}

int world_submit_event(Event *event) {
	return event_buffer_write(&world_event_buffer, event);
}

// Finds a free entity slot. Returns a non-zero value on error.
int get_free_entity_slot(uint16_t *slot) {

	for (int i = 0; i < MAX_ENTITY_COUNT; ++i) {
		if (entitites[i])
			continue;
		
		*slot = i;
		return 0;
	}

	return 1;
}

int world_add_entity(Entity *entity) {
	if (!entity)
		return 1;

	uint16_t slot;
	int result = get_free_entity_slot(&slot);
	if (result != 0)
		return result;
	
	entitites[slot] = entity;
	entity->slot = slot;

	return 0;
}
