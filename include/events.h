#ifndef EVENTS_H_
#define EVENTS_H_

#include <stdint.h>

#define EVENT_BUFFER_CAPACITY 255

// Enumerates the different kinds of possible events.
typedef enum {
	EK_HEARTBEAT,
	EK_ENTITY_MOVED
} EventKind;

typedef struct {
	float delta_x, delta_y;
} EventEntityMoved;

// Encodes a game event.
typedef struct {
	uint16_t entity_id;
	uint16_t sequence_number;
	
	EventKind kind;
	union {
		EventEntityMoved entity_moved;
	};
} Event;

// A circular buffer that holds unprocessed events.
typedef struct {
	Event events[EVENT_BUFFER_CAPACITY + 1];
	uint8_t head; // The next position to read from.
	uint8_t tail; // The next position to write to.
} EventBuffer;

// Initializes a event buffer.
void event_buffer_init(EventBuffer *buffer);

// Returns the number of events in a given buffer.
int event_buffer_get_size(EventBuffer *buffer);

// Reads an event from a given buffer.
int event_buffer_read(EventBuffer *buffer, Event *event);

// Writes an event to a given buffer.
int event_buffer_write(EventBuffer *buffer, Event *event);

#endif // EVENTS_H_