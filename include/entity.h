#ifndef ENTITY_H_
#define ENTITY_H_

#include "events.h"
#include <stdint.h>

typedef struct {
	uint16_t id; // The id of the entity.
	float world_x, world_y; // The entity's position within the world.
	EventBuffer world_events; // Buffer for receiving events from the world.
} Entity;

#endif // ENTITY_H_