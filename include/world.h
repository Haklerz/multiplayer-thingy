#ifndef WORLD_H_
#define WORLD_H_

#include "events.h"

typedef struct {
	uint16_t slot; // The slot of the entity.
	float world_x, world_y; // The entity's position within the world.
	EventBuffer world_events; // Buffer for receiving events from the world.
} Entity;

// Initializes the world.
void world_init();

// Updates the world.
void world_update(const float delta_time);

// Submits an event to the world's event buffer.
int world_submit_event(Event *event);

// Adds a entity to the world. Effectivly subscribes an entity to receiving events from the world.
int world_add_entity(Entity *entity);

#endif // WORLD_H_