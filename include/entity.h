#ifndef ENTITY_H_
#define ENTITY_H_

#include <stdint.h>

typedef enum {
	EK_NONE,
	EK_PLAYER,
	EK_NPC
} EntityKind;

typedef struct {
	struct sockaddr_storage client_address;
} PlayerData;

typedef struct {

} NpcData;

typedef struct {
	uint16_t slot; // The id of the entity.
	float world_x, world_y;
	
	EntityKind kind;
	union {
		PlayerData player_data;
		NpcData npc_data;
	};
	
} Entity;

#endif // ENTITY_H_