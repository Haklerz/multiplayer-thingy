#include "events.h"

// Sets the tail to the same index as the head to "empty" the buffer.
void event_buffer_init(EventBuffer *buffer) {
	if (!buffer)
		return;
	
	buffer->tail = buffer->head;
}

// Returns the difference of the tail and the head.
int event_buffer_get_size(EventBuffer *buffer) {
	if (!buffer)
		return 1;
	
	return buffer->tail - buffer->head;
}

int event_buffer_read(EventBuffer *buffer, Event *event) {
	if (!buffer)
		return 1;
	
	if (!event)
		return 2;
	
	if (event_buffer_get_size(buffer) == 0)
		return 3;
	
	*event = buffer->events[buffer->head++];
	return 0;
}

int event_buffer_write(EventBuffer *buffer, Event *event) {
	if (!buffer)
		return 1;
	
	if (!event)
		return 2;
	
	if (event_buffer_get_size(buffer) == EVENT_BUFFER_CAPACITY)
		return 3;
	
	buffer->events[buffer->tail++] = *event;
	return 0;
}